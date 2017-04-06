#include <boost/python.hpp>

#include "../HeaderFiles/Backtester.h"

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
	void newCandlesAction(string ticker, vector<Candle> candles) override {
		this->get_override("new_candles_action")(ticker, candles);
	}
	void newTickAction(vector<OrderChange> matcher,
					   unordered_map<string, AnonimousMaketDepthVector> depths) override {
		this->get_override("new_tick_action")(matcher, depths);
	}
};

BOOST_PYTHON_MODULE(backtester) {
	//to convert from python list of strings to c++ vector wher receiving tickers
	custom_vector_from_seq<string> conv;

	//changes received by trader
	to_python_converter <vector<OrderChange>, stdvector_to_python<OrderChange>>();

	//candles received by trader
	to_python_converter <vector<Candle>, stdvector_to_python<Candle>>();

	//all history data
	to_python_converter <CandlesVectorMap, unordered_map_to_python<string, vector<Candle>>>();

	//orders in trader of exact stock 
	to_python_converter <unordered_map<int, int>,
		unordered_map_to_python<int, int>>();

	//one depth received by trader
	to_python_converter <AnonimousMaketDepthVector, stdvector_to_python<AnonimousMarketDepthItem>>();

	//all depths received
	to_python_converter <unordered_map<string, AnonimousMaketDepthVector>,
		unordered_map_to_python<string, AnonimousMaketDepthVector>>();

	//orders in trader of all stocks
	to_python_converter <unordered_map<string, unordered_map<int, int>>, 
		unordered_map_to_python<string, unordered_map<int, int>>>();
	
	//portfolio
	to_python_converter< unordered_map<string, int>,
		unordered_map_to_python<string, int>>();

	//performance_vector
	to_python_converter<vector<float>,
		stdvector_to_python<float>>();
	
	//performance
	to_python_converter< unordered_map<string, vector<float>>,
		unordered_map_to_python<string, vector<float>>>();

	//recieved changes
	enum_<ChangeReason>("ChangeReason")
		.value("Placed", ChangeReason::Placed)
		.value("Matched", ChangeReason::Matched)
		.value("Cancelled", ChangeReason::Cancelled)
		.value("Unknown", ChangeReason::Unknown);

	class_<Candle>("Candle")
		.def_readwrite("open", &Candle::open)
		.def_readwrite("high", &Candle::high)
		.def_readwrite("low", &Candle::low)
		.def_readwrite("close", &Candle::close)
		.def_readwrite("volume", &Candle::volume)
		.add_property("datetime_index", &Candle::datetime_index)
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
		.def_readwrite("cause", &OrderChange::reason)
		.def_readwrite("ticker", &OrderChange::ticker)
		.def_readwrite("trader_identifier", &OrderChange::traderIdentifier)
		.def_readwrite("volume_change", &OrderChange::volumeChange)
		.def(self == self)
		.def(self_ns::str(self_ns::self))
		.def(self_ns::repr(self_ns::self));

	class_<Order>("Order")
		.def_readwrite("volume", &Order::volume)
		.def_readwrite("price", &Order::price)
		.def_readwrite("orderType", &Order::orderType)

		.def_readwrite("trader_identifier", &Order::traderIdentifier)

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
		.def_readonly("number_of_tics", &SingleTraderMarket::numberOfTics)
		.def("get_internal_history_candles", &SingleTraderMarket::getInternalHistoryCandles,
					   return_value_policy<copy_const_reference>());

	class_<TraderWrap, boost::noncopyable>("Trader", init<string>(args("identifier")))
		.def("tick_action", pure_virtual(&Trader::newTickAction))
		.def("new_candles_action", pure_virtual(&Trader::newCandlesAction))

		.def("change_order", &Trader::changeOrder, args("oredr"))
		.def("make_order", &Trader::makeOrder, args("oredr"))
		.def("get_portfolio", &Trader::getPortfio, return_value_policy<copy_const_reference>())
		.def("get_performance", &Trader::getPerformance, return_value_policy<copy_const_reference>())
		.def("set_market", &Trader::setMarket, args("market"))
		.def("create_limit_order", &Trader::createLimitOrder, args("ticker", "volume", "price"))
		.def("create_market_order", &Trader::createMarketOrder, args("ticker", "volume"))
		.def("create_IoC_order", &Trader::createIoCOrder, args("ticker", "volume", "price"))
		.def("request_candles", &Trader::requestCandles, args("tickrt", "length"))
		.add_property("orders", make_getter(&Trader::orders, return_value_policy<return_by_value>()));

}

