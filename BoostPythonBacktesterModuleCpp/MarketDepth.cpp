#include "stdafx.h"
#include "Backtester.h"


MarketDepth::MarketDepth(std::string ticker) {
	this->totalDepth = MarketDepthData();
	this->ticker = ticker;
}


MarketDepth::~MarketDepth() {
}

std::vector<OrderChange> MarketDepth::addOrder(Order order) {
	currentChanges.clear();
	tryMatchOrder(order);
	if (order.volume != 0) {
		auto it_whereOrdersWithSuchPrice = this->totalDepth.find(order.price);
		if (it_whereOrdersWithSuchPrice == this->totalDepth.end()) {
			(this->totalDepth)[order.price] = { order };
		} else {
			if (order.trader_identifier == OrdersFromCandelBuilder::HISTORY_IDENTIFIER) {
				it_whereOrdersWithSuchPrice->second.push_back(order); //Deprioritise history orders
			} else {
				it_whereOrdersWithSuchPrice->second.push_front(order);
			}
		}
	}
	return currentChanges;
}


void MarketDepth::tryMatchOrder(Order& order) {
	if (order.volume > 0) {
		tryMatchBuyOrder(order);
	} else {
		tryMatchSellOrder(order);
	}
}

void MarketDepth::tryMatchBuyOrder(Order& order) {
	auto prices = this->getUnsortedPrices();
	std::sort(prices.begin(), prices.end());

	for (int price : prices) {
		if (order.price >= price && (totalDepth)[price].begin()->volume < 0) {
			tryMatchOrderWithExactPriceOrders(order, price);
			if (order.volume == 0) 
				break;
		}
	}
}

void MarketDepth::tryMatchSellOrder(Order& order) {
	auto prices = this->getUnsortedPrices();
	std::sort(prices.begin(), prices.end(), [&](auto a, auto b) {return a > b; });

	for (int price : prices) {
		if (order.price <= price && (totalDepth)[price].begin()->volume > 0) {
			tryMatchOrderWithExactPriceOrders(order, price);
			if (order.volume == 0) break;
		}
	}
}

std::vector<int> MarketDepth::getUnsortedPrices() {
	std::vector<int> prices;
	prices.reserve(this->totalDepth.size());
	for (auto pair : this->totalDepth) {
		prices.push_back(pair.first);
	}
	return prices;
}

void MarketDepth::tryMatchOrderWithExactPriceOrders(Order& order, int price) {
	auto &orderList = (totalDepth)[price];
	for (auto it = orderList.begin(); it != orderList.end(); /* "it" is incrementing */) {
		if (abs(order.volume) >= abs(it->volume)) { //Vanishing of order already being inside depth

			this->currentChanges.
					push_back(OrderChange::ChangesOfOrderVanishing(*it, ChangeReason::Match, it->price));

			Order oldOrder = order;
			order.volume += it->volume;

			this->currentChanges.
				push_back(OrderChange::MakeChangesByComparison(oldOrder, order, ChangeReason::Match, it->price));
			
			
			it = orderList.erase(it); // <-- here incrementation
		} else {  //Vanishing of newly recieved order

			this->currentChanges.
				push_back(OrderChange::ChangesOfOrderVanishing(order, ChangeReason::Match, it->price));
			
			Order oldOrder = *it;
			it->volume += order.volume;
			order.volume = 0;

			this->currentChanges.
				push_back(OrderChange::MakeChangesByComparison(oldOrder, *it, ChangeReason::Match, it->price));
					
			break;
		}
	}
	if (orderList.size() == 0) {
		totalDepth.erase(price);
	}
}


std::vector<OrderChange>  MarketDepth::changeOrder(Order order) {
	currentChanges.clear();
	removeOrdersOfSuchTraderWithSuchPrice(order);
	if (order.volume != 0) {
		auto it_whereOrdersWithSuchPrice = this->totalDepth.find(order.price);
		if (it_whereOrdersWithSuchPrice == this->totalDepth.end()) {
			(this->totalDepth)[order.price] = { order };
		} else {
			it_whereOrdersWithSuchPrice->second.push_front(order);
		}
	}
	return currentChanges;
}

void MarketDepth::removeOrdersOfSuchTraderWithSuchPrice(Order newOrder) {
	auto it_whereOrdersWithSuchPrice = this->totalDepth.find(newOrder.price);
	if (it_whereOrdersWithSuchPrice != this->totalDepth.end()) {
		std::list<Order>& orderList = it_whereOrdersWithSuchPrice->second;
		for (auto it = orderList.begin(); it != orderList.end(); ) {
			if (it->trader_identifier == newOrder.trader_identifier) {

				this->currentChanges.
					push_back(OrderChange::ChangesOfOrderVanishing(*it, ChangeReason::Cancellation));

				it = orderList.erase(it);

			} else {
				it++;
			}
		}
	}
}

AnonimousMaketDepth  MarketDepth::getAnonimousDepth(int depthLength) {
	return std::vector<std::tuple<int, int>>{ 
		std::tuple<int, int>(1,2),
		std::tuple<int, int>(3,4)
	};
}

MarketDepthData const &  MarketDepth::getInternalDepth() {
	return this->totalDepth;
}

void MarketDepth::clearHistoryOrders() {
	for (auto pair : totalDepth) {
		auto orderList = pair.second;
		for (auto it = orderList.begin(); it != orderList.end(); ) {
			if (it->trader_identifier == OrdersFromCandelBuilder::HISTORY_IDENTIFIER) {
				it = orderList.erase(it);
			} else {
				it++;
			}
		}
	}
}
