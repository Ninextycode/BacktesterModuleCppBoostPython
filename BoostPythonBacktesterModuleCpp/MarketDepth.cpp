#include "stdafx.h"
#include "Backtester.h"


MarketDepth::MarketDepth(std::string ticker) {
	this->totalDepth = new MarketDepthData();
	this->ticker = new std::string(ticker);
}


MarketDepth::~MarketDepth() {
	delete this->ticker;
	delete this->totalDepth;
}

void MarketDepth::addOrder(Order order) {
	this->currentMatches = new std::vector<Match>();
	tryMatchOrder(&order);
	if (order.volume != 0) {
		auto it = this->totalDepth->find(order.price);
		if (it == this->totalDepth->end()) {
			(*this->totalDepth)[order.price] = { order };
		} else {
			it->second.push_front(order);
		}
	}
	delete this->currentMatches;
}


void MarketDepth::tryMatchOrder(Order* order) {
	if (order->volume > 0) {
		tryMatchBuyOrder(order);
	} else {
		tryMatchSellOrder(order);
	}
}

void MarketDepth::tryMatchBuyOrder(Order* order) {
	auto prices = this->getUnsortedPrices();
	std::sort(prices.begin(), prices.end());

	for (int price : prices) {
		if (order->price >= price && (*totalDepth)[price].begin()->volume < 0) {
			tryMatchOrderWithExactPriceOrders(order, price);
			if (order->volume == 0) 
				break;
		}
	}
}

void MarketDepth::tryMatchSellOrder(Order* order) {
	auto prices = this->getUnsortedPrices();
	std::sort(prices.begin(), prices.end(), [&](auto a, auto b) {return a > b; });

	for (int price : prices) {
		if (order->price <= price && (*totalDepth)[price].begin()->volume > 0) {
			tryMatchOrderWithExactPriceOrders(order, price);
			if (order->volume == 0) break;
		}
	}
}

std::vector<int> MarketDepth::getUnsortedPrices() {
	std::vector<int> prices;
	prices.reserve(this->totalDepth->size());
	for (auto pair : *this->totalDepth) {
		prices.push_back(pair.first);
	}
	return prices;
}

void MarketDepth::tryMatchOrderWithExactPriceOrders(Order* order, int price) {
	auto &ordersList = (*totalDepth)[price];
	for (auto it = ordersList.begin(); it != ordersList.end(); /* "it" is incrementing */) {
		if (abs(order->volume) >= abs(it->volume)) {
			this->currentMatches->
					push_back(Match::Make_Match(*it->trader_identifier, 
							  *it->ticker, it->volume, price));

			this->currentMatches->
				push_back(Match::Make_Match(*order->trader_identifier,
						  *order->ticker, -it->volume, price));
			
			order->volume += it->volume;
			it = ordersList.erase(it); // <-- here incrementation
		} else {
			this->currentMatches->
				push_back(Match::Make_Match(*it->trader_identifier,
						  *it->ticker, -order->volume, price));

			this->currentMatches->
				push_back(Match::Make_Match(*order->trader_identifier,
						  *order->ticker, order->volume, price));

			it->volume += order->volume;
			order->volume = 0;			
			break;
		}
	}
	if (ordersList.size() == 0) {
		totalDepth->erase(price);
	}
}

void MarketDepth::removeOrder(int id) {
	//TODO
}

AnonimousMaketDepth const *  MarketDepth::getAnonimousDepth(int depthLength) {
	return new std::vector<std::tuple<int, int>>{ 
		std::tuple<int, int>(1,2),
		std::tuple<int, int>(3,4)
	};
}

MarketDepthData const *  MarketDepth::getInternalDepth() {
	return this->totalDepth;
}
