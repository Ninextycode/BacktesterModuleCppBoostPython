#include "stdafx.h"
#include "Backtester.h"


Order Order::Make_Limit_Order(std::string traderIdentifier, std::string ticker, int volume, int price) {
	Order o;
	o.orderType = OrderType::limitOrder;
	o.volume = volume;
	o.price = price;

	o.ticker = std::string(ticker);

	o.traderIdentifier = std::string(traderIdentifier);
	return o;
}

Order Order::Make_IoC_Order(std::string traderIdentifier, std::string ticker, int volume, int price) {
	Order o;
	o.orderType = OrderType::IoC;
	o.volume = volume;
	o.price = price;

	o.ticker = std::string(ticker);

	o.traderIdentifier = std::string(traderIdentifier);
	return o;
}


Order::Order() {
}

Order::Order(const Order & order) {
	this->orderType = order.orderType;
	this->price = order.price;
	this->ticker = std::string(order.ticker);
	this->traderIdentifier = std::string(order.traderIdentifier);
	this->volume = order.volume;
}

Order::~Order() {
}



bool operator==(const Order & lhs, const Order & rhs) {
	return
		lhs.orderType == rhs.orderType &&
		lhs.price == rhs.price &&
		lhs.volume == rhs.volume &&

		lhs.ticker == rhs.ticker &&
		lhs.traderIdentifier == rhs.traderIdentifier;
}

bool operator==(const OrderChange & lhs, const OrderChange & rhs) {
	return
		lhs.currentVolume == rhs.currentVolume &&
		lhs.worstPossiblePrice == rhs.worstPossiblePrice &&
		lhs.reason == rhs.reason &&

		lhs.ticker == rhs.ticker &&
		lhs.traderIdentifier == rhs.traderIdentifier &&
		lhs.volumeChange == rhs.volumeChange;
}


Order & Order::operator=(const Order &other) {
	Order order;
	order.orderType = other.orderType;
	order.volume = other.volume;
	order.price = other.price;
	order.ticker = std::string(other.ticker);
	order.traderIdentifier =
		std::string(other.traderIdentifier);

	return order;
}


const std::string Order::getTicker() {
	return ticker;
}

OrderChange OrderChange::MakeChangesByComparison (const Order & initial,
												 const Order & after,
												  ChangeReason reason,
												  int matchPrice) {
	OrderChange change;

	change.volumeChange = after.volume - initial.volume;
	change.currentVolume = after.volume;
	change.ticker = after.ticker;
	change.traderIdentifier = after.traderIdentifier;
	change.reason = reason;
	change.worstPossiblePrice = after.price;
	change.matchPrice = matchPrice;

	return change;
}

OrderChange OrderChange::ChangesOfOrderVanished (const Order & order,
												  ChangeReason reason,
												  int matchPrice) {
	OrderChange change;

	change.volumeChange = -order.volume;
	change.currentVolume = 0;
	change.ticker = order.ticker;
	change.traderIdentifier = order.traderIdentifier;
	change.reason = reason;
	change.worstPossiblePrice = order.price;
	change.matchPrice = matchPrice;

	return change;
}

OrderChange OrderChange::ChangesOfOrderPlaced(const Order & order) {
	OrderChange change;

	change.volumeChange = order.volume;
	change.currentVolume = order.volume;
	change.ticker = order.ticker;
	change.traderIdentifier = order.traderIdentifier;
	change.reason = ChangeReason::Placed;
	change.worstPossiblePrice = order.price;
	change.matchPrice = 0;

	return change;
}

AnonimousMarketDepthItem::AnonimousMarketDepthItem() {
}

AnonimousMarketDepthItem::AnonimousMarketDepthItem(int volume, int price) :
	volume(volume), price(price) {

}

bool operator==(const AnonimousMarketDepthItem & rhs, const AnonimousMarketDepthItem & lhs) {
	return rhs.price == lhs.price && rhs.volume == lhs.volume;
}

bool operator==(const Candle & rhs, const Candle & lhs) {
	return
		rhs.open == lhs.open &&
		rhs.high == lhs.high &&
		rhs.low == lhs.low &&
		rhs.close == lhs.close &&
		rhs.volume == lhs.volume &&
		rhs.datetime_index == lhs.datetime_index;
}



std::ostream &  operator<<(std::ostream & outputStream, const Candle & c) {
	outputStream
		<< "open = " << c.open
		<< " high = " << c.high
		<< " low = " << c.low
		<< " close = " << c.close
		<< " volume = " << c.volume
		<< " datetime_index = " << c.datetime_index;
	return outputStream;
}


std::ostream &  operator<<(std::ostream & outputStream, const Order & p) {
	outputStream << p.ticker << ", trader " << p.traderIdentifier << ", volume = " << p.volume << ", price = " << p.price;
	return outputStream;
}

std::ostream &  operator<<(std::ostream & outputStream, const OrderChange & p) {
	outputStream << p.ticker << "trader "<<  p.traderIdentifier
		<< ", current volume = " << p.currentVolume
		<< ", match price = " << p.matchPrice
		<< ", worst possible price = " << p.worstPossiblePrice
		<< ", volume change = " << p.volumeChange
		<< ", cause of change = " << p.reason;
	return outputStream;
}
std::ostream &  operator<<(std::ostream & outputStream, const AnonimousMarketDepthItem & p) {
	outputStream << "volume = " << p.volume << ", price = " << p.price;
	return outputStream;
}
std::ostream &  operator<<(std::ostream & outputStream, const ChangeReason & p) {
	std::string name;
	switch (p) {
		case ChangeReason::Matched:
			name = "Matched";
			break;
		case ChangeReason::Cancelled:
			name = "Cancelled";
			break;
		case ChangeReason::Placed:
			name = "Placed";
			break;
		case ChangeReason::Unknown:
			name = "Unknown";
			break;
	}
	outputStream << name;
	return outputStream;
}