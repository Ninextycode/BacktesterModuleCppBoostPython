#pragma once


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

#define max(a, b) ((a > b) ? a : b)
#define min(a, b) ((a < b) ? a : b)