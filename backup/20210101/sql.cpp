
#include "database.hpp"

////////////////////////////////////////////////////////////////////////////////
// public functions - constructor
////////////////////////////////////////////////////////////////////////////////
d::sql::sql(const kind& _kind,
	const std::vector<std::variant<std::monostate, std::string, sql_arg>>& statement,
	const std::vector<sql_result_arg>& result_arg)
{ try {
	if(sql_statement.empty()) throw err("sql_statement cannot be an empty string");
	this->Kind = _kind;
	this->arg = arg;
	this->result = result;
 } catch (const std::exception &e) { throw err(e.what()); }
}

///////////////////////////////////////////////////////////////////////////////
// public functions - run functions
////////////////////////////////////////////////////////////////////////////////
void d::sql::run0(
	const std::map<std::string, field>& m1, const std::map<std::string, field>& m2)
{ try {
	pqxx::connection C("dbname = session user = borges password = JSG3bor_g873sqlptgs78b \
      hostaddr = 127.0.0.1 port = 5432");
    pqxx::work W{C};
	std::string query = make_query(m1, m2);
    W.exec0(query);
    W.commit();
 } catch (const std::exception &e) { throw err(e.what()); }
}

std::map<std::string, std::string>
d::sql::run1(
	const std::map<std::string, field>& m1, const std::map<std::string, field>& m2)
{ try {
	pqxx::connection C("dbname = session user = borges password = JSG3bor_g873sqlptgs78b \
      hostaddr = 127.0.0.1 port = 5432");
    pqxx::work W{C};
	std::string query = make_query(m1, m2);
    pqxx::row R{ W.exec1(query) };
    
    std::map<std::string, std::string> r = copy_result(R, main);
    
    W.commit();
    return r;
 } catch (const std::exception &e) { throw err(e.what()); }
}

////////////////////////////////////////////////////////////////////////////////
// private functions - auxiliar functions
////////////////////////////////////////////////////////////////////////////////

std::string
d::sql::make_query(
	const std::map<std::string, field>& m1, const std::map<std::string, field>& m2)
{ try {
	std::string query = "";
	switch(Kind)
	{
		case kind::CONCATENATION: query = make_query_concatenation(m1);
		case kind::CONCATENATION_OUT: query = make_query_concatenation_out(m1, m2);
		default: throw err("no sql kind found. kind: %d", static_cast<int>(kind));
	}
	
	return query;
 } catch (const std::exception &e) { throw err(e.what()); }
}

std::map<std::string, std::string>
copy_result(const pqxx::row& From, std::map<std::string, field>& dest)
{ try {
	if(Kind = kind::NONE) throw err("sql kind is NONE - forbidden run this kind");
	if(Kind = kind::CONCATENATION_OUT) return copy_result_concatenation_out(From, dest);
	
	// make auto copy
	for(const auto& F : From)
	{
		auto i = dest.find(F.name());
		if(i == dest.end()) { // verifica se existe a chave em dest
   			throw err("column result name does not exists in object key."
   				"column name: \"%s\" | column value: \"%s\"", F.name(), 
   				F.is_null() ? "" : F.as<std::string>());
   		}
		
		//if(F.is_null() == true) i->second.set() = "";
    	//else i->second.set() = F.as<std::string>;
		i->second.set() = F.to_string();
		i->second.check_read(); // trata a entrarda
		
		return {};
	}
 } catch (const std::exception &e) { throw err(e.what()); }
}

////////////////////////////////////////////////////////////////////////////////
// KIND::concatenation - functions
////////////////////////////////////////////////////////////////////////////////
std::string
d::sql::make_query_concatenation(
	const std::map<std::string, field>& map)
{ try {
    std::string query = "";
    for(auto const& i : statement)
    {
    	switch(static_cast<sql_stype>(i.index()))
    	{
    		case sql_stype::SQL_ARG:
    			auto const& arg = std::get<std::sql_arg>(i);
    			auto f = m1.find(arg.key()); // verifica se a chave é do primeiro mapa
    			if(f == m1.end()) {
    				throw err("not found arg.key() in field map. - "
    						"arg.key(): \"%s\" - arg.name(): \"%s\"", arg.key(), arg.name()); }
    			// verifica qual é o tipo de string que deve ser buscada
    			if(arg.type() == sql_earg::value)
    			{
    				f->second.check_write(); // verifica e trata o valor se necessário para inserir no banco de dados.
					query += f->second.str(); // get the value of field in string format
				} else {
    				auto n = f->second.name().find(i.name());
    				if(n == f->second.name().end())
    					{ throw err("not found arg.name() in field name map. - "
    						"arg.key(): \"%s\" - arg.name(): \"%s\"", arg.key(), arg.name()); }
    				
    				str += n->second; // get the name of field - não tem check pois não é 
    			} break;
    		case sql_stype::STR: str += std::get<std::string>(i); break;
    		default: throw err("no statement type in index: %d", i.index());
    	}
    }
    return query;
 } catch (const std::exception &e) { throw err(e.what()); }
}


////////////////////////////////////////////////////////////////////////////////
// KIND::concatenation out - functions
////////////////////////////////////////////////////////////////////////////////
std::string
d::sql::make_query_concatenation_out(
	const std::map<std::string, field>& m1, const std::map<std::string, field>& m2)
{ try {
    std::string query = "";
    for(auto const& i : statement)
    {
    	switch(static_cast<sql_stype>(i.index()))
    	{
    		case sql_stype::SQL_ARG:
    			auto const& arg = std::get<std::sql_arg>(i);
    			auto f = m1.find(arg.key()); // verifica se a chave é do primeiro mapa
    			if(f == m1.end()) {
    				f = m2.find(arg.key()); // verifica se a chave é do segundo mapa
    				if(f == m2.end())
    					{ throw err("not found arg.key() in none of both maps. - "
    						"arg.key(): \"%s\" - arg.name(): \"%s\"", arg.key(), arg.name()); }
    			}
    			// verifica qual é o tipo de string que deve ser buscada
    			if(arg.type() == sql_earg::value)
    			{
    				f->second.check_write(); // verifica e trata o valor se necessário para inserir no banco de dados.
					query += f->second.str(); // get the value of field in string format
				} else {
    				auto n = f->second.name().find(i.name());
    				if(n == f->second.name().end())
    					{ throw err("not found arg.name() in field name map. - "
    						"arg.key(): \"%s\" - arg.name(): \"%s\"", arg.key(), arg.name()); }
    				
    				str += n->second; // get the name of field - não tem check pois não é 
    			} break;
    		case sql_stype::STR: str += std::get<std::string>(i); break;
    		default: throw err("no statement type in index: %d", i.index());
    	}
    }
    
    return query;
 } catch (const std::exception &e) { throw err(e.what()); }
}

std::map<std::string, std::string>
copy_result_concatenation_out(const pqxx::row& From, std::map<std::string, field>& dest)
{ try {
	if(F.size() != result_arg.size())
		{ throw err("Different size of database result row and result argument vector - "
			"database result row: %d - result_arg: %d", F.size(), result_arg.size()); }
	
	std::map<std::string, std::string> rest = {};
	for(auto i = 0; i < result_arg.size(); ++i)
	{
		std::string val = ""; // buffer to keep the result that will be set
    	if(F[i].is_null() == true) val = result_arg.null();
    	else val = F[i].as<std::string>();
    		
   		auto j = dest.find(result_arg[i].key());
   		if(j == col.end()) { // verifica se existe a chave em main
   			auto r = rest.find(result_arg[i].key());
   			if(r == rest.end()) r->second = val; // verifica se é a primeira inserção no map
   			else throw err("duplicate key in result rest - key: \"%s\"", r->first);
   		} else {
   			j->second.set() = val;
   			j->second.check_read();
   		}
	}
 } catch (const std::exception &e) { throw err(e.what()); }
}

/**
 old codes
 */

























