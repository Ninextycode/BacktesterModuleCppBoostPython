#include "stdafx.h"
#include "Backtester.h"

const std::string  OrdersFromCandleBuilder::HISTORY_IDENTIFIER = "history";

std::vector<Order> OrdersFromCandleBuilder::ordersFromCandle(Candle candle, std::string ticker, int volumePerPrice,
															 int depthLevel) {
	std::vector<Order> orders;
	for (int price = candle.close - depthLevel; price < candle.close; price++) {
		orders.push_back(Order::Make_Limit_Order(HISTORY_IDENTIFIER, ticker, volumePerPrice, price));
	}
	for (int price = candle.close + depthLevel; price > candle.close; price--) {
		orders.push_back(Order::Make_Limit_Order(HISTORY_IDENTIFIER, ticker, -volumePerPrice, price));
	}
	return orders;
}
