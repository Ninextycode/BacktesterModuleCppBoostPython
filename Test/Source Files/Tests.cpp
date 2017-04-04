#include "../../BoostPythonBacktesterModuleCpp/Header Files/Backtester.h"
#include<stdio.h>     //for remove( )

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
using namespace std;

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE Tests
#include <boost/test/unit_test.hpp>

template<class T>
std::ostream& operator<<(std::ostream& outputStream,
						 const std::list<T>& l) {
	outputStream << "[";
	for (auto it = l.begin(); it != l.end(); ) {
		outputStream << *it;
		it++;
		if (it != l.end()) {
			outputStream << ", ";
		}
	}
	outputStream << " ]";
	return outputStream;
}
template<class T>
std::ostream& operator<<(std::ostream& outputStream,
						 const std::vector<T>& v) {
	outputStream << "[";
	for (auto it = v.begin(); it != v.end(); ) {
		outputStream << *it;
		it++;
		if (it != v.end()) {
			outputStream << ", ";
		}
	}
	outputStream << " ]";
	return outputStream;
}
template<class K, class V>
std::ostream& operator<<(std::ostream& outputStream,
						 const std::unordered_map<K, V>& depth) {
	outputStream << "[";
	for (auto it = depth.begin(); it != depth.end(); ) {
		outputStream << "(" << it->first << ":" << it->second << ")";
		it++;
		if (it != depth.end()) {
			outputStream << ", ";
		}
	}
	outputStream << " ]";
	return outputStream;
}

namespace boost {
	namespace test_tools {
		namespace tt_detail {
			template<class T>
			struct print_log_value<std::list<T>> {
				void operator()(std::ostream& os,
								const std::list<T>& ts) {
					::operator<<(os, ts);
				}
			};
			template<class T>
			struct print_log_value<std::vector<T>> {
				void operator()(std::ostream& os,
								const std::vector<T>& ts) {
					::operator<<(os, ts);
				}
			};
			template<class K, class V>
			struct print_log_value<std::unordered_map<K, V>> {
				void operator()(std::ostream& os,
								const std::unordered_map<K, V>& ts) {
					::operator<<(os, ts);
				}
			};
		}
	}
}

BOOST_AUTO_TEST_SUITE(Tests)

BOOST_AUTO_TEST_CASE(AddingOrders) {

	MarketDepth m("s1");
	Order o_buy = Order::Make_Limit_Order("Max", "share", 1, 10);
	Order o_sell = Order::Make_Limit_Order("Max", "share", -1, 11);
	m.addOrder(o_buy);
	m.addOrder(o_sell);
	m.addOrder(o_sell);
	MarketDepthDataMap expectedDepth{
		{ 10,{ o_buy } },
		{ 11 ,{ o_sell, o_sell } }
	};

	BOOST_CHECK_EQUAL(expectedDepth, m.getInternalDepth());
}

BOOST_AUTO_TEST_CASE(AddingOrders_TestOrdering) {

	MarketDepth m("s1");
	Order o_buy = Order::Make_Limit_Order("Max", "share", 1, 10);
	Order o_sell = Order::Make_Limit_Order("Max", "share", -1, 11);
	m.addOrder(o_buy);
	m.addOrder(o_sell);
	m.addOrder(o_sell);

	Order o_sell2 = Order::Make_Limit_Order("Max", "share", -15, 11);
	m.addOrder(o_sell2);

	MarketDepthDataMap expectedDepth = {
		{ 10 ,{ o_buy } },
		{ 11 ,{ o_sell2,  o_sell, o_sell } }
	};

	BOOST_CHECK_EQUAL(expectedDepth, m.getInternalDepth());
}

BOOST_AUTO_TEST_CASE(MatchingOrders_Partial) {
	MarketDepth m("s1");
	Order o_buy = Order::Make_Limit_Order("Max", "share", 10, 10);
	Order o_sell = Order::Make_Limit_Order("Max", "share", -10, 11);
	m.addOrder(o_buy);
	m.addOrder(o_sell);

	Order o_match_sell = Order::Make_Limit_Order("Max", "share", 5, 11);
	Order o_match_buy = Order::Make_Limit_Order("Max", "share", -5, 10);

	m.addOrder(o_match_sell);
	m.addOrder(o_match_buy);

	MarketDepthDataMap expectedDepth{
		{ 10 ,{ Order::Make_Limit_Order("Max", "share", 5, 10) } },
		{ 11 ,{ Order::Make_Limit_Order("Max", "share", -5, 11) } }
	};
	BOOST_CHECK_EQUAL(expectedDepth, m.getInternalDepth());
}

BOOST_AUTO_TEST_CASE(MatchingOrders_Full) {

	MarketDepth m("s1");
	Order o_buy = Order::Make_Limit_Order("Max", "share", 10, 10);
	Order o_sell = Order::Make_Limit_Order("Max", "share", -10, 11);
	m.addOrder(o_buy);
	m.addOrder(o_sell);

	o_buy = Order::Make_Limit_Order("Max", "share", 10, 9);
	o_sell = Order::Make_Limit_Order("Max", "share", -10, 12);

	m.addOrder(o_buy);
	m.addOrder(o_sell);

	Order o_match_sell = Order::Make_Limit_Order("Max", "share", 20, 15);
	Order o_match_buy = Order::Make_Limit_Order("Max", "share", -20, 5);

	m.addOrder(o_match_sell);
	m.addOrder(o_match_buy);

	MarketDepthDataMap expectedDepth{

	};
	BOOST_CHECK_EQUAL(expectedDepth, m.getInternalDepth());
}

BOOST_AUTO_TEST_CASE(OrdersFromCandleBuilderTest) {
	Candle c;
	c.open = 1;
	c.high = 2;
	c.low = 3;
	c.close = 1000;
	c.volume = 5;
	c.datetime_index = 0;

	vector<Order> actual = OrdersFromCandleBuilder::ordersFromCandle(c, "share", 10, 2);

	vector<Order> expected{
		Order::Make_Limit_Order("history", "share", 10, 800),
		Order::Make_Limit_Order("history", "share", 10, 900),

		Order::Make_Limit_Order("history", "share", -10, 1100),
		Order::Make_Limit_Order("history", "share", -10, 1200)
	};

	BOOST_CHECK_EQUAL(expected, actual);
}

BOOST_AUTO_TEST_CASE(MatchingOrders_OverBuyOverSell) {
	MarketDepth m("s1");
	Order o_sell = Order::Make_Limit_Order("Max", "share", -10, 11);
	m.addOrder(o_sell);

	Order o_match_sell = Order::Make_Limit_Order("Max", "share", 20, 12);

	m.addOrder(o_match_sell);

	MarketDepthDataMap expectedDepth{
		{ 12 ,{ Order::Make_Limit_Order("Max", "share", 10, 12) } }
	};
	BOOST_CHECK_EQUAL(expectedDepth, m.getInternalDepth());
}

BOOST_AUTO_TEST_CASE(MatchingOrders_OrderChangerReport) {
	MarketDepth m("s1");
	Order first_order = Order::Make_Limit_Order("Fin", "share", 10, 10);
	auto changes = m.addOrder(first_order);

	Order second_order = Order::Make_Limit_Order("Stan", "share", -10, 5);
	auto changes2 = m.addOrder(second_order);
	changes.insert(changes.end(), changes2.begin(), changes2.end());

	vector<OrderChange> expectedChanges;

	OrderChange change;
	change.volumeChange = 10;
	change.currentVolume = 10;
	change.ticker = "share";
	change.traderIdentifier = "Fin";
	change.reason = ChangeReason::Placed;
	change.worstPossiblePrice = 10;
	change.matchPrice = 0;

	expectedChanges.push_back(change);

	change.volumeChange = -10;
	change.currentVolume = -10;
	change.ticker = "share";
	change.traderIdentifier = "Stan";
	change.reason = ChangeReason::Placed;
	change.worstPossiblePrice = 5;
	change.matchPrice = 0;
	expectedChanges.push_back(change);


	change.currentVolume = 0;
	change.worstPossiblePrice = 10;
	change.reason = ChangeReason::Matched;
	change.ticker = "share";
	change.traderIdentifier = "Fin";
	change.volumeChange = -10;
	change.matchPrice = 10;
	expectedChanges.push_back(change);



	change.currentVolume = 0;
	change.worstPossiblePrice = 5;
	change.reason = ChangeReason::Matched;
	change.ticker = "share";
	change.traderIdentifier = "Stan";
	change.volumeChange = 10;
	change.matchPrice = 10;
	expectedChanges.push_back(change);


	BOOST_CHECK_EQUAL(expectedChanges, changes);
}

BOOST_AUTO_TEST_CASE(MatchingOrders_OrderCancelReport) {
	MarketDepth m("s1");
	Order o_sell = Order::Make_Limit_Order("Max", "share", -10, 11);
	m.addOrder(o_sell);

	auto changes = m.changeOrder(Order::Make_Limit_Order("Max", "share", 0, 11));

	OrderChange change;

	change.currentVolume = 0;
	change.worstPossiblePrice = 11;
	change.reason = ChangeReason::Cancelled;
	change.ticker = "share";
	change.traderIdentifier = "Max";
	change.volumeChange = 10;

	vector<OrderChange> expected_changes = { change };

	BOOST_CHECK_EQUAL(expected_changes, changes);
}

BOOST_AUTO_TEST_CASE(AddingOrders_DeprioritizeHistoryOrders) {
	MarketDepth m("s1");

	Order notHistoryOrder = Order::Make_Limit_Order("Max", "share", 1, 1);
	//check that OrdersFromCandleBuilder::HISTORY_IDENTIFIER == history
	Order historyOrder =
		Order::Make_Limit_Order("history", "share", 1, 1);

	m.addOrder(notHistoryOrder);
	m.addOrder(historyOrder);

	MarketDepthDataMap expectedDepth{
		{ 1 ,{ notHistoryOrder,  historyOrder } }
	};
	BOOST_CHECK_EQUAL(expectedDepth, m.getInternalDepth());
}

BOOST_AUTO_TEST_CASE(AnonimousDepthConstruction) {
	MarketDepth m("s1");

	vector<Order> orders;
	orders.push_back(Order::Make_Limit_Order("Max", "share", 1, 1));
	orders.push_back(Order::Make_Limit_Order("Max", "share", 1, 2));
	orders.push_back(Order::Make_Limit_Order("Max", "share", 1, 3));
	orders.push_back(Order::Make_Limit_Order("Max", "share", 1, 4));

	orders.push_back(Order::Make_Limit_Order("Max", "share", 1, 5));
	orders.push_back(Order::Make_Limit_Order("Max", "share", 1, 5));


	orders.push_back(Order::Make_Limit_Order("Max", "share", -1, 6));
	orders.push_back(Order::Make_Limit_Order("Max", "share", -1, 6));

	orders.push_back(Order::Make_Limit_Order("Max", "share", -1, 7));


	AnonimousMaketDepthVector expectedDepth = {
		{ 1, 3 },{ 1, 4 },{ 2, 5 },
		{ -2, 6 },{ -1, 7 }
	};

	for (Order o : orders) {
		m.addOrder(o);
	}

	BOOST_CHECK_EQUAL(expectedDepth, m.getAnonimousDepth(3));
}

BOOST_AUTO_TEST_CASE(ReadingCsvFile) {
	string tofile = "datetime,close,high,low,open,volume\n";
	tofile += "1998-08-04 10:00:00,1,2,3,4,5\n";
	tofile += "1998-08-05 10:00:00,1,2,3,4,5\n";
	tofile += "1998-08-06 10:00:00,1,2,3,4,5\n";
	ofstream out("share.csv");
	if (out.is_open()) {
		out << tofile;
		out.close();
	}
	SingleTraderMarket m;
	m.loadHistoryData("", { "share" });
	auto actual = m.getInternalHistoryCandles();

	Candle c;
	c.open = 1;
	c.high = 2;
	c.low = 3;
	c.close = 4;
	c.volume = 5;

	CandlesVectorMap expected = {};
	c.datetime_index = 0;
	expected["share"].push_back(c);
	c.datetime_index = 0;
	expected["share"].push_back(c);
	c.datetime_index = 0;
	expected["share"].push_back(c);
	remove("share.csv");
	BOOST_CHECK_EQUAL(expected, actual);
}

BOOST_AUTO_TEST_CASE(RecievingCandlesByRequestTest) {
	string tofile = "datetime,close,high,low,open,volume\n";
	tofile += "1998-08-04 10:00:00,1,2,3,40,50\n"; //0
	tofile += "1998-08-05 10:00:00,1,2,3,40,50\n";
	tofile += "1998-08-06 10:00:00,1,2,3,40,50\n";
	tofile += "1998-08-07 10:00:00,1,2,3,40,50\n";
	tofile += "1998-08-08 10:00:00,1,2,3,40,50\n";
	tofile += "1998-08-09 10:00:00,1,2,3,40,50\n"; //5
	tofile += "1998-08-10 10:00:00,1,2,3,40,50\n"; //6
	tofile += "1998-08-11 10:00:00,1,2,3,40,50\n";
	tofile += "1998-08-12 10:00:00,1,2,3,40,50\n";
	tofile += "1998-08-13 10:00:00,1,2,3,40,50\n"; //9
	ofstream out("share.csv");
	if (out.is_open()) {
		out << tofile;
		out.close();
	}

	class TraderSaver : public  Trader {
	public:
		TraderSaver(string identifier) :Trader(identifier) {};
		~TraderSaver() {};
		int tick = 0;
		void newTickAction(std::vector<OrderChange> matcher,
						   std::unordered_map<std::string, AnonimousMaketDepthVector> depth) {
			if (tick == 5) {
				this->market->requestCandles("share", 4);
			}
			tick++;
		}
		void newCandlesAction(std::string ticker, std::vector<Candle> candles) override {
			recieved[ticker] = candles;
		}
		CandlesVectorMap recieved;
	};

	SingleTraderMarket* market = new SingleTraderMarket();
	market->loadHistoryData("", { "share" });
	TraderSaver* trader = new TraderSaver("Max");
	market->setTrader(trader);
	trader->setMarket(market);


	market->runFullTest();

	Candle c;
	c.open = 1;
	c.high = 2;
	c.low = 3;
	c.close = 40;
	c.volume = 50;

	CandlesVectorMap expected = {};
	c.datetime_index = 0;
	expected["share"].push_back(c);
	c.datetime_index = 0;
	expected["share"].push_back(c);
	c.datetime_index = 0;
	expected["share"].push_back(c);
	c.datetime_index = 0;
	expected["share"].push_back(c);

	auto actual = trader->recieved;
	remove("share.csv");
	BOOST_CHECK_EQUAL(expected, actual);

	delete market;
	delete trader;
}

BOOST_AUTO_TEST_CASE(TestBuying) {
	string tofile = "datetime,close,high,low,open,volume\n";
	tofile += "1998-08-04 10:00:00,1,2,3,5000,50\n"; //0
	tofile += "1998-08-05 10:00:00,1,2,3,5000,50\n";
	tofile += "1998-08-06 10:00:00,1,2,3,5000,50\n";
	tofile += "1998-08-07 10:00:00,1,2,3,5000,50\n";
	tofile += "1998-08-08 10:00:00,1,2,3,5000,50\n";
	tofile += "1998-08-09 10:00:00,1,2,3,5000,50\n"; //5
	tofile += "1998-08-10 10:00:00,1,2,3,4500,50\n"; //6
	tofile += "1998-08-11 10:00:00,1,2,3,4200,50\n";
	tofile += "1998-08-12 10:00:00,1,2,3,4000,50\n";
	tofile += "1998-08-13 10:00:00,1,2,3,4000,50\n"; //9
	ofstream out("share.csv");
	if (out.is_open()) {
		out << tofile;
		out.close();
	}

	class TraderBuyer : public  Trader {
	public:
		TraderBuyer(string identifier) :Trader(identifier) {};
		~TraderBuyer() {};
		int tick = 0;
		void newTickAction(std::vector<OrderChange> matches,
						   std::unordered_map<std::string, AnonimousMaketDepthVector> depth) {
			if (tick == 5) {
				this->makeOrder(this->createLimitOrder("share", 5, 4100));
			}
			tick++;
		}
		void newCandlesAction(std::string ticker, std::vector<Candle> candles) {

		}
	};

	SingleTraderMarket* market = new SingleTraderMarket();
	market->loadHistoryData("", { "share" });
	TraderBuyer* trader = new TraderBuyer("Max");
	market->setTrader(trader);
	trader->setMarket(market);

	market->setComission("share", 5);

	market->runFullTest();

	unordered_map<string, int> expectedPortfolio = { { "share", 5 },{ "money", 5 * -4100 - 5 * 5 } };

	auto actualPortfolio = market->getPortfio();
	remove("share.csv");
	BOOST_CHECK_EQUAL(expectedPortfolio, actualPortfolio);

	delete market;
	delete trader;
}

BOOST_AUTO_TEST_CASE(TestSelling) {
	string tofile = "datetime,close,high,low,open,volume\n";
	tofile += "1998-08-04 10:00:00,1,2,3,4000,5000\n"; //0
	tofile += "1998-08-05 10:00:00,1,2,3,4100,5000\n";
	tofile += "1998-08-06 10:00:00,1,2,3,4000,5000\n";
	tofile += "1998-08-07 10:00:00,1,2,3,4000,5000\n";
	tofile += "1998-08-08 10:00:00,1,2,3,4000,5000\n";
	tofile += "1998-08-09 10:00:00,1,2,3,4000,5000\n"; //5
	tofile += "1998-08-10 10:00:00,1,2,3,4100,5000\n"; //6
	tofile += "1998-08-11 10:00:00,1,2,3,4200,5000\n";
	tofile += "1998-08-12 10:00:00,1,2,3,4500,5000\n";
	tofile += "1998-08-13 10:00:00,1,2,3,4000,5000\n"; //9
	ofstream out("share.csv");
	if (out.is_open()) {
		out << tofile;
		out.close();
	}

	class TraderSeller : public  Trader {
	public:
		TraderSeller(string identifier) :Trader(identifier) {};
		~TraderSeller() {};
		int tick = 0;
		void newTickAction(std::vector<OrderChange> matches,
						   std::unordered_map<std::string, AnonimousMaketDepthVector> depth) {
			if (tick == 5) {
				this->makeOrder(this->createLimitOrder("share", -5, 4400));
			}
			tick++;
		}
		void newCandlesAction(std::string ticker, std::vector<Candle> candles) {

		}
	};

	SingleTraderMarket* market = new SingleTraderMarket();
	market->loadHistoryData("", { "share" });
	TraderSeller* trader = new TraderSeller("Max");
	market->setTrader(trader);
	trader->setMarket(market);

	market->setComission("share", 5);

	market->runFullTest();

	unordered_map<string, int> expectedPortfolio = { { "share", -5 },{ "money", 5 * 4400 - 5 * 5 } };

	auto actualPortfolio = market->getPortfio();
	remove("share.csv");
	BOOST_CHECK_EQUAL(expectedPortfolio, actualPortfolio);

	delete market;
	delete trader;
}

BOOST_AUTO_TEST_CASE(TestDepthRecieving) {
	string tofile = "datetime,close,high,low,open,volume\n";
	tofile += "1998-08-04 10:00:00,1,2,3,1000,50\n"; //0
	tofile += "1998-08-05 10:00:00,1,2,3,2000,50\n";
	tofile += "1998-08-06 10:00:00,1,2,3,3000,50\n";
	tofile += "1998-08-07 10:00:00,1,2,3,4000,50\n";
	tofile += "1998-08-08 10:00:00,1,2,3,5000,50\n";
	tofile += "1998-08-09 10:00:00,1,2,3,6000,50\n"; //5
	tofile += "1998-08-10 10:00:00,1,2,3,7000,50\n"; //6
	tofile += "1998-08-11 10:00:00,1,2,3,8000,50\n";
	tofile += "1998-08-12 10:00:00,1,2,3,9000,50\n";
	tofile += "1998-08-13 10:00:00,1,2,3,10000,50\n"; //9
	ofstream out("share.csv");
	if (out.is_open()) {
		out << tofile;
		out.close();
	}

	class TraderDepthSaver : public  Trader {
	public:
		TraderDepthSaver(string identifier) :Trader(identifier) {};
		~TraderDepthSaver() {};
		int tick = 0;
		void newTickAction(std::vector<OrderChange> matches,
						   std::unordered_map<std::string, AnonimousMaketDepthVector> depth) {
			if (tick == 5) {
				depth5 = depth["share"];
			}
			tick++;
		}
		void newCandlesAction(std::string ticker, std::vector<Candle> candles) {

		}
		AnonimousMaketDepthVector depth5;
	};

	SingleTraderMarket* market = new SingleTraderMarket();
	market->loadHistoryData("", { "share" });
	TraderDepthSaver* trader = new TraderDepthSaver("Max");
	market->setTrader(trader);
	trader->setMarket(market);


	market->runFullTest();

	AnonimousMaketDepthVector expectedDepth =
	{
		{ 10, 4000 },
		{ 10, 4100 },
		{ 10, 4200 },
		{ 10, 4300 },
		{ 10, 4400 },
		{ 10, 4500 },
		{ 10, 4600 },
		{ 10, 4700 },
		{ 10, 4800 },
		{ 10, 4900 },
		{ 10, 5000 },
		{ 10, 5100 },
		{ 10, 5200 },
		{ 10, 5300 },
		{ 10, 5400 },
		{ 10, 5500 },
		{ 10, 5600 },
		{ 10, 5700 },
		{ 10, 5800 },
		{ 10, 5900 },

		{ -10, 6100 },
		{ -10, 6200 },
		{ -10, 6300 },
		{ -10, 6400 },
		{ -10, 6500 },
		{ -10, 6600 },
		{ -10, 6700 },
		{ -10, 6800 },
		{ -10, 6900 },
		{ -10, 7000 },
		{ -10, 7100 },
		{ -10, 7200 },
		{ -10, 7300 },
		{ -10, 7400 },
		{ -10, 7500 },
		{ -10, 7600 },
		{ -10, 7700 },
		{ -10, 7800 },
		{ -10, 7900 },
		{ -10, 8000 },
	};

	auto actualDepth = trader->depth5;
	remove("share.csv");
	BOOST_CHECK_EQUAL(expectedDepth, actualDepth);

	delete market;
	delete trader;
}

BOOST_AUTO_TEST_CASE(ImmOrCancelOrder) {
	MarketDepth m("s1");

	vector<Order> orders;
	orders.push_back(Order::Make_Limit_Order("history", "share", 1, 1));
	orders.push_back(Order::Make_Limit_Order("history", "share", 1, 2));
	orders.push_back(Order::Make_Limit_Order("history", "share", 1, 3));
	orders.push_back(Order::Make_Limit_Order("history", "share", 1, 4));
	orders.push_back(Order::Make_Limit_Order("history", "share", 1, 5));
	orders.push_back(Order::Make_Limit_Order("history", "share", 1, 5));

	for (Order o : orders) {
		m.addOrder(o);
	}

	vector<OrderChange> actualChanges = m.addOrder(Order::Make_IoC_Order("Max", "share", -10, 4));

	vector<OrderChange> expectedChanges;

	OrderChange change;
	change.volumeChange = -10;
	change.currentVolume = -10;
	change.ticker = "share";
	change.traderIdentifier = "Max";
	change.reason = ChangeReason::Placed;
	change.worstPossiblePrice = 4;
	change.matchPrice = 0;

	expectedChanges.push_back(change);  //add

	change.volumeChange = 1;
	change.currentVolume = -9;
	change.reason = ChangeReason::Matched;
	change.worstPossiblePrice = 4;
	change.matchPrice = 5;

	expectedChanges.push_back(change); //match 1st 5


	change.volumeChange = 1;
	change.currentVolume = -8;
	change.reason = ChangeReason::Matched;
	change.worstPossiblePrice = 4;
	change.matchPrice = 5;

	expectedChanges.push_back(change); //match 2nd 5

	change.volumeChange = 1;
	change.currentVolume = -7;
	change.reason = ChangeReason::Matched;
	change.worstPossiblePrice = 4;
	change.matchPrice = 4;

	expectedChanges.push_back(change); //match 4


	change.volumeChange = 7;
	change.currentVolume = 0;
	change.reason = ChangeReason::Cancelled;
	change.worstPossiblePrice = 4;
	change.matchPrice = 0;

	expectedChanges.push_back(change); //cancelled

	MarketDepthDataMap actuaDepth = m.getInternalDepth();

	MarketDepthDataMap expectedDepth{
		{ 1 ,{ Order::Make_Limit_Order("history", "share", 1, 1) } },
		{ 2 ,{ Order::Make_Limit_Order("history", "share", 1, 2) } },
		{ 3,{ Order::Make_Limit_Order("history", "share", 1, 3) } }
	};

	BOOST_CHECK_EQUAL(expectedDepth, actuaDepth);
	BOOST_CHECK_EQUAL(expectedChanges, actualChanges);
}


BOOST_AUTO_TEST_CASE(CancellingOrders) {
	string tofile = "datetime,close,high,low,open,volume\n";
	tofile += "1998-08-04 10:00:00,1,2,3,1000,50\n"; //0
	tofile += "1998-08-05 10:00:00,1,2,3,1000,50\n";
	tofile += "1998-08-06 10:00:00,1,2,3,1000,50\n";
	tofile += "1998-08-07 10:00:00,1,2,3,1000,50\n";
	tofile += "1998-08-08 10:00:00,1,2,3,1000,50\n";
	tofile += "1998-08-09 10:00:00,1,2,3,1000,50\n"; //5
	tofile += "1998-08-10 10:00:00,1,2,3,1000,50\n"; //6
	tofile += "1998-08-11 10:00:00,1,2,3,1000,50\n";
	tofile += "1998-08-12 10:00:00,1,2,3,1000,50\n";
	tofile += "1998-08-13 10:00:00,1,2,3,1000,50\n"; //9
	ofstream out("share.csv");
	if (out.is_open()) {
		out << tofile;
		out.close();
	}

	class TraderCanceller : public  Trader {
	public:
		TraderCanceller(string identifier) :Trader(identifier) {};
		~TraderCanceller() {};
		int tick = 0;
		void newTickAction(std::vector<OrderChange> matches,
						   std::unordered_map<std::string, AnonimousMaketDepthVector> depth) {
			if (tick == 0) {
				this->makeOrder(this->createLimitOrder("share", 100, 900));
				this->makeOrder(this->createLimitOrder("share", -100, 1100));
			}
			if (tick == 1) {
				this->changeOrder(this->createLimitOrder("share", 0, 900));
				this->changeOrder(this->createLimitOrder("share", 0, 1100));
			}
			tick++;
		}
		void newCandlesAction(std::string ticker, std::vector<Candle> candles) {

		}
		AnonimousMaketDepthVector depth5;
	};

	SingleTraderMarket* market = new SingleTraderMarket();
	market->loadHistoryData("", { "share" });
	TraderCanceller* trader = new TraderCanceller("Max");
	market->setTrader(trader);
	trader->setMarket(market);


	market->runFullTest();

	std::unordered_map<std::string, std::unordered_map<int, int>> expectedOrders; //empty
	auto actuaDepth = trader->orders;
	remove("share.csv");
	BOOST_CHECK_EQUAL(expectedOrders, actuaDepth);
}

BOOST_AUTO_TEST_SUITE_END()