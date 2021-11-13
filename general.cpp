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
	    pqxx::nontransaction N(C);// inicia uma transação com o BD - Create a notransactional object.
    
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

pqxx::row database::selectr1(const std::string& sql, const std::string& database_connection)
{ try {
    if(sql.empty()) throw err("sql of select query is empty");
    if(database_connection.empty()) throw err("database connection string is empty");
    
    pqxx::connection C(database_connection);
	try { // necessário for run C.disconnect() in catch()
    	if(!C.is_open()) throw err("Can't open session database.\nconnection_arg: \"%s\"", database_connection.c_str());
	    pqxx::nontransaction N(C);// inicia uma transação com o BD - Create a notransactional object.
    
        ////////////////////////////////////////////////////////////////////////////////
		// executa o sql e insere o resultado nas estruturas
		////////////////////////////////////////////////////////////////////////////////
    	auto R{ N.exec1(sql) }; // executa o sql 
    	C.disconnect();
        return R;
 	} catch (pqxx::sql_error const &e) {
 		C.disconnect(); throw err("SQL error: %s\nQuery was: \"%s\"", e.what(), e.query().c_str());
	} catch(std::exception const& e) { C.disconnect(); throw err(e.what()); }
 } catch (const std::exception &e) { throw err(e.what()); }
}

std::tuple<pqxx::result, std::string>
database::selectt(
	const std::string& table_name,
	const std::string& select,
	const std::string& where,
	const std::string& order_by,
    const std::string& page_str,
	const std::string& limit_str,
	const std::string& database_connection)
{ try {
	////////////////////////////////////////////////////////////////////////////////
	// check input
	////////////////////////////////////////////////////////////////////////////////
    if(table_name.empty()) throw err("table name string is empty.");
    if(database_connection.empty()) throw err("database connection string is empty.");

	////////////////////////////////////////////////////////////////////////////////
	// check @argument limit
	////////////////////////////////////////////////////////////////////////////////
	int limit = 10; // valor defaut
    if(!limit_str.empty()) {
		try { limit = std::stoi(limit_str); }
		catch (const std::exception &e) { throw err("%s\nLimit_str: \"%s\".", e.what(), limit_str.c_str()); }
		
		if(limit < 1 && limit != -1) 
			throw err("Limit argument is incorrect. Limit must have been greater than 0. Limit is %d", limit);
	}

	////////////////////////////////////////////////////////////////////////////////
	// check @argument page - set offset
	////////////////////////////////////////////////////////////////////////////////
	int offset = 0; // valor default
	if(!page_str.empty()) {
		try { offset = std::stoi(page_str); }
		catch (const std::exception &e) { throw err("%s\nPage_str: \"%s\".", e.what(), page_str.c_str()); }

		offset *= offset < 0 ? -1 : limit;
	}

	///////////////////////////////////////////////////////////////////////////////
	// cria o sql de busca
	////////////////////////////////////////////////////////////////////////////////
    std::string sql = "SELECT ";
	sql += select.empty() ? "*" : select;
	sql += " FROM " + table_name;
	sql += where.empty() ? " " : (" WHERE " + where);
	sql += order_by.empty() ? " " : (" ORDER BY " + order_by);
	sql += " OFFSET " + std::to_string(offset);
	sql += limit == -1 ? "" : (" LIMIT " + std::to_string(limit));
	sql += ";"; // finaliza o sql

	///////////////////////////////////////////////////////////////////////////////
	// cria o sql count
	////////////////////////////////////////////////////////////////////////////////
	std::string sql_count = "SELECT COUNT(*)";
	sql_count += " FROM " + table_name;
	sql_count += where.empty() ? " " : " WHERE " + where;
	sql_count += ";"; // finaliza o sql

	///////////////////////////////////////////////////////////////////////////////
	// abre a parte de conexão de dados
	////////////////////////////////////////////////////////////////////////////////
	pqxx::connection C(database_connection);
	try { // necessário for run C.disconnect() in catch()
    	if(!C.is_open()) throw err("Can't open session database.\nconnection_arg: \"%s\"", database_connection.c_str());
	    pqxx::nontransaction N(C);// inicia uma transação com o BD - Create a notransactional object.
    
        ////////////////////////////////////////////////////////////////////////////////
		// executa o sql e disconnecta do bando de dados
		////////////////////////////////////////////////////////////////////////////////
    	auto R{ N.exec(sql) }; // executa o sql 
		auto COUNT{N.exec1(sql_count)}; // executa o SELECT COUNT()
    	C.disconnect();

		////////////////////////////////////////////////////////////////////////////////
		// Retira o valor count
		////////////////////////////////////////////////////////////////////////////////
        std::string count = COUNT[0].is_null() ? "0" : COUNT[0].as<std::string>();

		return { R, count };
 	} catch (pqxx::sql_error const &e) {
 		C.disconnect(); throw err("SQL error: %s\nQuery was: \"%s\"", e.what(), e.query().c_str());
	} catch(std::exception const& e) { C.disconnect(); throw err(e.what()); }
 } catch (const std::exception &e) { throw err(e.what()); }
}

void database::exec0(const std::string& sql, const std::string& database_connection)
{ try {
    if(sql.empty()) throw err("sql string is empty");
    if(database_connection.empty()) throw err("database connection string is empty");
    
    pqxx::connection C(database_connection);
    std::unique_ptr<pqxx::work> W; // necessário para não precisar de outro try-catch to do roolback - W->abort()
	try { // necessário for run C.disconnect() in catch()
    	if(!C.is_open()) throw err("Can't open session database.\nconnection_arg: \"%s\"", database_connection.c_str());
	    W = std::make_unique<pqxx::work>(C); // inicia uma transação com o banco de dados - Create a transactional object.
    
        ////////////////////////////////////////////////////////////////////////////////
		// executa o sql e faz commit
		////////////////////////////////////////////////////////////////////////////////
        W->exec0(sql); // executa o sql - se houver respota retorna erro
		W->commit(); // somente faz o commit se NÃO ocorreu erro algum no salvamento
    	C.disconnect ();
 	} catch (pqxx::sql_error const &e) {
 		W->abort(); C.disconnect();
 		throw err("Rollback the transaction. SQL error: %s\nQuery was: \"%s\"", e.what(), e.query().c_str());  
	} catch(std::exception const& e) { C.disconnect(); throw err(e.what()); }
 } catch (const std::exception &e) { throw err(e.what()); }
}


////////////////////////////////////////////////////////////////////////////////
// private functions - body
////////////////////////////////////////////////////////////////////////////////



















