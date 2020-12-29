

#include "database.hpp"
////////////////////////////////////////////////////////////////////////////////
// public functions - constructor
////////////////////////////////////////////////////////////////////////////////

d::obj::obj(const std::string& table, const std::vector<std::string>& column)
{ try {
	this->table = table;

	for(const auto& c: column)
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
	
	for(const auto& a: col)
	{
		std::printf("[\"%s\"] \"%s\" | key: \"%s\" | NOT_NULL: %s | Type: \"%s\"\n", a.first.c_str(), 
			std::to_string(a.second.get()).c_str(), a.second.key_str(),
			a.second.notNull ? "true" : "false", a.second.type().c_str());
	}	
 } catch (const std::exception &e) { throw err(e.what()); }
}

auto // read mode
d::obj::operator[](const std::string& column_name)
{try {
	auto i = col.find(column_name);
	if(i == col.end()) { // check if has the index
		std::fprintf(stderr, "\nDATABASE ERROR - Key not fund - column_name: \"%s\"\n", column_name.c_str());
		std::fprintf(stderr, "Print all values of the object.");
		print();
		throw err();
	}
	
	return i->second.get();
 } catch (const std::exception &e) { throw err(e.what()); }
}

auto& // write mode
d::obj::operator[](const std::string& column_name)
{try {
	auto i = col.find(column_name);
	if(i == col.end()) { // check if has the index
		std::fprintf(stderr, "\nDATABASE ERROR - Key not fund - column_name: \"%s\"\n", column_name.c_str());
		std::fprintf(stderr, "Print all values of the object.");
		print();
		throw err();
	}
	
	return i->second.val;
 } catch (const std::exception &e) { throw err(e.what()); }
}

////////////////////////////////////////////////////////////////////////////////
// public functions - sql functions
////////////////////////////////////////////////////////////////////////////////
void
d::obj::insert(const std::map<std::string, std::string>& row)
{try {
	if(row.empty() == false) // atualiza os valores do objeto
		for(const auto& i : row)
			col[i.first] = i.second;

	pqxx::connection C("dbname = session user = borges password = JSG3bor_g873sqlptgs78b \
      hostaddr = 127.0.0.1 port = 5432");
    pqxx::work W{C};
    
    std::string sql = "INSERT INTO " + table + " (";
    std::string val = "VALUES ("; // values of insert
   for(const auto& i: col) // cria o sql que será enviado para o banco de dados
   {	
   		//if(i.second.val.empty() == true) // check errors
   		//{
   			if(i.second.key == primary_key) { 
   				print(); throw err("Primary Key is empty string - column name: \"%s\"\n", i.first.c_str()); }
   			if(i.second.notNull == true) {
   				print(); throw err("NotNull value is empty string (NULL) - column name: \"%s\"\n", i.first.c_str()); }
   			//continue; // não insere os valores vazios no sql
   		//}
   		
   		sql += i.first + ",";
   		//if()
   		val += W.quote(i.second.get()) + ",";
   }
   
   sql.back() = ')'; // change the last character of string
   val.back() = ')'; // change the last character of string
   
   sql += " " + val + ";"; // create a sql statement
   //std::printf("sql: \"%s\"\n", sql.c_str());
   
   W.exec0(sql); // execute sql
   W.commit();
 } catch (const std::exception &e) { throw err(e.what()); }
}

////////////////////////////////////////////////////////////////////////////////
// private functions - auxiliar functions
////////////////////////////////////////////////////////////////////////////////
























