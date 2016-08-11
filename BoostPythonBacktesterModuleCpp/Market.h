#pragma once

class DECLSPEC SngleTraderMarket {
public:
	SngleTraderMarket();
	~SngleTraderMarket();

	void setTrader(Trader* trader);
	void loadHistoryData(std::string path);
	void runFullTest();
	
	void addOrder(Order order);

	/* removes all previous orders of current trader */
	void changeOrder(Order order);
	void cancelOrder(std::string trades_identifier,
					 std::string ticker, int desiredVolume, int price);


	void updateDepths();
	
private:
	int currentTick = 0;
	void tick();
	void sendAndCleanTickData();
	void addAndCleanOrders();
	void sendAndCleanCandles();

	std::vector<Order> ordersToAdd;
	std::vector<Match> matchesToSend;
	std::vector<Candel> candelsToSend;
	std::unordered_map<std::string, std::vector<Candel>> historyData;
	std::unordered_map<std::string, std::vector<int>> portfolio;
	Trader* trader;

	std::vector<std::string> tickers;

	std::unordered_map<std::string, MarketDepth> depths;
	std::unordered_map<std::string, AnonimousMaketDepth> anonimousDepths;
};

