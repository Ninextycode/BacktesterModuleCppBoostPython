#pragma once

class SingleTraderMarket;

class DECLSPEC Trader {
public:
	Trader(std::string identifier);
	virtual ~Trader();

	
	virtual void newTickAction(std::vector<OrderChange> matcher, 
				std::unordered_map<std::string, AnonimousMaketDepth> depth) = 0;
	
	void recieveTickData(std::vector<OrderChange> matcher, 
						 std::unordered_map<std::string, AnonimousMaketDepth> depths);

	virtual void newCandelsAction(std::string ticker, std::vector<Candel> candels) = 0;

	void recieveCandels(std::string ticker, std::vector<Candel> candels);

	void setMarket(SingleTraderMarket* market);
	void changeOrder(Order order);
	void makeOrder(Order order);
	void requestCandels(std::string ticker, int length);

	const std::unordered_map<std::string, int>& getPortfio();
	

	Order createLimitOrder(std::string ticker, int volume, int price);
	Order createMarketOrder(std::string ticker, int volume);
	Order createIoCOrder(std::string ticker, int volume, int price);
	std::unordered_map<std::string, std::unordered_map<int, int>> orders;
protected:
	
	// ticker - price - volume map
	
	SingleTraderMarket* market;
	std::string identifier;
};

