#pragma once

class SngleTraderMarket;

class DECLSPEC Trader {
public:
	Trader(std::string identifier);
	virtual void tickAction(std::vector<OrderChange> matcher, 
				std::unordered_map<std::string, AnonimousMaketDepth> depth) = 0;
	void recieveTickData(std::vector<OrderChange> matcher, 
						 std::unordered_map<std::string, AnonimousMaketDepth> depths);

	void recieveCandels(std::string ticker, std::vector<Candel> candels);
	virtual void newCandelsAction(std::string ticker, std::vector<Candel> candels) = 0;
	void setMarket(SngleTraderMarket* market);
	void changeOrder(Order order);
	void makeOrder(Order order);

	virtual ~Trader();

protected:
	Order createLimitOrder(std::string ticker, int volume, int price);
	// ticker - price - volume map
	std::unordered_map<std::string, std::unordered_map<int, int>> orders;
	SngleTraderMarket* market;
	std::string identifier;
};

