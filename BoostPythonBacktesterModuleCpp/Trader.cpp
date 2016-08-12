#include "stdafx.h"
#include "Backtester.h"


Trader::Trader() {
}

void Trader::recieveTickData(std::vector<OrderChange> changes,
							 std::unordered_map<std::string, AnonimousMaketDepth> depth) {
	for (OrderChange change : changes) {
		this->orders[change.ticker][change.price] += change.volumeChange;
	}
	this->recieveTickData(changes, depth);
}

void Trader::recieveCandels(std::vector<Candel> candels) {
	this->newCandelsAction(candels);
}

void Trader::setMarket(SngleTraderMarket * market) {
	this->assigned_market = market;
}


Trader::~Trader() {
}
