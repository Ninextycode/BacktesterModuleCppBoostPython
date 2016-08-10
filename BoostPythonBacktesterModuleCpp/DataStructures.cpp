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
	//delete trader_identifier;
	//delete ticker;
}


Match Match::Make_Match(std::string  trader_identifier,
						std::string  ticker, int volume, int price) {
	Match match;
	match.volume = volume;
	match.price = price;

	match.ticker = std::string(ticker);

	match.trader_identifier = std::string(trader_identifier);

	return match;
}

Match::Match(const Match & match) {
	this->price = match.price;
	this->ticker = std::string(match.ticker);
	this->trader_identifier = std::string(match.trader_identifier);
	this->volume = match.volume;
}

Match::Match():ticker(), trader_identifier() {
}

Match::~Match() {
	//delete this->trader_identifier;
	//delete this->ticker;
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