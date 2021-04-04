
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
	std::string msg = "PRINT SQL\nsql kind: d::kind::CONCAT - CONCATENATION\n";
	msg += "Below sql statement - in format: [índice][type]: \"content\" - "
	"índice is the position in array (init from 1) - type: text: sql that is pure text | "
	"key: key of field of object - will chage in parser - content: is the value of array.\n";
	bool text = true;
	for(size_t i=0; i < statement.size(); ++i)
	{
		msg += u::format("[%ld][%s]: \"%s\"\n", i+1, 
			text ? "text" : "key", statement[i].c_str());
		text = !text;
	}
	u::error::set_header(false); err(msg.c_str()); u::error::set_header(true);
 } catch (const std::exception &e) { throw err(e.what()); }
}

void
d::sql::print_key(const std::vector<obj>& V)
{ try {
	// desabilita o cabeçalho - para não exibir que é mensagem de erro.
	// ideal utilizar esta função pois além de exibir em stderr pode também exibir em outra saída se desejar (tela)
	std::string msg = "";
	msg += "Show all keys and names below. [index in vector+1]: {\"key1\" (\"name1\", ..., \"nameN\"), ..., \"keyN\" (...)}\n";
   	int i = 0;
   	for(auto const& M : V)
   	{
   		msg += u::format("[%d]: {", ++i);
   		for(auto const& e : M)
   		{
   			msg += u::format("\"%s\" (", e.first.c_str()); // imprime a chave do map
   			auto const& Name = const_cast<field&>(e.second).name();
   			for(auto const& n : Name) msg += u::format("\"%s\", ", n.c_str()); // imprime o nome do field
   			if(Name.empty() == false) { msg.pop_back(); msg.pop_back(); }
   			msg += "), ";
   		}
   		if(M.empty() == false) { msg.pop_back(); msg.pop_back(); }
   		msg += "}\n";
   	}
   	msg.pop_back(); // get rid the last character
   	u::error::set_header(false); err(msg.c_str()); u::error::set_header(true);
 } catch (const std::exception &e) { throw err(e.what()); }
}

std::string
d::sql::make_query(const std::vector<obj>& V)
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
d::sql::copy_result(const pqxx::row& Row, std::vector<obj>& V)
{ try {
	for(const auto& R : Row)
	{ try {
		auto& F = get_field(R.name(), V);
		F.set() = R.is_null() ? "" : R.as<std::string>();
		F.check_read();
	  } catch (const std::exception &e) {
    		throw err("column name: \"%s\" | column value: \"%s\"", R.name(), 
   				R.is_null() ? "" : R.c_str()); }
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
d::field&
d::sql::get_field(const std::string& key,
	const std::vector<obj>& V, const size_t idx)
{ try {
    if(idx >= V.size()) // a chave não existe no maps como chave 
    	return get_field_by_name(key, V);

    const auto e = V[idx].find(key); // busca a chave no map[idx] - chave do map
    if(e == V[idx].end()) {
    	return get_field(key, V, idx+1); } // não encontrou o elemento - continua a busca
   
	return const_cast<field&>(e->second); //  encontrou o elemento
 } catch (const std::exception &e) { throw err(e.what()); }
}

d::field&
d::sql::get_field_by_name(const std::string& key,
	const std::vector<obj>& V, const size_t idx)
{
	if(idx >= V.size()) // a chave não existe no vetor de maps 
    {
    	print_key(V);
    	throw err("ERROR - not found key in vector of maps of fields. key: \"%s\"", key.c_str());
    }

    for(auto const& F : V[idx])
    {
    	auto const name = const_cast<field&>(F.second).name().find(key);
    	if(name != const_cast<field&>(F.second).name().end()) // verifica se encontrou a chave
    		return const_cast<field&>(F.second);
	}
	
    return get_field_by_name(key, V, idx+1); // não encontrou o elemento - continua a busca
}
////////////////////////////////////////////////////////////////////////////////
// KIND::concat - concatenation - functions
////////////////////////////////////////////////////////////////////////////////
std::string
d::sql::make_query_concat(const std::vector<obj>& V)
{ try {
    std::string query = "";
    bool Field_Key = false;
    for(auto const& S : statement)
    {
    	if(Field_Key == true)
    	{
    		auto const& F = get_field(S, V);
    		try { const_cast<field&>(F).check_write(); }
    		catch (const std::exception &e) { 
    			print(); 
    			throw err("Key Statement: \"%s\"", S.c_str()); }
    		
    		query += F.str();
    	} 
    	else query += S;
    	Field_Key = !Field_Key; // necessário para alteranr entre a string e as chaves
    }
    return query;
 } catch (const std::exception &e) { throw err(e.what()); }
}

////////////////////////////////////////////////////////////////////////////////
// KIND::format 
////////////////////////////////////////////////////////////////////////////////
std::string
d::sql::make_query_format(const std::vector<obj>& V)
{ try {
    throw err("not implemented yet");
 } catch (const std::exception &e) { throw err(e.what()); }
}

/**
 old codes
 */

























