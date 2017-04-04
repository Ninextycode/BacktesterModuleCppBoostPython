#pragma once

enum class OrderType {
	marketOrder, limitOrder, IoC
};

struct Candle {
	int open;
	int high;
	int low;
	int close;
	int volume = 0;
	long datetime_index;

	friend bool operator==(const Candle& rhs,
									const Candle& lhs);
	friend bool operator!=(const Candle& rhs,
						   const Candle& lhs);
	friend std::ostream& operator<<(std::ostream& outputStream,
											 const Candle& p);
};

struct Order {
	static Order Make_Limit_Order(std::string traderIdentifier, std::string ticker, int volume, int price);
	static Order Make_IoC_Order(std::string traderIdentifier, std::string ticker, int volume, int price);

	friend std::ostream& operator<<(std::ostream& outputStream, 
											 const Order& p);
	friend bool operator==(const Order& rhs,
									const Order& lhs);
	friend bool operator!=(const Order& rhs,
						   const Order& lhs);

	Order& operator=(const Order&);

	int volume;
	int price;
	OrderType orderType;

	std::string traderIdentifier;
	std::string ticker;

	const std::string getTicker();

	Order();
	Order(const Order &order);

	~Order();
} ;

enum class ChangeReason {
	Placed, Matched, Cancelled, Unknown
};

std::ostream& operator<<(std::ostream & outputStream, const ChangeReason & p);

struct OrderChange {
	static OrderChange MakeChangesByComparison(const Order& initial,
											   const Order& after, 
											   ChangeReason reason,
											   int matchPrice = 0);
	static OrderChange ChangesOfOrderVanished (const Order& order,
												ChangeReason reason,
												int matchPrice = 0);
	
	static OrderChange ChangesOfOrderPlaced(const Order& order);


	ChangeReason reason;
	int volumeChange;
	int worstPossiblePrice;
	int matchPrice = 0;
	int currentVolume;
	std::string ticker;
	std::string traderIdentifier;

	friend bool operator==(const OrderChange& rhs, const OrderChange& lhs);
	friend bool operator!=(const OrderChange& rhs, const OrderChange& lhs);
	friend std::ostream& operator<<(std::ostream& outputStream,
											 const OrderChange& p);
};

typedef std::unordered_map<int, std::list<Order>> MarketDepthDataMap;

struct AnonimousMarketDepthItem {
	AnonimousMarketDepthItem();
	AnonimousMarketDepthItem(int volume, int price);
	int price;
	int volume;
	friend bool operator==(const AnonimousMarketDepthItem& rhs, const AnonimousMarketDepthItem& lhs);
	friend bool operator!=(const AnonimousMarketDepthItem& rhs, const AnonimousMarketDepthItem& lhs);
	friend std::ostream& operator<<(std::ostream& outputStream,
											 const       AnonimousMarketDepthItem& p);
};

typedef std::vector<AnonimousMarketDepthItem> AnonimousMaketDepthVector;