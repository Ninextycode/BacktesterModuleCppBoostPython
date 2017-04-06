#pragma once

class SingleTraderMarket;

class Trader {
public:
	Trader(std::string identifier);
	virtual ~Trader();

	
	virtual void newTickAction(std::vector<OrderChange> matcher, 
				std::unordered_map<std::string, AnonimousMaketDepthVector> depth) = 0;
	
	void recieveTickData(std::vector<OrderChange> matcher, 
						 std::unordered_map<std::string, AnonimousMaketDepthVector> depths);

	virtual void newCandlesAction(std::string ticker, std::vector<Candle> candles) = 0;

	void recieveCandles(std::string ticker, std::vector<Candle> candles);

	void setMarket(SingleTraderMarket* market);
	void changeOrder(Order order);
	void makeOrder(Order order);
	void requestCandles(std::string ticker, int length);

	const std::unordered_map<std::string, int>& getPortfio();
	const std::unordered_map<std::string, std::vector<float>>& getPerformance();


	Order createLimitOrder(std::string ticker, int volume, int price);
	Order createMarketOrder(std::string ticker, int volume);
	Order createIoCOrder(std::string ticker, int volume, int price);
	std::unordered_map<std::string, std::unordered_map<int, int>> orders;
protected:
	
	// ticker - price - volume map
	
	SingleTraderMarket* market;
	std::string identifier;
};

