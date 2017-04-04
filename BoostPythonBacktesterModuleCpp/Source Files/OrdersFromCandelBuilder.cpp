#include "../Header Files/Backtester.h"

const std::string  OrdersFromCandleBuilder::HISTORY_IDENTIFIER = "history";

// Create orders that we expect were present at the time of this candle
std::vector<Order> OrdersFromCandleBuilder::ordersFromCandle(Candle candle, std::string ticker, int volumePerPrice,
															 int depthLevel) {
	std::vector<Order> orders;
	for (int i = depthLevel; i > 0; i--) {
		orders.push_back(Order::Make_Limit_Order(HISTORY_IDENTIFIER, ticker, volumePerPrice, candle.close - i * 100));
	}
	for (int i = 1; i <= depthLevel; i++) {
		orders.push_back(Order::Make_Limit_Order(HISTORY_IDENTIFIER, ticker, -volumePerPrice, candle.close + i * 100));
	}

	return orders;
}
