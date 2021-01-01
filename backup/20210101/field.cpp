#include "database.hpp"

////////////////////////////////////////////////////////////////////////////////
// public functions - constructor
////////////////////////////////////////////////////////////////////////////////
std::string
d::field::type() const
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

std::string
d::field::str() const
{ try {
	switch(etype())
	{
		case field_type::NO_TYPE: return "";
		case field_type::STR:  return std::get<std::string>(val);
		case field_type::CHAR: return std::to_string(std::get<char>(val));
		case field_type::INT:  return std::to_string(std::get<int>(val));
		case field_type::LONG: return std::to_string(std::get<long>(val));
		case field_type::LONG_LONG: return std::to_string(std::get<long long>(val));
		case field_type::UCHAR: return std::to_string(std::get<unsigned char>(val));
		case field_type::UINT:  return std::to_string(std::get<unsigned int>(val));
		case field_type::ULONG: return std::to_string(std::get<unsigned long>(val));
		case field_type::ULONG_LONG: return std::to_string(std::get<unsigned long long>(val));
		case field_type::FLOAT:  return std::to_string(std::get<float>(val));
		case field_type::DOUBLE: return std::to_string(std::get<double>(val));
		case field_type::LONG_DOUBLE: return std::to_string(std::get<long double>(val));
		default: throw  err("no index list: %d\n", index());
	}
 } catch (const std::exception &e) { throw err(e.what()); }	
}

void
d::field::check_write()
{ try {
	// TODO -
 } catch (const std::exception &e) { throw err(e.what()); }
}

void
d::field::check_read()
{ try {
	// TODO - 
 } catch (const std::exception &e) { throw err(e.what()); }
}

void
d::field::str_to(const field_type& type)
{ try {
	if(type == field_type::NO_TYPE) throw err("DATABASE: impossible convert string to NO_TYPE");
	if(type == field_type::STR) return; // se os tipos já são iguais, não realiza operação alguma
	if(etype() != field_type::STR) throw err("DATABASE: etype() = \"%s\"", this->type().c_str());
	
	switch(type)
	{
		//case  field_type::NO_TYPE: stype = "NO TYPE";
		//case  field_type::STR: stype =  "string"; break;
		case field_type::CHAR: val = std::get<std::string>(val)[0]; break;
		case field_type::INT:  val = std::stoi(std::get<std::string>(val)); break;
		case field_type::LONG: val = std::stol(std::get<std::string>(val)); break;
		case field_type::LONG_LONG: val = std::stoll(std::get<std::string>(val)); break;
		case field_type::UCHAR: throw err("not implemented yet - unsigned char"); break;
		case field_type::UINT:  throw err("not implemented yet - unsigned int"); break;
		case field_type::ULONG: val = std::stoul(std::get<std::string>(val)); break;
		case field_type::ULONG_LONG: val = std::stoull(std::get<std::string>(val)); break;
		case field_type::FLOAT:  val = std::stof(std::get<std::string>(val)); break;
		case field_type::DOUBLE: val = std::stod(std::get<std::string>(val)); break;
		case field_type::LONG_DOUBLE: val = std::stold(std::get<std::string>(val)); break;
		default: throw err("no index list: %d\n", static_cast<int>(type));
	}
 } catch (const std::exception &e) { throw err(e.what()); }
}

void
d::field::str_to(const std::string& type)
{ try {
	if(type.empty()) throw err("DATABASE: type cannot be an empty string");
	
	if(type == "string") str_to(field_type::STR);
	else if(type == "char") str_to(field_type::CHAR);
	else if(type == "int") str_to(field_type::INT);
	else if(type == "long") str_to(field_type::LONG);
	else if(type == "long long") str_to(field_type::LONG_LONG);
	else if(type == "unsigned char") str_to(field_type::UCHAR);
	else if(type == "unsigned int") str_to(field_type::UINT);
	else if(type == "unsigned long") str_to(field_type::ULONG);
	else if(type == "unsigned long long") str_to(field_type::ULONG_LONG);
	else if(type == "float") str_to(field_type::FLOAT);
	else if(type == "double") str_to(field_type::DOUBLE);
	else if(type == "long double") str_to(field_type::LONG_DOUBLE);
	else throw err("no type in list: \"%s\"\n", type.c_str());
 } catch (const std::exception &e) { throw err(e.what()); }
}

////////////////////////////////////////////////////////////////////////////////
// private functions - auxiliar
////////////////////////////////////////////////////////////////////////////////


























