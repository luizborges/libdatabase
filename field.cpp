#include "database.hpp"

////////////////////////////////////////////////////////////////////////////////
// public functions - constructor
////////////////////////////////////////////////////////////////////////////////
std::string
d::field::type() const
{ try {
	switch(val.index())
	{
		case  0: return "NO TYPE";
		case  1: return "string";
		case  2: return "char";
		case  3: return "int";
		case  4: return "long";
		case  5: return "long long";
		case  6: return "unsigned char";
		case  7: return "unsigned int";
		case  8: return "unsigned long";
		case  9: return "unsigned long long";
		case 10: return "float";
		case 11: return "double";
		default: throw err("no index list: %d\n", val.index());
	}
 } catch (const std::exception &e) { throw err(e.what()); }
}

std::string
d::field::key_str() const
{ try {
	switch(key) 
	{
		case field_key::no:       return "no key";
		case field_key::primary:  return "primary key";
		case field_key::foreign:  return "foreign key";
		default: throw err("no index list: %d\n", key);
	}
 } catch (const std::exception &e) { throw err(e.what()); }
}

std::string
d::field::str() const
{ try {
	switch(val.index())
	{
		case 0:  return "";
		case 1:  return std::get<std::string>(val);
		case 2:  return std::to_string(std::get<char>(val));
		case 3:  return std::to_string(std::get<int>(val));
		case 4:  return std::to_string(std::get<long>(val));
		case 5:  return std::to_string(std::get<long long>(val));
		case 6:  return std::to_string(std::get<unsigned char>(val));
		case 7:  return std::to_string(std::get<unsigned int>(val));
		case 8:  return std::to_string(std::get<unsigned long>(val));
		case 9:  return std::to_string(std::get<unsigned long long>(val));
		case 10: return std::to_string(std::get<float>(val));
		case 11: return std::to_string(std::get<double>(val));
		default: throw  err("no index list: %d\n", val.index());
	}
 } catch (const std::exception &e) { throw err(e.what()); }	
}

void
d::field::check() const
{ try {
	if(etype() == field_type::NO_TYPE) // check errors
   	{
   		if(key == field_key::primary) throw err("Primary Key is NO_TYPE");
   		if(notNull == true) throw err("NotNull value is NO_TYPE (NULL)");
   	}  	
   	else if(etype() == field_type::STR)
   	{
   		if(get().empty() == true)
   		{
   			if(key == field_key::primary) throw err("Primary Key is an empty string");
	   		if(notNull == true) throw err("NotNull value is empty string");
   		}
   	}
 } catch (const std::exception &e) { throw err(e.what()); }
}

void
d::field::str_to(const field_type& type)
{ try {
	if(type == field_type::NO_TYPE) throw err("DATABASE: impossible convert string to NO_TYPE");
	if(type == field_type::STR) return; // se os tipos já são iguais, não realiza operação alguma
	if(etype() != field_type::STR) throw err("DATABASE: etype() = \"%s\"", this->type().c_str());
	
	switch(static_cast<int>(type))
	{
		//case  0: stype = "NO TYPE";
		//case  1: stype =  "string"; break;
		case  2: val = std::get<std::string>(val)[0]; break;
		case  3: val = std::stoi(std::get<std::string>(val)); break;
		case  4: throw err("not implemented yet - long"); break;
		case  5: throw err("not implemented yet - long long"); break;
		case  6: throw err("not implemented yet - unsigned char"); break;
		case  7: throw err("not implemented yet - unsigned int"); break;
		case  8: throw err("not implemented yet - unsigned long"); break;
		case  9: throw err("not implemented yet - unsigned long long"); break;
		case 10: throw err("not implemented yet - float"); break;
		case 11: val = std::stof(std::get<std::string>(val)); break;
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
	else throw err("no type in list: \"%s\"\n", type.c_str());
 } catch (const std::exception &e) { throw err(e.what()); }
}

////////////////////////////////////////////////////////////////////////////////
// private functions - auxiliar
////////////////////////////////////////////////////////////////////////////////


























