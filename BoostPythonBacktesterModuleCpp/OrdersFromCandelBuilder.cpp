#include "stdafx.h"
#include "Backtester.h"

const std::string  OrdersFromCandelBuilder::HISTORY_IDENTIFIER = "history";

std::vector<Order> OrdersFromCandelBuilder::ordersFromCandel(Candel candel, std::string ticker, int volumePerPrice,
															 int depthLevel) {
	std::vector<Order> orders;
	for (int price = candel.close - depthLevel; price < candel.close; price++) {
		orders.push_back(Order::Make_Limit_Order(HISTORY_IDENTIFIER, ticker, volumePerPrice, price));
	}
	for (int price = candel.close + depthLevel; price > candel.close; price--) {
		orders.push_back(Order::Make_Limit_Order(HISTORY_IDENTIFIER, ticker, -volumePerPrice, price));
	}
	return orders;
}
