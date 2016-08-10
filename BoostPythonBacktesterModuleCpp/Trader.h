#pragma once

class SngleTraderMarket;

class Trader {
public:
	Trader();
	virtual void tickActions(std::vector<Match> matcher, AnonimousMaketDepth depth) = 0;
	virtual void newCandelsAction() = 0;

	void recieveTickData(std::vector<Match> matcher, AnonimousMaketDepth depth);
	void recieveCandels(std::vector<Candel> candels);

	void setMarket(SngleTraderMarket* market);

	virtual ~Trader();

private:
	SngleTraderMarket* assigned_market;
};

