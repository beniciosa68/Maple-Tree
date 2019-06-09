#include "downloadmanager.h"

DownloadManager* DownloadManager::self;

DownloadManager::DownloadManager(QObject* parent) : QObject(parent) {
	DownloadManager::self = this;
}

QFile* DownloadManager::downloadSingle(const QUrl& url, const QString& filepath, QString msg) {
	downloadQueue.enqueue({ filepath, url });
	++totalCount;
	block = true;
	if (!msg.isEmpty())
        qInfo() << msg;
	_startNextDownload();
	_downloadFinished();
    output.close();
    return new QFile(filepath);
}

void DownloadManager::append(const QUrl& url, const QString& filepath) {
	if (downloadQueue.isEmpty())
		QTimer::singleShot(0, Qt::CoarseTimer, this, &DownloadManager::_startNextDownload);

	downloadQueue.enqueue({ filepath, url });
	++totalCount;
}

DownloadManager* DownloadManager::getSelf() { return self; }

void DownloadManager::_startNextDownload() {
	if (downloadQueue.isEmpty()) {
		emit downloadFinished(downloadedCount, totalCount);
		return;
	}

	QPair<QString, QUrl> pair = downloadQueue.dequeue();
	QString filename = pair.first;
	QString dir(QFileInfo(filename).dir().path());
	QUrl url = pair.second;
	QDir().mkdir(dir);

	output.setFileName(filename);
	if (!output.open(QIODevice::WriteOnly)) {
		emit downloadError("_startNextDownload():" + output.errorString());
		emit downloadError("_startNextDownload():" + filename);
		emit downloadError("_startNextDownload():" + url.url());
		_startNextDownload();
		return;
	}

	currentDownload = manager.get(QNetworkRequest(url));
	connect(currentDownload, &QNetworkReply::readyRead, this, &DownloadManager::_downloadReadyRead);
	connect(currentDownload, &QNetworkReply::downloadProgress, this, &DownloadManager::_downloadProgress);

	downloadTime.start();
	emit downloadStarted(filename);

	QEventLoop loop;
	if (block) {
		connect(currentDownload, &QNetworkReply::finished, &loop, &QEventLoop::quit);
		loop.exec();
		block = false;
	}
	else {
		connect(currentDownload, &QNetworkReply::finished, this, &DownloadManager::_downloadFinished);
	}
}

void DownloadManager::_downloadProgress(qint64 bytesReceived, qint64 bytesTotal) {
	emit downloadProgress(bytesReceived, bytesTotal, downloadTime);
}

void DownloadManager::_downloadFinished() {
	try {
		output.close();

        if (currentDownload->error()) {
            qCritical() << currentDownload->errorString();
		}
		else {
			if (isHttpRedirect()) {
				reportRedirect();
				output.remove();
			}
			else {
				emit downloadSuccessful(output.fileName());
				++downloadedCount;
			}
		}

		currentDownload->deleteLater();
		_startNextDownload();
	}
	catch (std::errc) {
        qCritical() << currentDownload->errorString();
        qCritical() << output.errorString();
        qCritical() << output.fileName();
        qCritical() << currentDownload->url().url();
		output.remove();
	}
	emit downloadProgress(0, 100, downloadTime);
}

void DownloadManager::_downloadReadyRead() {
	try {
		if (output.isWritable() && currentDownload->isReadable()) {
			output.write(currentDownload->readAll());
		}
	}
    catch (std::errc) {
        qCritical() << output.errorString();
	}
}

bool DownloadManager::isHttpRedirect() const {
	int statusCode =
		currentDownload->attribute(QNetworkRequest::HttpStatusCodeAttribute)
		.toInt();
	return statusCode == 301 || statusCode == 302 || statusCode == 303 ||
		statusCode == 305 || statusCode == 307 || statusCode == 308;
}

void DownloadManager::reportRedirect() {
	QUrl requestUrl = currentDownload->request().url();

	QVariant target =
		currentDownload->attribute(QNetworkRequest::RedirectionTargetAttribute);
	if (!target.isValid())
		return;

	QUrl redirectUrl = target.toUrl();
	if (redirectUrl.isRelative())
		redirectUrl = requestUrl.resolved(redirectUrl);
}
