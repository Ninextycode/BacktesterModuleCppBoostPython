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

template<class containedType>
struct custom_vector_from_seq {
	custom_vector_from_seq() { converter::registry::push_back(&convertible, &construct, type_id<vector<containedType> >()); }
	static void* convertible(PyObject* obj_ptr) {
		if (!PySequence_Check(obj_ptr) || !PyObject_HasAttrString(obj_ptr, "__len__")) return 0;
		return obj_ptr;
	}
	static void construct(PyObject* obj_ptr, converter::rvalue_from_python_stage1_data* data) {
		void* storage = ((converter::rvalue_from_python_storage<vector<containedType> >*)(data))->storage.bytes;
		new (storage) vector<containedType>();
		vector<containedType>* v = (vector<containedType>*)(storage);
		int l = PySequence_Size(obj_ptr); 
		if (l<0) 
			abort();
		v->reserve(l);
		for (int i = 0; i<l; i++) {
			v->push_back(extract<containedType>(PySequence_GetItem(obj_ptr, i))); 
		}
		data->convertible = storage;
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
	//to convert from python list of strings to c++ vector wher receiving tickers
	custom_vector_from_seq<string> conv;

	//changes received by trader
	to_python_converter <vector<OrderChange>, stdvector_to_python<OrderChange>>();

	//candels received by trader
	to_python_converter <vector<Candel>, stdvector_to_python<Candel>>();

	//all history data
	to_python_converter <CandesVectorMap, unordered_map_to_python<string, vector<Candel>>>();

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

	class_<Candel>("Candel")
		.def_readwrite("open", &Candel::open)
		.def_readwrite("high", &Candel::high)
		.def_readwrite("low", &Candel::low)
		.def_readwrite("close", &Candel::close)
		.def_readwrite("volume", &Candel::volume)
		.add_property("datetime", &Candel::getdatetime)
		.def(self == self)
		.def(self_ns::str(self_ns::self))
		.def(self_ns::repr(self_ns::self));

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


	class_<SingleTraderMarket>("Market")
		.def("set_trader", &SingleTraderMarket::setTrader)
		.def("load_history_data", &SingleTraderMarket::loadHistoryData)
		.def("run_full_test", &SingleTraderMarket::runFullTest)
		.def("request_candles", &SingleTraderMarket::requestCandles)
		.def("set_comission", &SingleTraderMarket::setComission)
		.def_readwrite("DEPTH_LENGTH_PUBLIC", &SingleTraderMarket::DEPTH_LENGTH_PUBLIC)
		.def_readonly("DEPTH_LENGTH_PUBLIC", &SingleTraderMarket::DEPTH_LENGTH_PUBLIC)
		.def("get_internal_history_candles", &SingleTraderMarket::getInternalHistoryCandles,
					   return_value_policy<copy_const_reference>());

	class_<TraderWrap, boost::noncopyable>("Trader", init<string>(args("identifier")))		
		.def("tick_action", pure_virtual(&Trader::newTickAction))
		.def("new_candels_action", pure_virtual(&Trader::newCandelsAction))

		.def("change_order", &Trader::changeOrder, args("oredr"))
		.def("make_order", &Trader::makeOrder, args("oredr"))
		.def("get_portfolio", &Trader::getPortfio, return_value_policy<copy_const_reference>())

		.def("set_market", &Trader::setMarket, args("market"))
		.def("create_limit_order", &Trader::createLimitOrder, args("ticker", "volume", "price"))
		.def("create_market_order", &Trader::createMarketOrder, args("ticker", "volume"))
		.def("request_candles", &Trader::requestCandels, args("tickrt", "length"))
		.add_property("orders", make_getter(&Trader::orders, return_value_policy<return_by_value>()));

}

