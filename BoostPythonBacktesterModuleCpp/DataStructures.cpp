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
		lhs.worstPossiblePrice == rhs.worstPossiblePrice &&
		lhs.reason == rhs.reason &&

		lhs.ticker == rhs.ticker &&
		lhs.trader_identifier == rhs.trader_identifier &&
		lhs.volumeChange == rhs.volumeChange;
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
	change.worstPossiblePrice = after.price;
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
	change.worstPossiblePrice = order.price;
	change.matchPrice = matchPrice;

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

bool operator==(const Candel & rhs, const Candel & lhs) {
	return
		rhs.open == lhs.open &&
		rhs.high == lhs.high &&
		rhs.low == lhs.low &&
		rhs.close == lhs.close &&
		rhs.volume == lhs.volume &&
		rhs.datetime == lhs.datetime;
}

void Candel::setdatetime(std::string datetime) {
	this->datetime = boost::posix_time::time_from_string(datetime);
}

std::string Candel::getdatetime() {
	std::stringstream str;
	str << datetime;
	return str.str();
}

std::ostream &  operator<<(std::ostream & outputStream, const Candel & c) {
	outputStream
		<< "open = " << c.open
		<< " high = " << c.high
		<< " low = " << c.low
		<< " close = " << c.close
		<< " volume = " << c.volume
		<< " datetime = " << c.datetime;
	return outputStream;
}


std::ostream &  operator<<(std::ostream & outputStream, const Order & p) {
	outputStream << p.ticker << ", volume = " << p.volume << ", price = " << p.price;
	return outputStream;
}

std::ostream &  operator<<(std::ostream & outputStream, const OrderChange & p) {
	outputStream << p.ticker
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
		case ChangeReason::Match:
			name = "Match";
			break;
		case ChangeReason::Cancellation:
			name = "Cancellation";
			break;
		case ChangeReason::Unknown:
			name = "Unknown";
			break;
	}
	outputStream << name;
	return outputStream;
}