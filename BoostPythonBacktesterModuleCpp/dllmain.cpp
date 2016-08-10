// dllmain.cpp : Defines the entry point for the DLL application.

#define DLL_COMPILE

#include "stdafx.h"
#include "Backtester.h"
#include <boost/python.hpp>


using namespace boost::python;
using namespace std;

template<class T>
struct stdlist_to_python {
	static PyObject *convert(const std::list<Order> &ls) {
		boost::python::list py_list;
		for (auto iter = ls.begin(); iter != ls.end(); ++iter) {
			py_list.append(*iter);
		}
		return incref(py_list.ptr());
	}
};

struct MarketDepthData_to_python {
	static PyObject *convert(const MarketDepthData &map) {
		dict dictionary;
		for (auto iter = map.begin(); iter != map.end(); ++iter) {
			dictionary[iter->first] = iter->second;
		}
		return incref(dictionary.ptr());
	}
};

BOOST_PYTHON_MODULE(backtester) {
	to_python_converter <MarketDepthData, MarketDepthData_to_python>();
	to_python_converter <std::list<Order>, stdlist_to_python<Order>>();

	class_<MarketDepth>("MarketDepth")
		.def(init<string>(args("ticker")))
		.def("add_order", &MarketDepth::addOrder)
		.def("get_internal_depth", &MarketDepth::getInternalDepth, return_value_policy<copy_const_reference>());
	

	class_<Order>("Order")
		.def_readwrite("volume", &Order::volume)
		.def_readwrite("price", &Order::price)
		.def_readwrite("orderType", &Order::orderType)
		
		.def_readwrite("trader_identifier",	  &Order::trader_identifier)

		.def_readwrite("ticker", &Order::ticker)

		.def("make_limit_order", &Order::Make_Limit_Order,
			 args("trader_identifier", "ticker", "volume", "price"))
		.def(self_ns::str(self_ns::self))
		.def(self_ns::repr(self_ns::self))
		.staticmethod("make_limit_order")
		; 


}


/*
BOOL APIENTRY DllMain( HMODULE hModule,
DWORD  ul_reason_for_call,
LPVOID lpReserved
)
{
switch (ul_reason_for_call)
{
case DLL_PROCESS_ATTACH:
case DLL_THREAD_ATTACH:
case DLL_THREAD_DETACH:
case DLL_PROCESS_DETACH:
break;
}
return TRUE;
}
*/
