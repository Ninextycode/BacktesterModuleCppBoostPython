#pragma once

typedef std::unordered_map<std::string, std::vector<Candle>> CandlesVectorMap;

class SingleTraderMarket {
public:
	int DEPTH_LENGTH_PUBLIC = 20;
	
	SingleTraderMarket();
	~SingleTraderMarket();

	void setTrader(Trader* trader);
	void loadHistoryData(std::string path, std::vector<std::string> tickers);
	void runFullTest();
	
	void addOrder(Order order);

	void changeOrder(Order order);

	void requestCandles(std::string ticker, int length);

	const CandlesVectorMap & getInternalHistoryCandles();

	const std::unordered_map<std::string, int>& getPortfio();
	const std::unordered_map<std::string, std::vector<float>> & getPerformance();
	void setComission(std::string ticker, int comission);

	int numberOfTics = -1;
	int currentTick = 0;
private:

	

	void tick();
	void updateDepths();
	void sendAndCleanTickData();
	void addChangeAndCleanOrders();
	void sendAndCleanCandles();
	void addToChangesToSend(std::vector<OrderChange>& changes);
	void clearDepthFromHistoryOrders(std::string ticker);
	void updatePortfolio();
	void addToPerformance();
	void preparePerformance();
	void changeAndCleanOrders();
	void addAndCleanOrders();

	std::vector<Order> ordersToAdd;
	std::vector<Order> ordersToChange;
	std::vector<OrderChange> changesToSend;
	CandlesVectorMap candlesToSend;
	CandlesVectorMap historyData;
	std::unordered_map<std::string, int> portfolio;
	std::unordered_map<std::string, std::vector<float>> performance;
	Trader* trader;

	std::vector<std::string> tickers;

	std::unordered_map<std::string, MarketDepth> depths;
	std::unordered_map<std::string, AnonimousMaketDepthVector> anonimousDepths;
	std::unordered_map<std::string, int> commissions;
	void readFile(std::string path, std::string ticker);
};

