#include "database.hpp"


////////////////////////////////////////////////////////////////////////////////
// public global variables
////////////////////////////////////////////////////////////////////////////////
std::string database::database_connection = "";

////////////////////////////////////////////////////////////////////////////////
// private functions - head
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// public functions
////////////////////////////////////////////////////////////////////////////////
std::string database::quote(const std::string& str, const std::string& database_connection)
{ try {
	static pqxx::lazyconnection C {database_connection};
	static bool disconnect = true;
	if(disconnect) {
		C.disconnect();
		disconnect = false;
	}
	return C.quote(str);
 } catch (const std::exception &e) { throw err(e.what()); }
}


pqxx::result database::selectr(const std::string& sql, const std::string& database_connection)
{ try {
    if(sql.empty()) throw err("sql of select query is empty");
    if(database_connection.empty()) throw err("database connection string is empty");
    
    pqxx::connection C(database_connection);
	try { // necessário for run C.disconnect() in catch()
    	if(!C.is_open()) throw err("Can't open session database.\nconnection_arg: \"%s\"", database_connection.c_str());
	    pqxx::nontransaction N(C);// inicia uma transação com o BD /* Create a notransactional object. */
    
        ////////////////////////////////////////////////////////////////////////////////
		// executa o sql e insere o resultado nas estruturas
		////////////////////////////////////////////////////////////////////////////////
    	auto R{ N.exec(sql) }; // executa o sql 
    	C.disconnect();
        return R;
 	} catch (pqxx::sql_error const &e) {
 		C.disconnect(); throw err("SQL error: %s\nQuery was: \"%s\"", e.what(), e.query().c_str());
	} catch(std::exception const& e) { C.disconnect(); throw err(e.what()); }
 } catch (const std::exception &e) { throw err(e.what()); }
}

////////////////////////////////////////////////////////////////////////////////
// private functions - body
////////////////////////////////////////////////////////////////////////////////



















