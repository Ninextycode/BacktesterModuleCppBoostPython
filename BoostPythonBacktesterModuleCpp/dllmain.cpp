// dllmain.cpp : Defines the entry point for the DLL application.

#define DLL_COMPILE

#include "stdafx.h"
#include "Backtester.h"
#include <boost/python.hpp>


using namespace boost::python;
using namespace std;

template<class T>
struct stdlist_to_python {
	static PyObject *convert(const std::list<T> &ls) {
		boost::python::list py_list;
		for (auto iter = ls.begin(); iter != ls.end(); ++iter) {
			py_list.append(*iter);
		}
		return incref(py_list.ptr());
	}
};

template<class T>
struct stdvector_to_python {
	static PyObject *convert(const vector<T> &ls) {
		boost::python::list py_list;
		for (auto iter = ls.begin(); iter != ls.end(); ++iter) {
			py_list.append(*iter);
		}
		return incref(py_list.ptr());
	}
};


template<class K, class V>
struct unordered_map_to_python {
	static PyObject *convert(const  unordered_map<K, V> &map) {
		dict dictionary;
		for (auto iter = map.begin(); iter != map.end(); ++iter) {
			dictionary[iter->first] = iter->second;
		}
		return incref(dictionary.ptr());
	}
};


struct TraderWrap : Trader, wrapper<Trader> {
	TraderWrap(string identifier):Trader(identifier){

	}
	void newCandelsAction(string ticker, vector<Candel> candels) override {
		this->get_override("new_candels_action")(ticker, candels);
	}
	void newTickAction(vector<OrderChange> matcher,
					   unordered_map<string, AnonimousMaketDepth> depths) override {
		this->get_override("new_tick_action")(matcher, depths);
	}
};

BOOST_PYTHON_MODULE(backtester) {
	//changes received by trader
	to_python_converter <vector<OrderChange>, stdvector_to_python<OrderChange>>();

	//candels received by trader
	to_python_converter <vector<Candel>, stdvector_to_python<Candel>>();

	//orders in trader of exact stock 
	to_python_converter <unordered_map<int, int>,
		unordered_map_to_python<int, int>>();

	//one depth received by trader
	to_python_converter <AnonimousMaketDepth, stdvector_to_python<AnonimousMarketDepthItem>>();

	//all depths received
	to_python_converter <unordered_map<string, AnonimousMaketDepth>,
		unordered_map_to_python<string, AnonimousMaketDepth>>();

	//orders in trader of all stocks
	to_python_converter <unordered_map<string, unordered_map<int, int>>, 
		unordered_map_to_python<string, unordered_map<int, int>>>();
	
	//portfolio
	to_python_converter< unordered_map<string, int>,
		unordered_map_to_python<string, int>>();


	//recieved changes
	enum_<ChangeReason>("ChangeReason")
		.value("Match", ChangeReason::Match)
		.value("Cancellation", ChangeReason::Cancellation)
		.value("Unknown", ChangeReason::Unknown);

	class_<AnonimousMarketDepthItem>("AnonimousMarketDepthItem") 
		.def(init<int, int>(args("price", "volume")))
		.def(init<>())
		.def_readwrite("price", &AnonimousMarketDepthItem::price)
		.def_readwrite("volume", &AnonimousMarketDepthItem::volume)
		.def(self == self)
		.def(self_ns::str(self_ns::self))
		.def(self_ns::repr(self_ns::self));	

	class_<OrderChange>("OrderChange")
		.def_readwrite("current_volume", &OrderChange::currentVolume)
		.def_readwrite("worst_possible_price", &OrderChange::worstPossiblePrice)
		.def_readwrite("match_price", &OrderChange::matchPrice)
		.def_readwrite("order_type", &OrderChange::reason)
		.def_readwrite("ticker", &OrderChange::ticker)
		.def_readwrite("trader_identifier", &OrderChange::trader_identifier)

		.def_readwrite("ticker", &OrderChange::ticker)
		.def(self == self)
		.def(self_ns::str(self_ns::self))
		.def(self_ns::repr(self_ns::self));

	class_<Order>("Order")
		.def_readwrite("volume", &Order::volume)
		.def_readwrite("price", &Order::price)
		.def_readwrite("orderType", &Order::orderType)

		.def_readwrite("trader_identifier", &Order::trader_identifier)

		.def_readwrite("ticker", &Order::ticker)

		.def("make_limit_order", &Order::Make_Limit_Order,
			 args("trader_identifier", "ticker", "volume", "price"))
		.staticmethod("make_limit_order")
		.def(self == self)
		.def(self_ns::str(self_ns::self))
		.def(self_ns::repr(self_ns::self));


	class_<SngleTraderMarket>("Market")
		.def("set_trader", &SngleTraderMarket::setTrader)
		.def("load_history_data", &SngleTraderMarket::loadHistoryData)
		.def("run_full_test", &SngleTraderMarket::runFullTest)
		.def("request_candles", &SngleTraderMarket::requestCandles)
		.def("set_comission", &SngleTraderMarket::setComission)
		.def_readwrite("DEPTH_LENGTH_PUBLIC", &SngleTraderMarket::DEPTH_LENGTH_PUBLIC);

	class_<TraderWrap, boost::noncopyable>("Trader", init<string>(args("identifier")))		
		.def("tick_action", pure_virtual(&Trader::newTickAction))
		.def("new_candels_action", pure_virtual(&Trader::newCandelsAction))

		.def("change_order", &Trader::changeOrder, args("oredr"))
		.def("make_order", &Trader::makeOrder, args("oredr"))
		.def("get_portfio", &Trader::getPortfio, return_value_policy<copy_const_reference>())

		.def("set_market", &Trader::setMarket, args("market"))
		.def("create_limit_order", &Trader::createLimitOrder, args("ticker", "volume", "price"))
		.def_readonly("orders", &Trader::orders) 
		;

}

