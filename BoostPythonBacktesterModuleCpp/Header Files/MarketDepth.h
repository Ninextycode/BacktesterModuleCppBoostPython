#pragma once

class MarketDepth {
public:
	MarketDepth(std::string ticker = "stock");
	
	~MarketDepth();

	std::vector<OrderChange> addOrder(Order o);
	/* removes all previous orders of current trader */
	std::vector<OrderChange> changeOrder(Order order);
	AnonimousMaketDepthVector  getAnonimousDepth(int depthLength);
	const MarketDepthDataMap & getInternalDepth();
	void clearHistoryOrders();
private:
	std::vector<int> getUnsortedPrices();

	void tryMatchOrder(Order& order);
	void tryMatchBuyOrder(Order& order);
	void tryMatchSellOrder(Order& order);
	void tryMatchOrderWithExactPriceOrders(Order& order,
								int price);
	void workAroundCheckedOrder(Order& order);


	void addChangeIfItIsNotHistory(OrderChange change);

	void removeOrdersOfSuchTraderWithSuchPrice(Order order);
	std::vector<OrderChange> currentChanges;
	std::string ticker;
	MarketDepthDataMap  totalDepth;
};
