
#include "database.hpp"

////////////////////////////////////////////////////////////////////////////////
// public functions - constructor
////////////////////////////////////////////////////////////////////////////////
d::sql_arg::sql_arg(const std::string& field_key)
{ try {
	if(field_key.empty()) throw err("field key cannot be an empty string in this constructor.");
	key()  = field_key;
	type() = sql_earg::field_value;
 } catch (const std::exception &e) { throw err(e.what()); }
}


d::sql_arg::sql_arg(
	const std::string& field_key, const std::string& field_name)
{ try {
	if(field_key.empty()) throw err("field key cannot be an empty string in this constructor.");
	if(field_name.empty()) throw err("field name cannot be an empty string in this constructor.");
	key()  = field_key;
	name() = field_name;
	type() = sql_earg::field_name;	
 } catch (const std::exception &e) { throw err(e.what()); }
}
////////////////////////////////////////////////////////////////////////////////
// public functions - auxiliar functions
////////////////////////////////////////////////////////////////////////////////



