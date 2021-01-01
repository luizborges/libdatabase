
#include "database.hpp"

////////////////////////////////////////////////////////////////////////////////
// public functions - constructor
////////////////////////////////////////////////////////////////////////////////
d::sql::sql(const std::string& id, const kind& _kind,
	const std::string& sql_statement, const std::vector<sql_arg>& arg,
	const std::vector<sql_result_arg>& result_arg)
{ try {
	if(id.empty()) throw err("id cannot be an empty string in this constructor");
	if(sql_statement.empty()) throw err("sql_statement cannot be an empty string");
	this->id = id;
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

    std::string query = expand_statement(m1, m2);
    W.exec0(query);
    W.commit();
 } catch (const std::exception &e) { throw err(e.what()); }
}

std::map<std::string, std::string>
d::sql::run1(
	std::map<std::string, field>& main, const std::map<std::string, field>& m2)
{ try {
	pqxx::connection C("dbname = session user = borges password = JSG3bor_g873sqlptgs78b \
      hostaddr = 127.0.0.1 port = 5432");
    pqxx::work W{C};

    std::string query = expand_statement(main, m2);
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
d::sql::expand_statement(
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
copy_result(const pqxx::row& From, std::map<std::string, field>& dest)
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



























