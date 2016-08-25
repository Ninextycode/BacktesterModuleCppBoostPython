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
				stringstream message;
				message << "map: string-ins\n";
				for (auto it = d.begin(); it != d.end(); it++) {
					message << it->first << " " << it->second << "\n";
				}
				auto s = message.str();
				auto w_message = wstring(s.begin(), s.end());

				return w_message;
			}

			template<> static wstring ToString<AnonimousMaketDepth>
				(const AnonimousMaketDepth& d) {
				stringstream message;
				message << "AnonimousMaketDepth: \n";
				for (auto it = d.begin(); it != d.end(); it++) {
					message << *it << "\n";
				}
				auto s = message.str();
				auto w_message = wstring(s.begin(), s.end());

				return w_message;
			}

			template<> static wstring ToString<vector<Order>>
				(const vector<Order>& d) {
				stringstream message;
				message << "Vector of orders\n";
				for (auto it = d.begin(); it != d.end(); it++) {
					message << *it << "\n";
				}
				auto s = message.str();
				auto w_message = wstring(s.begin(), s.end());

				return w_message;
			}

			template<> static wstring ToString<CandesVectorMap>
				(const CandesVectorMap& d) {
				stringstream message;
				message << "map: string - vector of candles\n";
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


			template<> static wstring ToString<MarketDepthData>
				(const MarketDepthData& d) {
				stringstream message;
				message << "MarketDepthData\n";
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
				stringstream message;
				message << "vector of OrderChange\n";
				for (auto it = d.begin(); it != d.end(); it++) {
					message << *it << "\n";
				}
				auto s = message.str();
				auto w_message = wstring(s.begin(), s.end());

				return w_message;
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

		//Cannot export OrdersFromCandleBuilder. Uncommented won't compile
		/*TEST_METHOD(OrdersFromCandleBuilder) {

			Candle c;
			c.open = 1;
			c.high = 2;
			c.low = 3;
			c.close = 10;
			c.volume = 5;
			c.datetime = boost::posix_time::time_from_string("1998-08-07 10:00:00");

			vector<Order> actual = OrdersFromCandleBuilder::ordersFromCandle(c, "share", 10, 2);


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


			
			Assert::AreEqual(expectedChanges, changes);
		}

		TEST_METHOD(MatchingOrders_OrderCancelReport) {
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

			Assert::AreEqual(expected_changes, changes);

		}

		TEST_METHOD(AddingOrders_DeprioritizeHistoryOrders) {
			MarketDepth m("s1");
			
			Order notHistoryOrder = Order::Make_Limit_Order("Max", "share", 1, 1);
			//check that OrdersFromCandleBuilder::HISTORY_IDENTIFIER == history
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
			SingleTraderMarket m;
			m.loadHistoryData("", { "share" });
			auto actual = m.getInternalHistoryCandles();
			
			Candle c;
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
				void newCandlesAction(std::string ticker, std::vector<Candle> candles) override {
					recieved[ticker] = candles;
				}
				CandesVectorMap recieved;
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
				void newCandlesAction(std::string ticker, std::vector<Candle> candles) {

				}
				AnonimousMaketDepth depth5;
			};

			SingleTraderMarket* market = new SingleTraderMarket();
			market->loadHistoryData("", { "share" });
			TraderDepthSaver* trader = new TraderDepthSaver("Max");
			market->setTrader(trader);
			trader->setMarket(market);


			market->runFullTest();

			AnonimousMaketDepth expectedDepth =
			{
				{ 10, 40 },
				{ 10, 41 },
				{ 10, 42 },
				{ 10, 43 },
				{ 10, 44 },
				{ 10, 45 },
				{ 10, 46 },
				{ 10, 47 },
				{ 10, 48 },
				{ 10, 49 },
				{ 10, 50 },
				{ 10, 51 },
				{ 10, 52 },
				{ 10, 53 },
				{ 10, 54 },
				{ 10, 55},
				{ 10, 56 }, 
				{ 10, 57}, 
				{ 10, 58 }, 
				{ 10, 59 }, 

				{ -10, 61 }, 
				{ -10, 62 }, 
				{ -10, 63 }, 
				{ -10, 64 }, 
				{ -10, 65 }, 
				{ -10, 66 },
				{ -10, 67 },
				{ -10, 68 },
				{ -10, 69 },
				{ -10, 70 },
				{ -10, 71 },
				{ -10, 72 },
				{ -10, 73 },
				{ -10, 74 },
				{ -10, 75 },
				{ -10, 76 },
				{ -10, 77 },
				{ -10, 78 },
				{ -10, 79 },
				{ -10, 80 },
			};

			auto actualDepth = trader->depth5;
			Assert::AreEqual(expectedDepth, actualDepth);

			delete market;
			delete trader;
		}

		TEST_METHOD(ImmOrCancelOrder) {
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

			MarketDepthData actuaDepth = m.getInternalDepth();

			MarketDepthData expectedDepth{
				{ 1 ,{ Order::Make_Limit_Order("history", "share", 1, 1) } },
				{ 2 ,{ Order::Make_Limit_Order("history", "share", 1, 2) } },
				{ 3, { Order::Make_Limit_Order("history", "share", 1, 3) } }
			};

			Assert::AreEqual(expectedDepth, actuaDepth);
			Assert::AreEqual(expectedChanges, actualChanges);
		}

	private:
		MarketDepth marketDepth;
	};
}