#pragma once

class SngleTraderMarket;

class Trader {
public:
	Trader();
	virtual void tickActions(std::vector<OrderChange> matcher, AnonimousMaketDepth depth) = 0;
	virtual void newCandelsAction(std::vector<Candel> candels) = 0;

	void recieveTickData(std::vector<OrderChange> matcher, std::unordered_map<std::string, AnonimousMaketDepth> depth);
	void recieveCandels(std::vector<Candel> candels);

	void setMarket(SngleTraderMarket* market);


	virtual ~Trader();

private:
	// ticker - price - volume map
	std::unordered_map<std::string, std::unordered_map<int, int>> orders;
	SngleTraderMarket* assigned_market;
};

