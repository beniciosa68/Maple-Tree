// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <unordered_map>
#include <stdio.h>
#include <tchar.h>
#include <iostream>
#include <fstream>
#include <thread>
#include <cstdint>
#include <filesystem>
#include <conio.h>
#include <vector>
#include <map>
#include <future>
#include <string>
#include <stdarg.h>
#include <bitset>
#include <iomanip>
#include <algorithm>
#include <limits>
using namespace std;

typedef std::unordered_map<std::string, std::thread> ThreadMap;

//Third Party libraries
#include <boost/thread.hpp>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/beast/core.hpp>
#include <boost/progress.hpp>
#include <boost/asio.hpp>
using namespace boost::filesystem;
namespace po = boost::program_options;

#include <nana/gui.hpp>
#include <nana/gui/place.hpp>
#include <nana/threads/pool.hpp>
#include <nana/gui/widgets/label.hpp>
#include <nana/gui/widgets/button.hpp>
#include <nana/gui/widgets/picture.hpp>
#include <nana/gui/widgets/progress.hpp>
#include <nana/gui/widgets/listbox.hpp>
#pragma comment (lib, "nana_v141_Debug_x64.lib")
using namespace nana;

#include "ThirdParty\cdecrypt.cpp"
#include "ThirdParty\frozen.h"
#include "ThirdParty\rang.hpp"

#include <windows.h>
#include <Wininet.h>
#pragma comment (lib, "Wininet.lib")

//Project includes
#include "gen-versioninfo.h"
#include "ThreadManager.h"
#include "DownloadClient.h"
#include "TitleInfo.h"
#include "MapleTicket.h"
#include "MapleMini.h"
#include "UIMain.h"
#include "Tools.h"
#include "Library.h"

using namespace MapleSeed;
using namespace CommonTools;