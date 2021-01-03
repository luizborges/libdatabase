#include "database.hpp"


////////////////////////////////////////////////////////////////////////////////
// public functions - constructor
////////////////////////////////////////////////////////////////////////////////
d::table::table(const obj& Obj)
{ try {
	_obj.push_back(Obj);
 } catch (const std::exception &e) { throw err(e.what()); }
}



////////////////////////////////////////////////////////////////////////////////
// public functions
////////////////////////////////////////////////////////////////////////////////
void 
d::table::init(const obj& Obj)
{ try {
	_obj.clear();
	_obj.push_back(Obj);
 } catch (const std::exception &e) { throw err(e.what()); }
}

d::obj&
d::table::move(const obj& Obj, const size_t idx)
{ try {
	if(idx >= _obj.size())
		throw err("No index in table. idx: %lu | table size: %lu", idx, _obj.size());
	
	_obj[idx] = std::move(Obj);
	return _obj[idx];
 } catch (const std::exception &e) { throw err(e.what()); }
}

////////////////////////////////////////////////////////////////////////////////
// public functions - overloading operators
////////////////////////////////////////////////////////////////////////////////
d::obj& 
d::table::operator[] (const size_t idx)
{ try {
	if(idx >= _obj.size())
		throw err("No index in table. idx: %lu | table size: %lu", idx, _obj.size());
	
	return _obj[idx];
 } catch (const std::exception &e) { throw err(e.what()); }
}

////////////////////////////////////////////////////////////////////////////////
// public functions - overloading operators
////////////////////////////////////////////////////////////////////////////////




















