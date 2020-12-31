#include "database.hpp"


////////////////////////////////////////////////////////////////////////////////
// Constructors
////////////////////////////////////////////////////////////////////////////////
/*d::field_query_result::field_query_result(const std::string& column_name) {
	field_query_result(column_name, field_type::STR);
}*/

d::field_query_result::field_query_result(
	const std::string& column_name, const field_type& type)
{try{
	if(column_name.empty()) throw err("DATABASE: column name cannot be an empty string");
	if(type == field_type::NO_TYPE) throw err("DATABASE: type cannot be NOT_TYPE");
	
	this->column_name = column_name;
	_etype = type;
 } catch (const std::exception &e) { throw err(e.what()); }
}

d::field_query_result::field_query_result(
	const std::string& column_name, const std::string& type)
{try{
	if(column_name.empty()) throw err("DATABASE: column name cannot be an empty string");
	if(type.empty()) throw err("DATABASE: type cannot be an empty string");
	if(type == "NO_TYPE") throw err("DATABASE: type cannot be NOT_TYPE");
	
	this->column_name = column_name;
	
	if(type == "string") _etype = field_type::STR;
	else if(type == "char") _etype = field_type::CHAR;
	else if(type == "int") _etype = field_type::INT;
	else if(type == "long") _etype = field_type::LONG;
	else if(type == "long long") _etype = field_type::LONG_LONG;
	else if(type == "unsigned char") _etype = field_type::UCHAR;
	else if(type == "unsigned int") _etype = field_type::UINT;
	else if(type == "unsigned long") _etype = field_type::ULONG;
	else if(type == "unsigned long long") _etype = field_type::ULONG_LONG;
	else if(type == "float") _etype = field_type::FLOAT;
	else if(type == "double") _etype = field_type::DOUBLE;
	else if(type == "long double") _etype = field_type::LONG_DOUBLE;
	else throw err("no type in list: \"%s\"\n", type.c_str());
	
 } catch (const std::exception &e) { throw err(e.what()); }
}

d::field_query_result::field_query_result(
	const std::string& column_name, const int type)
{try{
	if(column_name.empty()) throw err("DATABASE: column name cannot be an empty string");
	if(type == 0) throw err("DATABASE: type cannot be NOT_TYPE");
	
	this->column_name = column_name;
	_etype = static_cast<field_type>(type);
	
 } catch (const std::exception &e) { throw err(e.what()); }
}

std::string
d::field_query_result::type() const
{ try {
	switch(etype())
	{
		case field_type::NO_TYPE: return "NO TYPE";
		case field_type::STR:  return "string";
		case field_type::CHAR: return "char";
		case field_type::INT:  return "int";
		case field_type::LONG: return "long";
		case field_type::LONG_LONG: return "long long";
		case field_type::UCHAR: return "unsigned char";
		case field_type::UINT:  return "unsigned int";
		case field_type::ULONG: return "unsigned long";
		case field_type::ULONG_LONG: return "unsigned long long";
		case field_type::FLOAT:  return "float";
		case field_type::DOUBLE: return "double";
		case field_type::LONG_DOUBLE: return "long double";
		default: throw err("no index list: %d\n", index());
	}
 } catch (const std::exception &e) { throw err(e.what()); }
}

