#include "stdafx.h"
#include "Backtester.h"


Trader::Trader(std::string identifier) {
	if (identifier == OrdersFromCandleBuilder::HISTORY_IDENTIFIER) {
		identifier += "_";
	}
	this->identifier = identifier;
}

void Trader::recieveTickData(std::vector<OrderChange> changes,
							 std::unordered_map<std::string, AnonimousMaketDepth> depth) {
	for (OrderChange change : changes) {
		this->orders[change.ticker][change.worstPossiblePrice] += change.volumeChange;
		if (orders[change.ticker][change.worstPossiblePrice] == 0) {
			orders[change.ticker].erase(change.worstPossiblePrice);
		}
	}
	
	this->newTickAction(changes, depth);
}

void Trader::recieveCandles(std::string ticker, std::vector<Candle> candles) {
	this->newCandlesAction(ticker, candles);
}

void Trader::makeOrder(Order order) {
	this->market->addOrder(order);
}

const std::unordered_map<std::string, int>& Trader::getPortfio() {
	return this->market->getPortfio();
}

const std::unordered_map<std::string, std::vector<float>>& Trader::getPerformance() {
	return this->market->getPerformance();
}

void Trader::changeOrder(Order order) {
	this->market->changeOrder(order);
}


void Trader::setMarket(SingleTraderMarket * market) {
	this->market = market;
}


Order Trader::createLimitOrder(std::string ticker, int volume, int price) {
	return Order::Make_Limit_Order(this->identifier, ticker, volume, price);
}

Order Trader::createMarketOrder(std::string ticker, int volume) {
	Order order = Order::Make_Limit_Order(this->identifier, ticker, volume, (volume > 0?INT_MAX:0) );
	order.orderType = OrderType::marketOrder;
	return order;
}

Order Trader::createIoCOrder(std::string ticker, int volume, int price) {
	Order order = Order::Make_IoC_Order(this->identifier, ticker, volume, price);
	return order;
}

void Trader::requestCandles(std::string ticker, int length) {
	this->market->requestCandles(ticker, length);
}

Trader::~Trader() {
}
