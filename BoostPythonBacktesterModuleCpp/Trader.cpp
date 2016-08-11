#include "stdafx.h"
#include "Backtester.h"


Trader::Trader() {
}

void Trader::recieveTickData(std::vector<Match> matcher, std::unordered_map<std::string, AnonimousMaketDepth> depth) {
}

void Trader::recieveCandels(std::vector<Candel> candels) {
}

void Trader::setMarket(SngleTraderMarket * market) {
}


Trader::~Trader() {
}
