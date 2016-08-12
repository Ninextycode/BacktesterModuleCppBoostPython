#pragma once

#ifdef DLL_COMPILE
#define DECLSPEC __declspec(dllexport)
#else
#define DECLSPEC __declspec(dllimport)
#endif

#include <unordered_map>
#include <string>
#include <list>
#include <ostream>
#include <algorithm>
#include <fstream>

#include "boost/date_time/posix_time/posix_time.hpp"

#include "DataStructures.h"
#include "MarketDepth.h"

#include "Trader.h"
#include "Market.h"

#include "OrdersFromCandelBuilder.h"