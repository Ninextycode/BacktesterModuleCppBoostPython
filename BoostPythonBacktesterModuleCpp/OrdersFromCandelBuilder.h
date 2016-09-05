#pragma once
class  OrdersFromCandleBuilder {
public:
	static const int VOLUME_PER_PRICE = 10;
	static const int DEPTH_LEVEL = 100;
	static const std::string  HISTORY_IDENTIFIER;
	static std::vector<Order> ordersFromCandle(Candle candle,
											   std::string ticker,
											   int volumePerPrice= VOLUME_PER_PRICE,
											   int depthLevel = DEPTH_LEVEL);
};

