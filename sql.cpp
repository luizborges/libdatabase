
#include "database.hpp"

////////////////////////////////////////////////////////////////////////////////
// public functions - constructor
////////////////////////////////////////////////////////////////////////////////
d::sql::sql(const std::vector<std::string>& statement)
{ try {
	if(statement.empty()) throw err("sql_statement cannot be an empty string");
	this->_kind = kind::CONCAT;
	this->statement = statement;
 } catch (const std::exception &e) { throw err(e.what()); }
}

d::sql::sql(const kind& _kind, const std::vector<std::string>& statement)
{ try {
	if(statement.empty()) throw err("sql_statement cannot be an empty string");
	this->_kind = _kind;
	this->statement = statement;
 } catch (const std::exception &e) { throw err(e.what()); }
}
///////////////////////////////////////////////////////////////////////////////
// public functions -
////////////////////////////////////////////////////////////////////////////////
void d::sql::print()
{ try {
	std::fprintf(stderr, "PRINT SQL\nsql kind: d::kind::CONCAT - CONCATENATION\n");
	std::fprintf(stderr, "Below sql statement - in format: [índice][type]: \"content\" - "
	"índice is the position in array (init from 1) - type: text: sql that is pure text | "
	"key: key of field of object - will chage in parser - content: is the value of array.\n");
	bool text = true;
	for(size_t i=0; i < statement.size(); ++i)
	{
		std::fprintf(stderr, "[%ld][%s]: \"%s\"\n", i+1, 
			text ? "text" : "key", statement[i].c_str());
		text = !text;
	}
 } catch (const std::exception &e) { throw err(e.what()); }
}


///////////////////////////////////////////////////////////////////////////////
// public functions - run functions
////////////////////////////////////////////////////////////////////////////////
// template functions - end of database.hpp

////////////////////////////////////////////////////////////////////////////////
// private functions - auxiliar functions
////////////////////////////////////////////////////////////////////////////////
void
print_keys(const std::vector<std::map<std::string, field>>& V)
{ try {
	std::fprintf(stderr, "Show all maps below. [index in vector+1]: { \"key1\", ..., \"key n\", }");
   	int i = 0;
   	for(auto const& M : V)
   	{
   		std::fprintf("[%d]: { ", ++i);
   		for(auto const& e : M) std::fprintf(stderr, "\"%s\", ", e.first);
   		std::fprintf(" }\n");
   	}
 } catch (const std::exception &e) { throw err(e.what()); }
}


std::string
d::sql::make_query(const std::map<std::string, field>& M)
{ try {
	std::string query = "";
	switch(_kind)
	{
		case kind::FORMAT: query = make_query_format(M); break;
		case kind::CONCAT: query = make_query_concat(M); break;
		default: throw err("no sql kind found. kind: %d", static_cast<int>(_kind));
	}
	
	return query + ";";
 } catch (const std::exception &e) { throw err(e.what()); }
}

std::string
d::sql::make_query(const std::vector<std::map<std::string, field>>& V)
{ try {
	std::string query = "";
	switch(_kind)
	{
		case kind::FORMAT: query = make_query_format(V); break;
		case kind::CONCAT: query = make_query_concat(V); break;
		default: throw err("no sql kind found. kind: %d", static_cast<int>(_kind));
	}
	
	return query + ";";
 } catch (const std::exception &e) { throw err(e.what()); }
}

void
d::sql::copy_result(const pqxx::row& Row, std::map<std::string, field>& M)
{ try {
	for(const auto& R : Row)
	{
		auto F = M.find(R.name());
		if(F == M.end()) { // verifica se existe a chave em dest
   			throw err("column result name does not exists in object key. "
   				"column name: \"%s\" | column value: \"%s\"", R.name(), 
   				R.is_null() ? "" : R.c_str());
   		}
   		
		F->second.set() = R.is_null() ? "" : R.as<std::string>();
		F->second.check_read(); // trata a entrarda
	}
 } catch (const std::exception &e) { throw err(e.what()); }
}

////////////////////////////////////////////////////////////////////////////////
// KIND::concat - concatenation - functions
////////////////////////////////////////////////////////////////////////////////
std::string
d::sql::make_query_concat(const std::map<std::string, field>& M)
{ try {
    std::string query = "";
    bool Field_Key = false;
    for(auto const& S : statement)
    {
    	if(Field_Key == true)
    	{
    		auto const F = M.find(S);
    		if(F == M.end()) {
    			std::fprintf(stderr, "\nERROR\n");
    			print();
    			throw err("not field key of statement in object field key - "
    			"field key: \"%s\"", S);}
    		const_cast<field&>(F->second).check_write();
    		query += F->second.str();
    	} 
    	else query += S;
    	Field_Key = !Field_Key; // necessário para alteranr entre a string e as chaves
    }
    return query;
 } catch (const std::exception &e) { throw err(e.what()); }
}

std::string
d::sql::make_query_concat(const std::vector<std::map<std::string, field>>& V)
{ try {
    std::string query = "";
    bool Field_Key = false;
    for(auto const& S : statement)
    {
    	if(Field_Key == true)
    	{
    		auto const F = M.find(S);
    		if(F == M.end()) {
    			std::fprintf(stderr, "\nERROR\n");
    			print();
    			throw err("not field key of statement in object field key - "
    			"field key: \"%s\"", S);}
    		const_cast<field&>(F->second).check_write();
    		query += F->second.str();
    	} 
    	else query += S;
    	Field_Key = !Field_Key; // necessário para alteranr entre a string e as chaves
    }
    return query;
 } catch (const std::exception &e) { throw err(e.what()); }
}

field&
d::sql::get_field(const std::string& key,
	const std::vector<std::map<std::string, field>>& V, const int idx = 0)
{ try {
    if(idx >= V.size()) // a chave não existe no vetor de maps 
    {
    	print_keys(V);
    	throw err("ERROR - not found key in vector of maps of fields. key: \"%s\"", key.c_str());
    }

    const auto e = V[idx].find(key);
    if(e == V[idx].end()) return get_field(key, V, idx+1); // não encontrou o elemento - continua a busca
    return e; // encontrou o elemento
 } catch (const std::exception &e) { throw err(e.what()); }
}
////////////////////////////////////////////////////////////////////////////////
// KIND::format 
////////////////////////////////////////////////////////////////////////////////
std::string
d::sql::make_query_format(const std::map<std::string, field>& M)
{ try {
    throw err("not implemented yet");
 } catch (const std::exception &e) { throw err(e.what()); }
}

/**
 old codes
 */

























