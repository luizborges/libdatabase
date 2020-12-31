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
	this->etype = type;
	this->index = static_cast<int>(type);
	
	switch(index)
	{
		//case  0: stype = "NO TYPE";
		case  1: stype =  "string"; break;
		case  2: stype =  "char"; break;
		case  3: stype =  "int"; break;
		case  4: stype =  "long"; break;
		case  5: stype =  "long long"; break;
		case  6: stype =  "unsigned char"; break;
		case  7: stype =  "unsigned int"; break;
		case  8: stype =  "unsigned long"; break;
		case  9: stype =  "unsigned long long"; break;
		case 10: stype =  "float"; break;
		case 11: stype =  "double"; break;
		default: throw err("no index list: %d\n", index);
	}
 } catch (const std::exception &e) { throw err(e.what()); }
}

d::field_query_result::field_query_result(
	const std::string& column_name, const std::string& type)
{try{
	if(column_name.empty()) throw err("DATABASE: column name cannot be an empty string");
	if(type.empty()) throw err("DATABASE: type cannot be an empty string");
	if(type == "NO_TYPE") throw err("DATABASE: type cannot be NOT_TYPE");
	
	this->column_name = column_name;
	stype = type;
	
	if(type == "string") etype = field_type::STR;
	else if(type == "char") etype = field_type::CHAR;
	else if(type == "int") etype = field_type::INT;
	else if(type == "long") etype = field_type::LONG;
	else if(type == "long long") etype = field_type::LONG_LONG;
	else if(type == "unsigned char") etype = field_type::UCHAR;
	else if(type == "unsigned int") etype = field_type::UINT;
	else if(type == "unsigned long") etype = field_type::ULONG;
	else if(type == "unsigned long long") etype = field_type::ULONG_LONG;
	else if(type == "float") etype = field_type::FLOAT;
	else if(type == "double") etype = field_type::DOUBLE;
	else throw err("no type in list: \"%s\"\n", type.c_str());
	
	index = static_cast<int>(etype);
 } catch (const std::exception &e) { throw err(e.what()); }
}

d::field_query_result::field_query_result(
	const std::string& column_name, const int type)
{try{
	if(column_name.empty()) throw err("DATABASE: column name cannot be an empty string");
	if(type == 0) throw err("DATABASE: type cannot be NOT_TYPE");
	
	this->column_name = column_name;
	this->etype = static_cast<field_type>(type);
	this->index = index;
	
	switch(index)
	{
		//case  0: stype = "NO TYPE";
		case  1: stype =  "string"; break;
		case  2: stype =  "char"; break;
		case  3: stype =  "int"; break;
		case  4: stype =  "long"; break;
		case  5: stype =  "long long"; break;
		case  6: stype =  "unsigned char"; break;
		case  7: stype =  "unsigned int"; break;
		case  8: stype =  "unsigned long"; break;
		case  9: stype =  "unsigned long long"; break;
		case 10: stype =  "float"; break;
		case 11: stype =  "double"; break;
		default: throw err("no index list: %d\n", index);
	}
 } catch (const std::exception &e) { throw err(e.what()); }
}


