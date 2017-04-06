#include "../HeaderFiles/Backtester.h"


MarketDepth::MarketDepth(std::string ticker) {
	this->totalDepth = MarketDepthDataMap();
	this->ticker = ticker;
}


MarketDepth::~MarketDepth() {
}

std::vector<OrderChange> MarketDepth::addOrder(Order order) {
	currentChanges.clear();
	addChangeIfItIsNotHistory(OrderChange::ChangesOfOrderPlaced(order));

	tryMatchOrder(order);
	workAroundCheckedOrder(order);
	return currentChanges;
}

void MarketDepth::workAroundCheckedOrder(Order & order) {
	if (order.orderType != OrderType::IoC && order.volume != 0) {
		auto it_whereOrdersWithSuchPrice = this->totalDepth.find(order.price);
		if (it_whereOrdersWithSuchPrice == this->totalDepth.end()) {
			(this->totalDepth)[order.price] = { order };
		} else {
			if (order.traderIdentifier == OrdersFromCandleBuilder::HISTORY_IDENTIFIER) {
				it_whereOrdersWithSuchPrice->second.push_back(order); //Deprioritise history orders
			} else {
				it_whereOrdersWithSuchPrice->second.push_front(order);
			}
		}
	} else if (order.orderType == OrderType::IoC) {
		currentChanges.push_back(OrderChange::ChangesOfOrderVanished(order, ChangeReason::Cancelled));
	}
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

			addChangeIfItIsNotHistory(OrderChange::ChangesOfOrderVanished(*it, ChangeReason::Matched, it->price));

			Order oldOrder = order;
			order.volume += it->volume;

			addChangeIfItIsNotHistory(OrderChange::MakeChangesByComparison(oldOrder, order, ChangeReason::Matched, it->price));
			
			
			it = orderList.erase(it); // <-- here incrementation
		} else {  //Vanishing of newly recieved order

			addChangeIfItIsNotHistory(OrderChange::ChangesOfOrderVanished(order, ChangeReason::Matched, it->price));
			
			Order oldOrder = *it;
			it->volume += order.volume;
			order.volume = 0;

			addChangeIfItIsNotHistory(OrderChange::MakeChangesByComparison(oldOrder, *it, ChangeReason::Matched, it->price));
					
			break;
		}
	}
	if (orderList.size() == 0) {
		totalDepth.erase(price);
	}
}

void MarketDepth::addChangeIfItIsNotHistory(OrderChange change) {
	if (change.traderIdentifier != OrdersFromCandleBuilder::HISTORY_IDENTIFIER) {
		this->currentChanges.
			push_back(change);
	}
}

std::vector<OrderChange>  MarketDepth::changeOrder(Order order) {
	currentChanges.clear();
	removeOrdersOfSuchTraderWithSuchPrice(order);
	workAroundCheckedOrder(order);
	return currentChanges;
}

void MarketDepth::removeOrdersOfSuchTraderWithSuchPrice(Order newOrder) {
	auto it_whereOrdersWithSuchPrice = this->totalDepth.find(newOrder.price);
	if (it_whereOrdersWithSuchPrice != this->totalDepth.end()) {
		std::list<Order>& orderList = it_whereOrdersWithSuchPrice->second;
		for (auto it = orderList.begin(); it != orderList.end(); ) {
			if (it->traderIdentifier == newOrder.traderIdentifier) {
				addChangeIfItIsNotHistory(OrderChange::ChangesOfOrderVanished(*it, ChangeReason::Cancelled));
				it = orderList.erase(it);
			} else {
				it++;
			}
		}
	}
}

AnonimousMaketDepthVector  MarketDepth::getAnonimousDepth(int depthLength) {
	auto prices = getUnsortedPrices();
	std::sort(prices.begin(), prices.end());

	AnonimousMaketDepthVector depth;
	int firstSell;
	for (firstSell = 0; firstSell < prices.size()  &&  totalDepth[prices[firstSell]].begin()->volume > 0;
		 firstSell++);

	for (int i = Max(0, firstSell - depthLength); i < firstSell &&  i < prices.size(); i++) {
		AnonimousMarketDepthItem item;
		item.volume = 0;
		for (Order o : totalDepth[prices[i]]) {
			item.volume += o.volume;
		}
		item.price = prices[i];
		depth.push_back(item);
	}

	for (int i = firstSell; i < (firstSell+ depthLength) &&  i < prices.size(); i++) {
		AnonimousMarketDepthItem item;
		item.volume = 0;
		for (Order o : totalDepth[prices[i]]) {
			item.volume += o.volume;
		}
		item.price = prices[i];
		depth.push_back(item);
	}
	return depth;
}

MarketDepthDataMap const &  MarketDepth::getInternalDepth() {
	return this->totalDepth;
}

void MarketDepth::clearHistoryOrders() {
	auto prices = getUnsortedPrices();
	for (int price : prices) {
		std::list<Order>& orderList = totalDepth[price];
		for (auto it = orderList.begin(); it != orderList.end(); ) {
			if (it->traderIdentifier == OrdersFromCandleBuilder::HISTORY_IDENTIFIER) {
				it = orderList.erase(it);
			} else {
				it++;
			}
		}
		if (totalDepth[price].size() == 0) {
			totalDepth.erase(price);
		}
	}
}
