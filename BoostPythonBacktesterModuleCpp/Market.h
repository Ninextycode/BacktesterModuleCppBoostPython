#pragma once

typedef std::unordered_map<std::string, std::vector<Candel>> CandesVectorMap;

class DECLSPEC SingleTraderMarket {
public:
	int DEPTH_LENGTH_PUBLIC = 5;
	
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
	std::vector<Order> ordersToAdd;
	std::vector<OrderChange> changesToSend;
	CandesVectorMap candelsToSend;
	CandesVectorMap historyData;
	std::unordered_map<std::string, int> portfolio;
	Trader* trader;

	std::vector<std::string> tickers;

	std::unordered_map<std::string, MarketDepth> depths;
	std::unordered_map<std::string, AnonimousMaketDepth> anonimousDepths;
	std::unordered_map<std::string, int> commissions;
	void readFile(std::string path, std::string ticker);
};

