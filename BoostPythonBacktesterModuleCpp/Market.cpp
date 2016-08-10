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

void SngleTraderMarket::cancelOrder(std::string trades_identifier, std::string ticker, int desiredVolume, int price) {
}

void SngleTraderMarket::tick() {
}

void SngleTraderMarket::send_data_to_traders(AnonimousMaketDepth & depth, std::vector<Match> matches) {
}

void loadHistoryData(std::string path) {
}
