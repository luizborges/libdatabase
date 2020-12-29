#include "database.hpp"
////////////////////////////////////////////////////////////////////////////////
// public functions - constructor
////////////////////////////////////////////////////////////////////////////////

std::string
d::field::type()
{
	switch(val.index())
	{
		case 0:  return "NO TYPE";
		case 1:  return "std::string";
		case 2:  return "bool";
		case 3:  return "char";
		case 4:  return "int";
		case 5:  return "long";
		case 6:  return "long long";
		case 7:  return "unsigned char";
		case 8:  return "unsigned int";
		case 9:  return "unsigned long";
		case 10: return "unsigned long long";
		default: throw err("no index list: %d\n", val.index());
	}
}

auto
d::field::get()
{
	switch(val.index())
	{
		case 0:  return nullptr;
		case 1:  return std::get<std::string>(val);
		case 2:  return std::get<bool>(val);
		case 3:  return std::get<char>(val);
		case 4:  return std::get<int>(val);
		case 5:  return std::get<long>(val);
		case 6:  return std::get<long long>(val);
		case 7:  return std::get<unsigned char>(val);
		case 8:  return std::get<unsigned int>(val);
		case 9:  return std::get<unsigned long>(val);
		case 10: return std::get<unsigned long long>(val);
		default: throw err("no index list: %d\n", val.index());
	}
}

std::string
d::filed::key_str()
{
	switch(val.index())
	{
		case 0:  return "no key";
		case 1:  return "primary key";
		case 2:  return "foreign key";
		default: throw err("no index list: %d\n", val.key());
	}
}





























