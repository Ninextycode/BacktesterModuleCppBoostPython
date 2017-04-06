#include "../HeaderFiles/Backtester.h"


SingleTraderMarket::SingleTraderMarket() {
	this->portfolio["money"] = 0;
}


SingleTraderMarket::~SingleTraderMarket() {
}

void SingleTraderMarket::setTrader(Trader * trader) {
	this->trader = trader;
}

void SingleTraderMarket::loadHistoryData(std::string path, std::vector<std::string> tickers) {
	if (path.length() != 0 && path[path.length() - 1] != '/') {
		path += "/";
	}
	for (auto ticker : tickers) {
		this->tickers.push_back(ticker);
		readFile(path + ticker + ".csv", ticker);
	}
}

void SingleTraderMarket::readFile(std::string path, std::string ticker) {
	std::ifstream input(path);
	if (!input.is_open()) {
		throw std::invalid_argument("Wrong path");
	}
	std::string s;
	std::getline(input, s);
	int i = 0;
	while (!input.eof()) {		
		std::getline(input, s);

		std::string datetime = s.substr(0, s.find_first_of(","));
		s = s.substr(s.find_first_of(",") + 1);
		std::string open = s.substr(0, s.find_first_of(","));
		s = s.substr(s.find_first_of(",") + 1);
		std::string high = s.substr(0, s.find_first_of(","));
		s = s.substr(s.find_first_of(",") + 1);
		std::string low = s.substr(0, s.find_first_of(","));
		s = s.substr(s.find_first_of(",") + 1);
		std::string close = s.substr(0, s.find_first_of(","));
		s = s.substr(s.find_first_of(",") + 1);
		std::string volume = s.substr(0, s.find_first_of(","));
		s = s.substr(s.find_first_of(",") + 1);

		if (datetime.size() * open.size() * high.size() * low.size() * close.size() == 0) {
			continue;
		}

		Candle c;
		c.open = stoi(open);
		c.high = stoi(high);
		c.low = stoi(low);
		c.close = stoi(close);
		c.volume = volume.size() == 0 ? 0 : stoi(volume);
		c.datetime_index = i; //TODO FIX THIS datetime_index should br better
		historyData[ticker].push_back(c);
	}
	if (numberOfTics == -1) {
		numberOfTics = historyData[ticker].size();
	} else {
		numberOfTics = Min(historyData[ticker].size(), numberOfTics);
	}
	input.close();
}

void SingleTraderMarket::preparePerformance() {
	for (auto ticker : tickers) {
		performance[ticker].reserve(numberOfTics);
	}
	performance["money"].reserve(numberOfTics);
	performance["lv"].reserve(numberOfTics);
}

void SingleTraderMarket::runFullTest() {
	preparePerformance();
	for (currentTick = 0; currentTick < numberOfTics; currentTick++) {
		tick();
	}
}

void SingleTraderMarket::addOrder(Order order) {
	ordersToAdd.push_back(order);
}

void SingleTraderMarket::changeOrder(Order order) {
	ordersToChange.push_back(order);
}

void SingleTraderMarket::tick() {
	this->updateDepths();
	this->addChangeAndCleanOrders();
	this->updatePortfolio();
	this->addToPerformance();
	this->sendAndCleanTickData();
	this->sendAndCleanCandles();
}

void SingleTraderMarket::updateDepths() {
	for (auto ticker : tickers) {
		clearDepthFromHistoryOrders(ticker);
		auto currentCandle = historyData[ticker][currentTick];
		auto orders = OrdersFromCandleBuilder::ordersFromCandle(currentCandle, ticker);
		for (Order order : orders) {
			auto changes = depths[ticker].addOrder(order);
			addToChangesToSend(changes);
		}
		anonimousDepths[ticker] = depths[ticker].getAnonimousDepth(DEPTH_LENGTH_PUBLIC);
	}
}

void  SingleTraderMarket::clearDepthFromHistoryOrders(std::string ticker) {
	depths[ticker].clearHistoryOrders();
}

void SingleTraderMarket::updatePortfolio() {
	for (OrderChange change : changesToSend) {
		if (change.reason == ChangeReason::Matched) {
			portfolio[change.ticker] += (-change.volumeChange);
			portfolio["money"] += (change.volumeChange * change.matchPrice
								   - abs(change.volumeChange) * this->commissions[change.ticker]);
		}
	}
}

void SingleTraderMarket::addToPerformance() {
	for (auto ticker : tickers) {
		performance[ticker].push_back(portfolio[ticker]);
	}
	performance["money"].push_back(portfolio["money"]);
	float lv = 0;
	for (auto ticker : tickers) {
		lv = lv
			+ portfolio["money"]
			+ portfolio[ticker] * historyData[ticker][currentTick].close
			- abs(portfolio[ticker]) //for slippage
			- portfolio[ticker]*commissions[ticker];		
	}
	performance["lv"].push_back(lv);
}

void SingleTraderMarket::addChangeAndCleanOrders() {
	addAndCleanOrders();
	changeAndCleanOrders();	
}

void  SingleTraderMarket::addAndCleanOrders() {
	for (Order order : this->ordersToAdd) {
		auto changes = depths[order.ticker].addOrder(order);
		addToChangesToSend(changes);
	}
	ordersToAdd.clear();
}

void  SingleTraderMarket::changeAndCleanOrders() {
	for (Order order : this->ordersToChange) {
			auto changes = depths[order.ticker].changeOrder(order);
			addToChangesToSend(changes);
		}
	ordersToChange.clear();
}

void SingleTraderMarket::sendAndCleanTickData() {
	this->trader->recieveTickData(changesToSend, anonimousDepths);
	changesToSend.clear();
}

void SingleTraderMarket::sendAndCleanCandles() {
	for (auto ticker : tickers) {
		if (candlesToSend[ticker].size() > 0) {
			this->trader->recieveCandles(ticker, candlesToSend[ticker]);
		}
		candlesToSend[ticker].clear();
	}
}


void SingleTraderMarket::requestCandles(std::string ticker, int length) {
	// additional +1 for not including begin and including end
	int begin = Max(0, currentTick + 1 - length + 1);
	int end = Min(numberOfTics, currentTick + 1 + 1);
	candlesToSend[ticker] = std::vector<Candle>(historyData[ticker].begin()+ begin,
											 historyData[ticker].begin() + end);
}

void SingleTraderMarket::addToChangesToSend(std::vector<OrderChange>& changes) {
	changesToSend.insert(changesToSend.end(), changes.begin(), changes.end());
}

const CandlesVectorMap & SingleTraderMarket::getInternalHistoryCandles() {
	return this->historyData;
}

const std::unordered_map<std::string, int>& SingleTraderMarket::getPortfio() {
	return this->portfolio;
}

const std::unordered_map<std::string, std::vector<float>>& SingleTraderMarket::getPerformance() {
	return this->performance;
}

void SingleTraderMarket::setComission(std::string ticker, int comission) {
	this->commissions[ticker] = comission;
}
