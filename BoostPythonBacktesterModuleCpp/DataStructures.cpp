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

bool operator==(const Match & lhs, const Match & rhs) {
	return
		lhs.price == rhs.price &&
		lhs.volume == rhs.volume &&

		lhs.ticker == rhs.ticker &&
		lhs.trader_identifier == rhs.trader_identifier;
}

std::ostream &  operator<<(std::ostream & outputStream, const Order & p) {
	outputStream << p.ticker << ", volume = " << p.volume << ", price = " << p.price;
	return outputStream;
}

std::ostream & operator<<(std::ostream & outputStream, const Match & p) {
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

Match & Match::operator=(const Match &other) {
	Match match;
	match.volume = other.volume;
	match.price = other.price;
	match.ticker = std::string(other.ticker);
	match.trader_identifier =
		std::string(other.trader_identifier);

	return match;
}

const std::string Order::getTicker() {
	return ticker;
}

OrderChange OrderChange::MakeChangesByComparison (const Order & initial,
												 const Order & after, ChangeReason reason) {
	OrderChange change;

	change.volumeChange = after.volume - initial.volume;
	change.currentVolume = after.volume;
	change.ticker = after.ticker;
	change.trader_identifier = after.trader_identifier;
	change.reason = reason;

	return change;
}

OrderChange OrderChange::ChangesOfOrderVanishing (const Order & order, ChangeReason reason) {
	OrderChange change;

	change.volumeChange = -order.volume;
	change.currentVolume = 0;
	change.ticker = order.ticker;
	change.trader_identifier = order.trader_identifier;
	change.reason = reason;

	return change;
}
