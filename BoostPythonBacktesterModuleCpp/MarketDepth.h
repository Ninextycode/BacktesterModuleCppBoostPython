#pragma once

class DECLSPEC MarketDepth {
public:
	MarketDepth(std::string ticker = "stock");
	
	~MarketDepth();

	void addOrder(Order o);
	void removeOrder(int id);
	AnonimousMaketDepth const * getAnonimousDepth(int depthLength);
	MarketDepthData const & getInternalDepth();
private:
	std::vector<int> getUnsortedPrices();

	void tryMatchOrder(Order *order);
	void tryMatchBuyOrder(Order *order);
	void tryMatchSellOrder(Order *order);
	void tryMatchOrderWithExactPriceOrders(Order *order,
								int price);

	std::string* trader_identifier;
	std::vector<Match>* currentMatches;
	std::string* ticker;
	MarketDepthData  totalDepth;
};
