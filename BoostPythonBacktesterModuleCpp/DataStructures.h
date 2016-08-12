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

enum class ChangeReason {
	Match, Cancellation, Unknown
};

struct DECLSPEC OrderChange {
	static OrderChange MakeChangesByComparison(const Order& initial,
											   const Order& after, ChangeReason reason);
	static OrderChange ChangesOfOrderVanishing (const Order& order, ChangeReason reason);
	
	ChangeReason reason;
	int volumeChange;
	int price;
	int currentVolume;
	std::string ticker;
	std::string trader_identifier;

	friend DECLSPEC bool operator==(const OrderChange& rhs, const OrderChange& lhs);
};

typedef std::unordered_map<int, std::list<Order>> MarketDepthData;