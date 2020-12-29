#include <iostream>
#include <pqxx/pqxx>
#include <util.hpp>
#include <database.hpp>

int main() {
 try {
      pqxx::connection C("dbname = session user = borges password = JSG3bor_g873sqlptgs78b \
      hostaddr = 127.0.0.1 port = 5432");
      if (C.is_open()) {
         std::cout << "Opened database successfully: " << C.dbname() << std::endl;
      } else {
         std::cout << "Can't open database" << std::endl;
         return 1;
      }
      
      d::obj sh {"session", {"id", "time"}};
      sh["id"] = "xupeta";
      sh.insert();
      
      C.disconnect ();
 } catch (const std::exception &e) {
	err(e.what());
	return 1;
 }
}


