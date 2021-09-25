#include <iostream>
#include <pqxx/pqxx>
#include <util.hpp>
#include <database.hpp>
using namespace std;

#define DATABASE_CONNECTION "dbname=pet user=borges password=JSG3bor_g873sqlptgs78b hostaddr=127.0.0.1 port=5432"
/**
 selects:
  - to retry time in minutes in session:
  SELECT EXTRACT(EPOCH FROM (CURRENT_TIMESTAMP - (select date from head where id = '123')))/60;
 */

//void* CB;
pqxx::connection_base* CB;

int main() {
 try {
	 d::database_connection = "";
	 cout << d::quote("pula", DATABASE_CONNECTION) << "\n";
	 cout << d::quote("pula1") << "\n";
	 cout << d::quote("pula2") << "\n";
	 cout << d::quote("pula3") << "\n";
	 cout << d::quote("pula4") << "\n";
	 
	 /*
 	string ww = "   Mue12	   ";
 	printf("\"%s\"\n", ww.c_str());
 	u::trim(ww);
 	u::tolower(ww = ww + "bizARO");
 	printf("\"%s\"\n", ww.c_str());
 	u::trim(ww);
 	if(u::isalnum(ww)) printf("is alnum() = \"%s\"\n", ww.c_str());
 */
 
 
 
   /* pqxx::connection C("dbname = session user = borges password = JSG3bor_g873sqlptgs78b \
    	hostaddr = 127.0.0.1 port = 5432");
    if (C.is_open()) {
    	std::cout << "Opened database successfully: " << C.dbname() << std::endl;
    } else {
    	std::cout << "Can't open database" << std::endl; return 1;
    } 
    pqxx::nontransaction N(C);
    CB = (pqxx::connection_base*)&N;
    string s1 = "SELECT EXTRACT(EPOCH FROM (CURRENT_TIMESTAMP - (select date from head where id = '"; //'1')))/60 as time";
	d::obj sh {{ {"id", { {}, {}} }, {"time", {{}, {" ", "ate", "date"}} }}, {},
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
	printf("\n===changeXX===\n");
	sh["id"].set() = "ok";
	printf("xp\n");
	xp.print();
	printf("sh\n");
	sh.print();
	//string str = ((pqxx::connection_base*)CB)->quote("xupeta");
	string str = CB->quote("xus");
	printf("%s\n", str.c_str());
	if(sh.empty()) printf("sh is empty\n");
	else printf("sh is NOT empty\n");
	d::table table(sh);
	table.print();
	d::sql sql {{"select * from head"}};
	printf("refresh\n");
	sh["id"].set() = "123";
	table.refresh(N, sql, { std::move(sh) });
	d::commit(N);
	table.print();
	sh.print();
	if(sh.empty()) printf("sh is empty\n");
	else printf("sh is NOT empty\n");
	/*printf("[%s]: \"%s\"\n", "id", table[0]["id"].get().c_str());
	printf("[%s]: \"%s\"\n", "time", table[0]["time"].get().c_str());
	
	printf("for range\n");
	for(auto& o : table) {
		for(auto& f : o)
			printf("[%s]: \"%s\"\n", f.first.c_str(), f.second.get().c_str());
	}*/
	
	/*
	d::obj ss {"session", {"diff"}};
	ss.select("SELECT EXTRACT(EPOCH FROM (CURRENT_TIMESTAMP - (select date from head where id = '"+sh["id"].get()+"')))/60;",
		{{"diff"}});

	printf("diff: \"%s\"\n", ss["diff"].get().c_str());
	
	ss.select("SELECT CURRENT_TIMESTAMP;", {{"diff"}});
	d::obj s2 {"head", {"id", "date"}};
	s2.insert({{"id", "111"}, {"date", ss["diff"].get()}});
	C.disconnect ();*/
 } catch (const std::exception &e) {
	err(e.what());
	return 1;
 }
}























