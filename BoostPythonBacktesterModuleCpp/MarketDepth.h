#pragma once

class DECLSPEC MarketDepth {
public:
	MarketDepth(std::string ticker = "stock");
	
	~MarketDepth();

	std::vector<OrderChange> addOrder(Order o);
	/* removes all previous orders of current trader */
	std::vector<OrderChange> changeOrder(Order order);
	AnonimousMaketDepth const * getAnonimousDepth(int depthLength);
	MarketDepthData const & getInternalDepth();
private:
	std::vector<int> getUnsortedPrices();

	void tryMatchOrder(Order *order);
	void tryMatchBuyOrder(Order *order);
	void tryMatchSellOrder(Order *order);
	void tryMatchOrderWithExactPriceOrders(Order *order,
								int price);

	void removeOrdersOfSuchTraderWithSuchPrice(Order order);
	std::vector<OrderChange> currentChanges;
	std::string* ticker;
	MarketDepthData  totalDepth;
};
