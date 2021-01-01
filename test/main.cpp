#include <iostream>
#include <pqxx/pqxx>
#include <util.hpp>
#include <database.hpp>
using namespace std;
/**
 selects:
  - to retry time in minutes in session:
  SELECT EXTRACT(EPOCH FROM (CURRENT_TIMESTAMP - (select date from head where id = '123')))/60;
 */
 


int main() {
 try {
    pqxx::connection C("dbname = session user = borges password = JSG3bor_g873sqlptgs78b \
    	hostaddr = 127.0.0.1 port = 5432");
    if (C.is_open()) {
    	std::cout << "Opened database successfully: " << C.dbname() << std::endl;
    } else {
    	std::cout << "Can't open database" << std::endl; return 1;
    } */
    
	d::obj sh {"session", {"id", "time"}};
	
	//sh.select("select * from head;", {{"id", d::field_type::STR}, {"time", "int"}});
	sh.select("select * from head where id = '123';", {{"id"}, {"time"}});
	printf("[%s]: \"%s\"\n", "id", sh["id"].get().c_str());
	printf("[%s]: \"%s\"\n", "time", sh["time"].get().c_str());
	
	d::obj ss {"session", {"diff"}};
	ss.select("SELECT EXTRACT(EPOCH FROM (CURRENT_TIMESTAMP - (select date from head where id = '"+sh["id"].get()+"')))/60;",
		{{"diff"}});

	printf("diff: \"%s\"\n", ss["diff"].get().c_str());
	
	ss.select("SELECT CURRENT_TIMESTAMP;", {{"diff"}});
	d::obj s2 {"head", {"id", "date"}};
	s2.insert({{"id", "111"}, {"date", ss["diff"].get()}});
//	C.disconnect ();
 } catch (const std::exception &e) {
	err(e.what());
	return 1;
 }
}























