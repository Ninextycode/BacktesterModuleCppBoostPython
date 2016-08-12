#include "stdafx.h"
#include "Backtester.h"


SngleTraderMarket::SngleTraderMarket() {
}


SngleTraderMarket::~SngleTraderMarket() {
}

void SngleTraderMarket::setTrader(Trader * trader) {
	this->trader = trader;
}

void SngleTraderMarket::loadHistoryData(std::string path) {
}

void SngleTraderMarket::runFullTest() {
}

void SngleTraderMarket::addOrder(Order order) {
}

void SngleTraderMarket::changeOrder(Order order) {
	//depths[ticker].cancelOrder(trades_identifier, );
}

void SngleTraderMarket::tick() {
	this->updateDepths();
	this->addAndCleanOrders();
	this->sendAndCleanTickData();
	this->sendAndCleanCandles();
}

void SngleTraderMarket::updateDepths() {
	for (auto ticker : tickers) {
		auto currentCandle = historyData[ticker][currentTick];
		auto orders = OrdersFromCandelBuilder::ordersFromCandel(currentCandle, ticker);
		for (Order order : orders) {
			depths[ticker].addOrder(order);
		}
	}
}

void SngleTraderMarket::addAndCleanOrders() {
	for (Order order : this->ordersToAdd) {
		auto desiderDepth = this->depths.find(order.ticker);
		desiderDepth->second.addOrder(order);		
	}
	ordersToAdd.clear();
}

void SngleTraderMarket::sendAndCleanTickData() {
	this->trader->recieveTickData(changesToSend, anonimousDepths);
	changesToSend.clear();
}

void SngleTraderMarket::sendAndCleanCandles() {
	this->trader->recieveCandels(candelsToSend);
	candelsToSend.clear();
}

void loadHistoryData(std::string path) {
}
