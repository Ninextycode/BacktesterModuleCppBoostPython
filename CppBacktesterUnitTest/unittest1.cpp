#include "stdafx.h"
#include "CppUnitTest.h"


#include "../BoostPythonBacktesterModuleCpp/Backtester.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
using namespace std;
namespace Microsoft {
	namespace  VisualStudio {
		namespace CppUnitTestFramework {
			
			template<> static wstring ToString<unordered_map<string, int>>
				(const unordered_map<string, int>& d) {
				return L"Wrong portfolio";
			}
			template<> static wstring ToString<AnonimousMaketDepth>
				(const AnonimousMaketDepth& d) {
				return L"Wrong Anonimous Depth";
			}
			template<> static wstring ToString<vector<Order>>
				(const vector<Order>& d) {
				return L"Wrong vector of orders";
			}
			template<> static wstring ToString<CandesVectorMap>
				(const CandesVectorMap& d) {
				return L"Wrong CandesVectorMap";
			}
			template<> static wstring ToString<MarketDepthData>
				(const MarketDepthData& d) {

				stringstream message;
				message << "\n";
				for (auto it = d.begin(); it != d.end(); it++) {
					message << it->first << " : ";
					for (auto it2 = it->second.begin(); it2 != it->second.end(); it2++) {
						message << (*it2) << " |";
					}
					message << "\n";
				}
				auto s = message.str();
				auto w_message = wstring(s.begin(), s.end());

				return w_message;
				
			}

			template<> static wstring ToString<vector<OrderChange>>
				(const vector<OrderChange>& d) {
				return L"Wrong Changes Track";
			}
		}
	}
}


namespace CppBacktesterUnitTest {	

	TEST_CLASS(UnitTest1) {
	public:

		TEST_METHOD(AddingOrders) {

			MarketDepth m("s1");
			Order o_buy = Order::Make_Limit_Order("Max", "share", 1, 10);
			Order o_sell = Order::Make_Limit_Order("Max","share", -1, 11);
			m.addOrder(o_buy);
			m.addOrder(o_sell);
			m.addOrder(o_sell);
			MarketDepthData expectedDepth { 
				{ 10, { o_buy } },
				{ 11 , { o_sell, o_sell } } 
			};

			Assert::AreEqual(expectedDepth, m.getInternalDepth());


		}	

		TEST_METHOD(AddingOrders_TestOrdering) {

			MarketDepth m("s1");
			Order o_buy = Order::Make_Limit_Order("Max", "share", 1, 10);
			Order o_sell = Order::Make_Limit_Order("Max", "share", -1, 11);
			m.addOrder(o_buy);
			m.addOrder(o_sell);
			m.addOrder(o_sell);

			Order o_sell2 = Order::Make_Limit_Order("Max", "share", -15, 11);
			m.addOrder(o_sell2);

			MarketDepthData expectedDepth = {
				{ 10 ,{ o_buy } },
				{ 11 ,{ o_sell2,  o_sell, o_sell } }
			};

			Assert::AreEqual(expectedDepth, m.getInternalDepth());
		}

		TEST_METHOD(MatchingOrders_Partial) {

			MarketDepth m("s1");
			Order o_buy = Order::Make_Limit_Order("Max", "share", 10, 10);
			Order o_sell = Order::Make_Limit_Order("Max", "share", -10, 11);
			m.addOrder(o_buy);
			m.addOrder(o_sell);
			
			Order o_match_sell = Order::Make_Limit_Order("Max", "share", 5, 11);
			Order o_match_buy = Order::Make_Limit_Order("Max", "share", -5, 10);

			m.addOrder(o_match_sell);
			m.addOrder(o_match_buy);

			MarketDepthData expectedDepth{
				{ 10 ,{ Order::Make_Limit_Order("Max", "share", 5, 10) } },
				{ 11 ,{ Order::Make_Limit_Order("Max", "share", -5, 11) } }
			};
			Assert::AreEqual(expectedDepth, m.getInternalDepth());
		}


		TEST_METHOD(MatchingOrders_Full) {

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

			MarketDepthData expectedDepth{
				
			};
			Assert::AreEqual(expectedDepth, m.getInternalDepth());
		}

		//Cannot export OrdersFromCandelBuilder. Uncommented won't compile
		/*TEST_METHOD(OrdersFromCandelBuilder) {

			Candel c;
			c.open = 1;
			c.high = 2;
			c.low = 3;
			c.close = 10;
			c.volume = 5;
			c.datetime = boost::posix_time::time_from_string("1998-08-07 10:00:00");

			vector<Order> actual = OrdersFromCandelBuilder::ordersFromCandel(c, "share", 10, 2);


			vector<Order> ecpected{
				Order::Make_Limit_Order("history", "share", 10, 8),
				Order::Make_Limit_Order("history", "share", 10, 9),

				Order::Make_Limit_Order("history", "share", -10, 11),
				Order::Make_Limit_Order("history", "share", -10, 12)
			};

			Assert::AreEqual(ecpected, actual);

		}*/

		TEST_METHOD(MatchingOrders_OverBuyOverSell) {
			MarketDepth m("s1");
			Order o_sell = Order::Make_Limit_Order("Max", "share", -10, 11);
			m.addOrder(o_sell);

			Order o_match_sell = Order::Make_Limit_Order("Max", "share", 20, 12);

			m.addOrder(o_match_sell);

			MarketDepthData expectedDepth{
				{ 12 ,{ Order::Make_Limit_Order("Max", "share", 10, 12)} }
			};
			Assert::AreEqual(expectedDepth, m.getInternalDepth());
		}

		TEST_METHOD(MatchingOrders_OrderChangerReport) {
			MarketDepth m("s1");
			Order first_order = Order::Make_Limit_Order("Fin", "share", 10, 10);
			m.addOrder(first_order);

			Order second_order = Order::Make_Limit_Order("Stan", "share", -10, 5);

			auto changes = m.addOrder(second_order);
			OrderChange finChange;
			
			finChange.currentVolume = 0;
			finChange.worstPossiblePrice = 10;
			finChange.reason = ChangeReason::Match;
			finChange.ticker = "share";
			finChange.trader_identifier = "Fin";
			finChange.volumeChange = -10;
			finChange.matchPrice = 10;

			OrderChange stanChange;

			stanChange.currentVolume = 0;
			stanChange.worstPossiblePrice = 5;
			stanChange.reason = ChangeReason::Match;
			stanChange.ticker = "share";
			stanChange.trader_identifier = "Stan";
			stanChange.volumeChange = 10;
			finChange.matchPrice = 10;

			vector<OrderChange> expected_changes = { finChange , stanChange };
			
			Assert::AreEqual(expected_changes, changes);
		}

		TEST_METHOD(MatchingOrders_OrderCancelReport) {
			MarketDepth m("s1");
			Order o_sell = Order::Make_Limit_Order("Max", "share", -10, 11);
			m.addOrder(o_sell);

			auto changes = m.changeOrder(Order::Make_Limit_Order("Max", "share", 0, 11));

			OrderChange change;

			change.currentVolume = 0;
			change.worstPossiblePrice = 11;
			change.reason = ChangeReason::Cancellation;
			change.ticker = "share";
			change.trader_identifier = "Max";
			change.volumeChange = 10;

			vector<OrderChange> expected_changes = { change };

			Assert::AreEqual(expected_changes, changes);

		}

		TEST_METHOD(AddingOrders_DeprioritizeHistoryOrders) {
			MarketDepth m("s1");
			
			Order notHistoryOrder = Order::Make_Limit_Order("Max", "share", 1, 1);
			//check that OrdersFromCandelBuilder::HISTORY_IDENTIFIER == history
			Order historyOrder = 
				Order::Make_Limit_Order("history", "share", 1, 1);

			m.addOrder(notHistoryOrder);
			m.addOrder(historyOrder);

			MarketDepthData expectedDepth{
				{ 1 ,{ notHistoryOrder,  historyOrder } }
			};
			Assert::AreEqual(expectedDepth, m.getInternalDepth());
		}

		TEST_METHOD(AnonimousDepthConstruction) {
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


			AnonimousMaketDepth expectedDepth = { 
				{1, 3}, {1, 4}, {2, 5},
				{-2, 6}, {-1, 7}
			};

			for (Order o : orders) {
				m.addOrder(o);
			}

			Assert::AreEqual(expectedDepth, m.getAnonimousDepth(3));
		}

		TEST_METHOD(ReadingCsvFile) {
			string tofile = "datetime,close,high,low,open,volume\n";
			tofile += "1998-08-04 10:00:00,1,2,3,4,5\n";
			tofile += "1998-08-05 10:00:00,1,2,3,4,5\n";
			tofile += "1998-08-06 10:00:00,1,2,3,4,5\n";
			ofstream out("share.csv");
			if (out.is_open()) {
				out << tofile;
				out.close();
			}
			SngleTraderMarket m;
			m.loadHistoryData("", { "share" });
			auto actual = m.getInternalHistoryCandles();
			
			Candel c;
			c.open = 1;
			c.high = 2;
			c.low = 3;
			c.close = 4;
			c.volume = 5;

			CandesVectorMap expected = {};
			c.datetime = boost::posix_time::time_from_string("1998-08-04 10:00:00");
			expected["share"].push_back(c);
			c.datetime = boost::posix_time::time_from_string("1998-08-05 10:00:00");
			expected["share"].push_back(c);
			c.datetime = boost::posix_time::time_from_string("1998-08-06 10:00:00");
			expected["share"].push_back(c);
			Assert::AreEqual(expected, actual);
		}

		TEST_METHOD(RecievingCandlesByRequestTest) {
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
				TraderSaver(string identifier) :Trader(identifier){};
				~TraderSaver() {};
				int tick = 0;
				void newTickAction(std::vector<OrderChange> matcher, 
								std::unordered_map<std::string, AnonimousMaketDepth> depth) {
					if (tick == 5) {
						this->market->requestCandles("share", 4);
					}
					tick++;
				}
				void newCandelsAction(std::string ticker, std::vector<Candel> candels) override {
					recieved[ticker] = candels;
				}
				CandesVectorMap recieved;
			};

			SngleTraderMarket* market = new SngleTraderMarket();
			market->loadHistoryData("", { "share" });
			TraderSaver* trader = new TraderSaver("Max");
			market->setTrader(trader);
			trader->setMarket(market);


			market->runFullTest();

			Candel c;
			c.open = 1;
			c.high = 2;
			c.low = 3;
			c.close = 40;
			c.volume = 50;

			CandesVectorMap expected = {};
			c.datetime = boost::posix_time::time_from_string("1998-08-07 10:00:00");
			expected["share"].push_back(c);
			c.datetime = boost::posix_time::time_from_string("1998-08-08 10:00:00");
			expected["share"].push_back(c);
			c.datetime = boost::posix_time::time_from_string("1998-08-09 10:00:00");
			expected["share"].push_back(c);
			c.datetime = boost::posix_time::time_from_string("1998-08-10 10:00:00");
			expected["share"].push_back(c);

			auto actual = trader->recieved;
			Assert::AreEqual(expected , actual);
			
			delete market;
			delete trader;
		}

		TEST_METHOD(TestBuying) {
			string tofile = "datetime,close,high,low,open,volume\n";
			tofile += "1998-08-04 10:00:00,1,2,3,50,50\n"; //0
			tofile += "1998-08-05 10:00:00,1,2,3,50,50\n";
			tofile += "1998-08-06 10:00:00,1,2,3,50,50\n";
			tofile += "1998-08-07 10:00:00,1,2,3,50,50\n";
			tofile += "1998-08-08 10:00:00,1,2,3,50,50\n";
			tofile += "1998-08-09 10:00:00,1,2,3,50,50\n"; //5
			tofile += "1998-08-10 10:00:00,1,2,3,45,50\n"; //6
			tofile += "1998-08-11 10:00:00,1,2,3,42,50\n";
			tofile += "1998-08-12 10:00:00,1,2,3,40,50\n";
			tofile += "1998-08-13 10:00:00,1,2,3,40,50\n"; //9
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
								std::unordered_map<std::string, AnonimousMaketDepth> depth) {
					if (tick == 5) {
						this->makeOrder(this->createLimitOrder("share", 5, 41));
					}
					tick++;
				}
				void newCandelsAction(std::string ticker, std::vector<Candel> candels) {

				}
			};

			SngleTraderMarket* market = new SngleTraderMarket();
			market->loadHistoryData("", { "share" });
			TraderBuyer* trader = new TraderBuyer("Max");
			market->setTrader(trader);
			trader->setMarket(market);

			market->setComission("share", 5);

			market->runFullTest();

			unordered_map<string, int> expectedPortfolio = { {"share", 5}, {"money", 5*-41 - 5*5} };

			auto actualPortfolio = market->getPortfio();
			Assert::AreEqual(expectedPortfolio, actualPortfolio);

			delete market;
			delete trader;
		}

		TEST_METHOD(TestSelling) {
			string tofile = "datetime,close,high,low,open,volume\n";
			tofile += "1998-08-04 10:00:00,1,2,3,40,50\n"; //0
			tofile += "1998-08-05 10:00:00,1,2,3,41,50\n";
			tofile += "1998-08-06 10:00:00,1,2,3,40,50\n";
			tofile += "1998-08-07 10:00:00,1,2,3,40,50\n";
			tofile += "1998-08-08 10:00:00,1,2,3,40,50\n";
			tofile += "1998-08-09 10:00:00,1,2,3,40,50\n"; //5
			tofile += "1998-08-10 10:00:00,1,2,3,41,50\n"; //6
			tofile += "1998-08-11 10:00:00,1,2,3,42,50\n";
			tofile += "1998-08-12 10:00:00,1,2,3,45,50\n";
			tofile += "1998-08-13 10:00:00,1,2,3,40,50\n"; //9
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
								std::unordered_map<std::string, AnonimousMaketDepth> depth) {
					if (tick == 5) {
						this->makeOrder(this->createLimitOrder("share", -5, 44)); 
					}
					tick++;
				}
				void newCandelsAction(std::string ticker, std::vector<Candel> candels) {

				}
			};

			SngleTraderMarket* market = new SngleTraderMarket();
			market->loadHistoryData("", { "share" });
			TraderSeller* trader = new TraderSeller("Max");
			market->setTrader(trader);
			trader->setMarket(market);

			market->setComission("share", 5);

			market->runFullTest();

			unordered_map<string, int> expectedPortfolio = { { "share", -5 },{ "money", 5*44 - 5*5 } };

			auto actualPortfolio = market->getPortfio();
			Assert::AreEqual(expectedPortfolio, actualPortfolio);

			delete market;
			delete trader;
		}

		TEST_METHOD(TestDepthRecieving) {
			string tofile = "datetime,close,high,low,open,volume\n";
			tofile += "1998-08-04 10:00:00,1,2,3,10,50\n"; //0
			tofile += "1998-08-05 10:00:00,1,2,3,20,50\n";
			tofile += "1998-08-06 10:00:00,1,2,3,30,50\n";
			tofile += "1998-08-07 10:00:00,1,2,3,40,50\n";
			tofile += "1998-08-08 10:00:00,1,2,3,50,50\n";
			tofile += "1998-08-09 10:00:00,1,2,3,60,50\n"; //5
			tofile += "1998-08-10 10:00:00,1,2,3,70,50\n"; //6
			tofile += "1998-08-11 10:00:00,1,2,3,80,50\n";
			tofile += "1998-08-12 10:00:00,1,2,3,90,50\n";
			tofile += "1998-08-13 10:00:00,1,2,3,100,50\n"; //9
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
								std::unordered_map<std::string, AnonimousMaketDepth> depth) {
					if (tick == 5) {
						depth5 = depth["share"];
					}
					tick++;
				}
				void newCandelsAction(std::string ticker, std::vector<Candel> candels) {

				}
				AnonimousMaketDepth depth5;
			};

			SngleTraderMarket* market = new SngleTraderMarket();
			market->loadHistoryData("", { "share" });
			TraderDepthSaver* trader = new TraderDepthSaver("Max");
			market->setTrader(trader);
			trader->setMarket(market);


			market->runFullTest();

			AnonimousMaketDepth expectedDepth =
			{
				{ 5, 55},
				{ 5, 56 }, 
				{ 5, 57}, 
				{ 5, 58 }, 
				{ 5, 59 }, 
				{ -5, 61 }, 
				{ -5, 62 }, 
				{ -5, 63 }, 
				{ -5, 64 }, 
				{ -5, 65 }, 
			};

			auto actualDepth = trader->depth5;
			Assert::AreEqual(expectedDepth, actualDepth);

			delete market;
			delete trader;
		}

	private:
		MarketDepth marketDepth;
	};
}