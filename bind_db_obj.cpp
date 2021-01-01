

#include "database.hpp"
////////////////////////////////////////////////////////////////////////////////
// public functions - constructor
////////////////////////////////////////////////////////////////////////////////

d::obj::obj(const std::string& table, const std::vector<std::string>& column)
{ try {
	this->table = table;

	for(auto const& c: column)
	{
		field i;
		col.emplace(c, i);
	}
 } catch (const std::exception &e) { throw err(e.what()); }
}

////////////////////////////////////////////////////////////////////////////////
// public functions - auxiliar functions
////////////////////////////////////////////////////////////////////////////////
void
d::obj::print()
{ try {
	std::printf("\n\nPrint d::obj\n");
	std::printf("Table name is \"%s\"\n", table.c_str());
	std::printf("Values of the object: [\"column_name\"] \"column_value\"\n");
	
	for(auto const& a: col)
	{
		std::fprintf(stderr, "[\"%s\"] \"%s\" | Type: \"%s\" | Index: %d\n", a.first.c_str(), 
			a.second.str().c_str(), a.second.type().c_str(), a.second.index());
	}	
 } catch (const std::exception &e) { throw err(e.what()); }
}

d::field& // write mode
d::obj::operator[](const std::string& column_name)
{ try {
	auto i = col.find(column_name);
	if(i == col.end()) { // check if has the index
		std::fprintf(stderr, "\nDATABASE ERROR - Key not fund - column_name: \"%s\"\n", column_name.c_str());
		std::fprintf(stderr, "Print all values of the object.");
		print();
		throw err("");
	}
	
	return i->second;
 } catch (const std::exception &e) { throw err(e.what()); }
}

void
d::obj::set(const std::map<std::string, std::variant<D_FIELD_TYPES>>& row)
{ try{
	if(row.empty() == false) 
		for(auto const& i : row) // atualiza os valores do objeto
		{
			auto j = col.find(i.first); // verifica se existe a chave
			if(j == col.end()) { throw err("DATABASE: No column in object. - column name: \"%s\"", i.first); }
			j->second.set() = i.second; // atualiza o valor do objeto
		}
 } catch (const std::exception &e) { throw err(e.what()); }
}
////////////////////////////////////////////////////////////////////////////////
// public functions - sql functions
////////////////////////////////////////////////////////////////////////////////
void
d::obj::insert(const std::map<std::string, std::variant<D_FIELD_TYPES>>& row)
{try {
	try{ set(row); }
	catch (const std::exception &e) { throw err("Updating the values of object"); }
	
	pqxx::connection C("dbname = session user = borges password = JSG3bor_g873sqlptgs78b \
      hostaddr = 127.0.0.1 port = 5432");
    pqxx::work W{C};
    
    std::string sql = "INSERT INTO " + table + " (";
    std::string val = "VALUES ("; // values of insert
   for(auto const& i: col) // cria o sql que será enviado para o banco de dados
   {	
   		try{ const_cast<field&>(i.second).check_write(); } // check erros
   		catch(const std::exception &e) { print(); throw err("error in column of name: \"%s\"\n", i.first.c_str()); }
   		
   		if(i.second.etype() == field_type::STR) {
   			if(i.second.get().empty() == true) continue; // no insert the field in sql
   			else val += W.quote(i.second.get()) + ","; // escaping + quote string to safe
   		}
   		else if(i.second.etype() == field_type::NO_TYPE) continue; // no insert the field in sql
   		else val += i.second.str() + ","; // no need to quote - because std::variant guarantees
   		
   		sql += i.first + ",";
   }
   
   sql.back() = ')'; // change the last character of string
   val.back() = ')'; // change the last character of string
   
   sql += " " + val + ";"; // create a sql statement
   std::printf("sql: \"%s\"\n", sql.c_str());
   
   W.exec0(sql); // execute sql
   W.commit();
 } catch (const std::exception &e) { throw err(e.what()); }
}

void
d::obj::select(
	const std::string& sql_statement,
	const std::vector<field_query_result>& select_column,
	const std::map<std::string, std::variant<D_FIELD_TYPES>>& row)
{ try{
	try{ set(row); }
	catch (const std::exception &e) { throw err("Updating the values of object"); }
	
	pqxx::connection C("dbname = session user = borges password = JSG3bor_g873sqlptgs78b \
      hostaddr = 127.0.0.1 port = 5432");
    pqxx::nontransaction N{C}; // Create a non-transactional object
	pqxx::row R{ N.exec1(sql_statement) }; // Execute SQL query
 
    if(select_column.empty() == true)
    {
    	throw err("not implemented yet.");
    } else {
    	const int size = R.size() < col.size() ? R.size() : col.size();
    	for(int i = 0; i < size; ++i)
    	{
    		std::string s = ""; // buffer to keep the result that will be set in this->col
    		if(R[i].is_null() == true &&
    			select_column[i].etype() != field_type::STR) s = "0";
    		else if(R[i].is_null() == false) s = R[i].as<std::string>();
    		
    		auto j = col.find(const_cast<field_query_result&>(select_column[i]).get_column_name());
    		if(j == col.end()) { // verifica se existe a chave em this->col
    			std::fprintf(stderr, "\nDATABASE ERROR - Key not fund - column_name: \"%s\"\n",
    				const_cast<field_query_result&>(select_column[i]).get_column_name().c_str());
				std::fprintf(stderr, "Print all values of the object.");
				print(); throw err("");
    		}
    		j->second.set() = s;
    		j->second.str_to(select_column[i].etype()); // transforma no tipo correspondente
    	}
    }
 } catch (const std::exception &e) { throw err(e.what()); }
}

////////////////////////////////////////////////////////////////////////////////
// private functions - auxiliar functions
////////////////////////////////////////////////////////////////////////////////























