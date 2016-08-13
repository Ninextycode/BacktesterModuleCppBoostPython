#include "stdafx.h"
#include "Backtester.h"


Order Order::Make_Limit_Order(std::string trader_identifier, std::string ticker, int volume, int price) {
	Order o;
	o.orderType = OrderType::limitOrder;
	o.volume = volume;
	o.price = price;

	o.ticker = std::string(ticker);

	o.trader_identifier = std::string(trader_identifier);
	return o;
}


Order::Order() {
}

Order::Order(const Order & order) {
	this->orderType = order.orderType;
	this->price = order.price;
	this->ticker = std::string(order.ticker);
	this->trader_identifier = std::string(order.trader_identifier);
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
		lhs.trader_identifier == rhs.trader_identifier;
}

bool operator==(const OrderChange & lhs, const OrderChange & rhs) {
	return
		lhs.currentVolume == rhs.currentVolume &&
		lhs.price == rhs.price &&
		lhs.reason == rhs.reason &&

		lhs.ticker == rhs.ticker &&
		lhs.trader_identifier == rhs.trader_identifier &&
		lhs.volumeChange == rhs.volumeChange;
}

std::ostream &  operator<<(std::ostream & outputStream, const Order & p) {
	outputStream << p.ticker << ", volume = " << p.volume << ", price = " << p.price;
	return outputStream;
}


Order & Order::operator=(const Order &other) {
	Order order;
	order.orderType = other.orderType;
	order.volume = other.volume;
	order.price = other.price;
	order.ticker = std::string(other.ticker);
	order.trader_identifier =
		std::string(other.trader_identifier);

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
	change.trader_identifier = after.trader_identifier;
	change.reason = reason;
	change.price = after.price;
	change.matchPrice = matchPrice;

	return change;
}

OrderChange OrderChange::ChangesOfOrderVanishing (const Order & order,
												  ChangeReason reason,
												  int matchPrice) {
	OrderChange change;

	change.volumeChange = -order.volume;
	change.currentVolume = 0;
	change.ticker = order.ticker;
	change.trader_identifier = order.trader_identifier;
	change.reason = reason;
	change.price = order.price;
	change.matchPrice = matchPrice;

	return change;
}

AnonimousmarketDepthItem::AnonimousmarketDepthItem() {
}

AnonimousmarketDepthItem::AnonimousmarketDepthItem(int volume, int price) :
	volume(volume), price(price) {

}

bool operator==(const AnonimousmarketDepthItem & rhs, const AnonimousmarketDepthItem & lhs) {
	return rhs.price == lhs.price && rhs.volume == lhs.volume;
}

bool operator==(const Candel & rhs, const Candel & lhs) {
	return
		rhs.open == lhs.open &&
		rhs.high == lhs.high &&
		rhs.low == lhs.low &&
		rhs.close == lhs.close &&
		rhs.volume == lhs.volume &&
		rhs.datetime == lhs.datetime;
}