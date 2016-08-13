#pragma once
class OrdersFromCandelBuilder {
public:
	static const int VOLUME_PER_PRICE = 5;
	static const int DEPTH_LEVEL = 5;
	static  const std::string  HISTORY_IDENTIFIER;
	static std::vector<Order> ordersFromCandel(Candel candel,
											   std::string ticker,
											   int volumePerPrice= VOLUME_PER_PRICE,
											   int depthLevel = DEPTH_LEVEL);
};

