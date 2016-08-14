#pragma once

enum class OrderType {
	marketOrder, limitOrder
};

struct DECLSPEC Candel {
	int open;
	int high;
	int low;
	int close;
	int volume = 0;
	boost::posix_time::ptime datetime;

	void setdatetime(std::string datetime);
	friend DECLSPEC bool operator==(const Candel& rhs,
									const Candel& lhs);
};

struct DECLSPEC Order {
	static Order Make_Limit_Order(std::string trader_identifier, std::string ticker, int volume, int price);
	
	friend DECLSPEC std::ostream& operator<<(std::ostream& outputStream, 
											 const Order& p);
	friend DECLSPEC bool operator==(const Order& rhs,
									const Order& lhs);
	Order& operator=(const Order&);

	int volume;
	int price;
	OrderType orderType;

	std::string trader_identifier;
	std::string ticker;

	const std::string getTicker();

	Order();
	Order(const Order &order);

	~Order();
} ;

enum class ChangeReason {
	Match, Cancellation, Unknown
};

std::ostream& operator<<(std::ostream & outputStream, const  ChangeReason & p);

struct DECLSPEC OrderChange {
	static OrderChange MakeChangesByComparison(const Order& initial,
											   const Order& after, 
											   ChangeReason reason,
											   int matchPrice = 0);
	static OrderChange ChangesOfOrderVanishing (const Order& order,
												ChangeReason reason,
												int matchPrice = 0);
	
	ChangeReason reason;
	int volumeChange;
	int worstPossiblePrice;
	int matchPrice = 0;
	int currentVolume;
	std::string ticker;
	std::string trader_identifier;

	friend DECLSPEC bool operator==(const OrderChange& rhs, const OrderChange& lhs);
	friend DECLSPEC std::ostream& operator<<(std::ostream& outputStream,
											 const OrderChange& p);
};

typedef std::unordered_map<int, std::list<Order>> MarketDepthData;

struct DECLSPEC AnonimousMarketDepthItem {
	AnonimousMarketDepthItem();
	AnonimousMarketDepthItem(int volume, int price);
	int price;
	int volume;
	friend DECLSPEC bool operator==(const AnonimousMarketDepthItem& rhs, const AnonimousMarketDepthItem& lhs);
	friend DECLSPEC std::ostream& operator<<(std::ostream& outputStream,
											 const AnonimousMarketDepthItem& p);
};

typedef std::vector<AnonimousMarketDepthItem> AnonimousMaketDepth;