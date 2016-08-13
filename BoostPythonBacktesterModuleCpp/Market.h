#pragma once

typedef std::unordered_map<std::string, std::vector<Candel>> CandesVectorMap;

class DECLSPEC SngleTraderMarket {
public:
	static const int DEPTH_LENGTH_PUBLIC = 5;
	
	SngleTraderMarket();
	~SngleTraderMarket();

	void setTrader(Trader* trader);
	void loadHistoryData(std::string path, std::vector<std::string> tickers);
	void runFullTest();
	
	void addOrder(Order order);


	void changeOrder(Order order);

	void requestCandles(std::string ticker, int length);

	void updateDepths();
	const CandesVectorMap & getInternalHistoryCandles();
private:
	int currentTick = 0;
	int numberOfTics = -1;

	void tick();
	void sendAndCleanTickData();
	void addAndCleanOrders();
	void sendAndCleanCandles();
	void addToChangesToSend(std::vector<OrderChange>& changes);
	void clearDepthFromHistoryOrders(std::string ticker);

	std::vector<Order> ordersToAdd;
	std::vector<OrderChange> changesToSend;
	CandesVectorMap candelsToSend;
	CandesVectorMap historyData;
	std::unordered_map<std::string, std::vector<int>> portfolio;
	Trader* trader;

	std::vector<std::string> tickers;

	std::unordered_map<std::string, MarketDepth> depths;
	std::unordered_map<std::string, AnonimousMaketDepth> anonimousDepths;

	void readFile(std::string path, std::string ticker);
};

