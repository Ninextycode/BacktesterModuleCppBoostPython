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
			template<> static wstring ToString<AnonimousMaketDepth>
				(const AnonimousMaketDepth& d) {
				return L"Wrong Anonimous Depth";
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
		}
	}
}


namespace CppBacktesterUnitTest {	

	TEST_CLASS(UnitTest1) {
	public:
		TEST_METHOD(TestMethod1) {
			MarketDepth m();
			auto x = AnonimousMaketDepth{
				std::tuple<int, int>(1, 2),
				std::tuple<int, int>(3, 4)
			};

			auto y = marketDepth.getAnonimousDepth(1);

			
			Assert::IsTrue(true);
			Assert::AreEqual(x, (*marketDepth.getAnonimousDepth(1)));
		}

		TEST_METHOD(AddingOrders) {

			MarketDepth m("s1");
			Order o_buy = Order::Make_Limit_Order("Max", "stock", 1, 10);
			Order o_sell = Order::Make_Limit_Order("Max","stock", -1, 11);
			m.addOrder(o_buy);
			m.addOrder(o_sell);
			m.addOrder(o_sell);
			MarketDepthData expectedDepth { 
				{ 10, { o_buy } },
				{ 11 , { o_sell, o_sell } } 
			};

			Assert::AreEqual(expectedDepth, m.getInternalDepth());


		}	

		TEST_METHOD(AddingOrders_TestSequense) {

			MarketDepth m("s1");
			Order o_buy = Order::Make_Limit_Order("Max", "stock", 1, 10);
			Order o_sell = Order::Make_Limit_Order("Max", "stock", -1, 11);
			m.addOrder(o_buy);
			m.addOrder(o_sell);
			m.addOrder(o_sell);

			Order o_sell2 = Order::Make_Limit_Order("Max", "stock", -15, 11);
			m.addOrder(o_sell2);

			MarketDepthData expectedDepth = {
				{ 10 ,{ o_buy } },
				{ 11 ,{ o_sell2,  o_sell, o_sell } }
			};

			Assert::AreEqual(expectedDepth, m.getInternalDepth());
		}

		TEST_METHOD(MatchingOrders_Partial) {

			MarketDepth m("s1");
			Order o_buy = Order::Make_Limit_Order("Max", "stock", 10, 10);
			Order o_sell = Order::Make_Limit_Order("Max", "stock", -10, 11);
			m.addOrder(o_buy);
			m.addOrder(o_sell);
			
			Order o_match_sell = Order::Make_Limit_Order("Max", "stock", 5, 11);
			Order o_match_buy = Order::Make_Limit_Order("Max", "stock", -5, 10);

			m.addOrder(o_match_sell);
			m.addOrder(o_match_buy);

			MarketDepthData expectedDepth{
				{ 10 ,{ Order::Make_Limit_Order("Max", "stock", 5, 10) } },
				{ 11 ,{ Order::Make_Limit_Order("Max", "stock", -5, 11) } }
			};
			Assert::AreEqual(expectedDepth, m.getInternalDepth());
		}


		TEST_METHOD(MatchingOrders_Full) {

			MarketDepth m("s1");
			Order o_buy = Order::Make_Limit_Order("Max", "stock", 10, 10);
			Order o_sell = Order::Make_Limit_Order("Max", "stock", -10, 11);
			m.addOrder(o_buy);
			m.addOrder(o_sell);

			o_buy = Order::Make_Limit_Order("Max", "stock", 10, 9);
			o_sell = Order::Make_Limit_Order("Max", "stock", -10, 12);

			m.addOrder(o_buy);
			m.addOrder(o_sell);

			Order o_match_sell = Order::Make_Limit_Order("Max", "stock", 20, 15);
			Order o_match_buy = Order::Make_Limit_Order("Max", "stock", -20, 5);

			m.addOrder(o_match_sell);
			m.addOrder(o_match_buy);

			MarketDepthData expectedDepth{
				
			};
			Assert::AreEqual(expectedDepth, m.getInternalDepth());
		}


		TEST_METHOD(MatchingOrders_OverBuyOverSell) {
			MarketDepth m("s1");
			Order o_sell = Order::Make_Limit_Order("Max", "stock", -10, 11);
			m.addOrder(o_sell);

			Order o_match_sell = Order::Make_Limit_Order("Max", "stock", 20, 12);

			m.addOrder(o_match_sell);

			MarketDepthData expectedDepth{
				{ 12 ,{ Order::Make_Limit_Order("Max", "stock", 10, 12)} }
			};
			Assert::AreEqual(expectedDepth, m.getInternalDepth());
		}

		TEST_METHOD(MatchingOrders_OverSellOverBuy) {
			MarketDepth m("s1");
			Order o_buy = Order::Make_Limit_Order("Max", "stock", 10, 10);
			m.addOrder(o_buy);

			Order o_match_buy = Order::Make_Limit_Order("Max", "stock", -20, 9);

			m.addOrder(o_match_buy);

			MarketDepthData expectedDepth{
				{ 9 ,{ Order::Make_Limit_Order("Max", "stock", -10, 9) } }
			};
			Assert::AreEqual(expectedDepth, m.getInternalDepth());
		}
	private:
		MarketDepth marketDepth;
	};
}