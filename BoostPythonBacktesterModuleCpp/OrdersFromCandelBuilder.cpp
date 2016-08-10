#include "stdafx.h"
#include "Backtester.h"


std::vector<Order> OrdersFromCandelBuilder::ordersFromCandel(Candel candel, std::string ticker, int volumePerPrice,
															 int depthLevel) {
	std::vector<Order> orders;
	for (int price = candel.close - depthLevel; price < candel.close; price++) {
		orders.push_back(Order::Make_Limit_Order("history", ticker, volumePerPrice, price));
	}
	for (int price = candel.close + depthLevel; price > candel.close; price--) {
		orders.push_back(Order::Make_Limit_Order("history", ticker, -volumePerPrice, price));
	}
	return std::vector<Order>();
}
