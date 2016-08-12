#pragma once

class DECLSPEC SngleTraderMarket {
public:
	SngleTraderMarket();
	~SngleTraderMarket();

	void setTrader(Trader* trader);
	void loadHistoryData(std::string path);
	void runFullTest();
	
	void addOrder(Order order);


	void changeOrder(Order order);


	void updateDepths();
	
private:
	int currentTick = 0;
	void tick();
	void sendAndCleanTickData();
	void addAndCleanOrders();
	void sendAndCleanCandles();

	std::vector<Order> ordersToAdd;
	std::vector<OrderChange> changesToSend;
	std::vector<Candel> candelsToSend;
	std::unordered_map<std::string, std::vector<Candel>> historyData;
	std::unordered_map<std::string, std::vector<int>> portfolio;
	Trader* trader;

	std::vector<std::string> tickers;

	std::unordered_map<std::string, MarketDepth> depths;
	std::unordered_map<std::string, AnonimousMaketDepth> anonimousDepths;
};

