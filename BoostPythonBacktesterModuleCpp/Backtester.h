#pragma once

#define ONWINDOWS


#ifdef ONWINDOWS
#ifdef DLL_COMPILE
#define DECLSPEC __declspec(dllexport)
#else
#define DECLSPEC __declspec(dllimport)
#endif
#else
#define DECLSPEC 
#endif

#include <unordered_map>
#include <string>
#include <list>
#include <ostream>
#include <algorithm>
#include <fstream>

#include "DataStructures.h"
#include "MarketDepth.h"

#include "Trader.h"
#include "Market.h"

#include "OrdersFromCandelBuilder.h"