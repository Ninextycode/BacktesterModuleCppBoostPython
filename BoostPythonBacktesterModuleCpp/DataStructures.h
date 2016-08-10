#pragma once
typedef std::vector<std::tuple<int, int>> AnonimousMaketDepth;


enum class OrderType {
	marketOrder, limitOrder
};

struct DECLSPEC Candel {
	int open;
	int high;
	int low;
	int close;
	int volume;
	boost::posix_time::ptime datetime;
};

struct DECLSPEC Order {
	static Order Make_Limit_Order(std::string trader_identifier, std::string ticker, int volume, int price);
	
	friend DECLSPEC std::ostream& operator<<(std::ostream& outputStream, const Order& p);
	friend DECLSPEC bool operator==(const Order& rhs, const Order& lhs);
	Order& operator=(const Order&);

	int volume;
	int price;
	OrderType orderType;

	std::string trader_identifier;
	std::string ticker;

	bool isHistory = false;  //Should be set true for orders created from history data 

	const std::string getTicker();

	Order();
	Order(const Order &order);

	~Order();
} ;


struct DECLSPEC Match {

	static Match Make_Match(std::string trader_identifier, std::string ticker,
		int volume, int price);
	int volume;
	int price;

	friend DECLSPEC std::ostream& operator<<(std::ostream& outputStream, const Match& p);

	friend DECLSPEC bool operator==(const Match& rhs, const Match& lhs);

	std::string ticker;
	std::string trader_identifier;

	Match& operator=(const Match&);

	Match(const Match &match);
	Match();
	~Match();
};


typedef std::unordered_map<int, std::list<Order>> MarketDepthData;