#pragma once

typedef std::unordered_map<std::string, std::vector<Candle>> CandesVectorMap;

class DECLSPEC SingleTraderMarket {
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

	const CandesVectorMap & getInternalHistoryCandles();

	const std::unordered_map<std::string, int>& getPortfio();
	const std::unordered_map<std::string, std::vector<float>> & getPerformance();
	void setComission(std::string ticker, int comission);

	int numberOfTics = -1;
	int currentTick = 0;
private:

	

	void tick();
	void updateDepths();
	void sendAndCleanTickData();
	void addAndCleanOrders();
	void sendAndCleanCandles();
	void addToChangesToSend(std::vector<OrderChange>& changes);
	void clearDepthFromHistoryOrders(std::string ticker);
	void updatePortfolio();
	void addToPerformance();
	void preparePerformance();
	std::vector<Order> ordersToAdd;
	std::vector<OrderChange> changesToSend;
	CandesVectorMap candlesToSend;
	CandesVectorMap historyData;
	std::unordered_map<std::string, int> portfolio;
	std::unordered_map<std::string, std::vector<float>> performance;
	Trader* trader;

	std::vector<std::string> tickers;

	std::unordered_map<std::string, MarketDepth> depths;
	std::unordered_map<std::string, AnonimousMaketDepth> anonimousDepths;
	std::unordered_map<std::string, int> commissions;
	void readFile(std::string path, std::string ticker);
};

