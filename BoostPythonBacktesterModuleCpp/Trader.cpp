#include "stdafx.h"
#include "Backtester.h"


Trader::Trader(std::string identifier) {
	if (identifier == OrdersFromCandelBuilder::HISTORY_IDENTIFIER) {
		identifier += "_";
	}
	this->identifier = identifier;
}

void Trader::recieveTickData(std::vector<OrderChange> changes,
							 std::unordered_map<std::string, AnonimousMaketDepth> depth) {
	for (OrderChange change : changes) {
		this->orders[change.ticker][change.price] += change.volumeChange;
	}
	this->tickAction(changes, depth);
}

void Trader::recieveCandels(std::string ticker, std::vector<Candel> candels) {
	this->newCandelsAction(ticker, candels);
}

void Trader::makeOrder(Order order) {
	this->market->addOrder(order);
}

void Trader::changeOrder(Order order) {
	this->market->changeOrder(order);
}


void Trader::setMarket(SngleTraderMarket * market) {
	this->market = market;
}


Order Trader::createLimitOrder(std::string ticker, int volume, int price) {
	return Order::Make_Limit_Order(this->identifier, ticker, volume, price);
}

Trader::~Trader() {
}
