#pragma once

class DECLSPEC SngleTraderMarket {
public:
	SngleTraderMarket();
	~SngleTraderMarket();

	void setTrader(Trader* trader);
	void loadHistoryData(std::string path);
	void runFullTest();
	
	void addOrder(Order order);
	void cancelOrder(std::string trades_identifier, std::string ticker, int desiredVolume, int price);
	
private:
	void tick();
	void send_data_to_traders(AnonimousMaketDepth& depth, std::vector<Match> matches);
	std::vector<Order> ordersToAdd;
	std::vector<Match> matchesToSend;
	std::vector<Candel> candelsToSend;
	std::vector<Candel> historyData;
	std::unordered_map<std::string, std::vector<int>> portfolio;
	Trader* trader;
};

