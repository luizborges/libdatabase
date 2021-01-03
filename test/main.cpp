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
 
//void* CB;
pqxx::connection_base* CB;

int main() {
 try {
    pqxx::connection C("dbname = session user = borges password = JSG3bor_g873sqlptgs78b \
    	hostaddr = 127.0.0.1 port = 5432");
    if (C.is_open()) {
    	std::cout << "Opened database successfully: " << C.dbname() << std::endl;
    } else {
    	std::cout << "Can't open database" << std::endl; return 1;
    } 
    pqxx::nontransaction N(C);
    CB = (pqxx::connection_base*)&N;
    string s1 = "SELECT EXTRACT(EPOCH FROM (CURRENT_TIMESTAMP - (select date from head where id = '"; //'1')))/60 as time";
	d::obj sh {{ {"id", { {}, {}} }, {"time", {{}, {" ", "ate", "date"}} }}, 
		//{{"init", { d::kind::CONCAT, {"select id, date as time from head where id = '", "id", "'"}}}}};
		{{"init", { d::kind::CONCAT, {"select id, date from head where id = ", "id", ""}}}}};
		//{{"init", { d::kind::CONCAT, {"select * from head where id = '", "id", "'"}}}}};
		//{{"init", { {s1, "id", "')))/60 as time"}}}}};
		//SELECT EXTRACT(EPOCH FROM (CURRENT_TIMESTAMP - (select date from head where id = '1')))/60 as time;
	
	
	printf("[%s]: \"%s\"\n", "time", sh["time"].get().c_str());
	sh.run1({"init"}, {{"id", "123"}});
	printf("[%s]: \"%s\"\n", "id", sh["id"].get().c_str());
	printf("[%s]: \"%s\"\n", "time", sh["time"].get().c_str());
	
	sh.print();
	
	d::obj xp = {};
	
	xp = sh;
	printf("xp\n");
	xp.print();
	printf("sh\n");
	sh.print();
	printf("\n===change===\n");
	sh["id"].set() = "ok";
	printf("xp\n");
	xp.print();
	printf("sh\n");
	sh.print();
	//string str = ((pqxx::connection_base*)CB)->quote("xupeta");
	string str = CB->quote("xus");
	printf("%s\n", str.c_str());
	
	/*
	d::obj ss {"session", {"diff"}};
	ss.select("SELECT EXTRACT(EPOCH FROM (CURRENT_TIMESTAMP - (select date from head where id = '"+sh["id"].get()+"')))/60;",
		{{"diff"}});

	printf("diff: \"%s\"\n", ss["diff"].get().c_str());
	
	ss.select("SELECT CURRENT_TIMESTAMP;", {{"diff"}});
	d::obj s2 {"head", {"id", "date"}};
	s2.insert({{"id", "111"}, {"date", ss["diff"].get()}});*/
	C.disconnect ();
 } catch (const std::exception &e) {
	err(e.what());
	return 1;
 }
}























