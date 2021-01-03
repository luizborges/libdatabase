#include "database.hpp"

////////////////////////////////////////////////////////////////////////////////
// global variable to use quote() function of postgres
////////////////////////////////////////////////////////////////////////////////
pqxx::connection_base* d::___CB = nullptr;

////////////////////////////////////////////////////////////////////////////////
// public functions - constructor
////////////////////////////////////////////////////////////////////////////////
d::obj::obj(const std::vector<std::string>& field_key,
	  		const std::unordered_map<std::string, sql>& sql_real,
	  		const std::unordered_map<std::string, std::vector<std::string>>& sql_fake)
{ try {
	for(auto const& c: field_key)
	{
		_field.emplace(c, field());
	}
	_sql_real = sql_real;
	_sql_fake = sql_fake;
	check_equal_key_sql_real_fake();
 } catch (const std::exception &e) { throw err(e.what()); }
}

d::obj::obj(const std::unordered_map<std::string, field>& _field,
	  		const std::unordered_map<std::string, sql>& sql_real,
	  		const std::unordered_map<std::string, std::vector<std::string>>& sql_fake)
{ try {
	this->_field = _field;
	_sql_real = sql_real;
	_sql_fake = sql_fake;
	check_equal_key_sql_real_fake();
 } catch (const std::exception &e) { throw err(e.what()); }
}


////////////////////////////////////////////////////////////////////////////////
// public functions - auxiliar functions
////////////////////////////////////////////////////////////////////////////////
void
d::obj::print() // utilizando o err para mandar a saída não somente no stderr mas também recuperar ela se for do interesse
{ try {
	std::string msg = "Print d::obj\n";
	msg += "Values of the object: [\"field_key\"] \"field_value\"\n";
	
	for(auto const& a: _field)
	{
		msg += u::format("[\"%s\"] \"%s\" | Type: \"%s\" | Index: %d\n", a.first.c_str(), 
			a.second.str().c_str(), a.second.type().c_str(), a.second.index());
	}
	u::error::set_header(false); err(msg.c_str()); u::error::set_header(true);
 } catch (const std::exception &e) { throw err(e.what()); }
}

d::field& // write mode
d::obj::operator[](const std::string& column_name)
{ try {
	auto i = _field.find(column_name);
	if(i == _field.end()) { // check if has the index
		print(); 
		throw err("DATABASE ERROR - Key not fund - column_name: \"%s\"\n", column_name.c_str()); }
	
	return i->second;
 } catch (const std::exception &e) { throw err(e.what()); }
}

void
d::obj::set(const std::unordered_map<std::string, std::variant<D_FIELD_TYPES>>& row)
{ try{
	if(row.empty() == false) 
		for(auto const& i : row) // atualiza os valores do objeto
		{
			auto j = _field.find(i.first); // verifica se existe a chave
			if(j == _field.end()) { throw err("DATABASE: No column in object. - column name: \"%s\"", i.first); }
			j->second.set() = i.second; // atualiza o valor do objeto
		}
 } catch (const std::exception &e) { throw err(e.what()); }
}
////////////////////////////////////////////////////////////////////////////////
// public functions - run functions
////////////////////////////////////////////////////////////////////////////////
void d::obj::run0(const std::vector<std::string>& sql_key,
	const std::unordered_map<std::string, std::variant<D_FIELD_TYPES>>& _field_)
{ try {
	try { set(_field_); }
    catch (const std::exception &e) { print(); throw err(e.what()); }
    
    pqxx::connection C("dbname = session user = borges password = JSG3bor_g873sqlptgs78b \
      hostaddr = 127.0.0.1 port = 5432");
    pqxx::work W{C};
    init_quote(W);
    xrunX(sql_key, erun::run0, W);
    commit(W);
 } catch (const std::exception &e) { throw err(e.what()); }
}

void d::obj::run1(const std::vector<std::string>& sql_key,
	const std::unordered_map<std::string, std::variant<D_FIELD_TYPES>>& _field_)
{ try {
	try { set(_field_); }
    catch (const std::exception &e) { print(); throw err(e.what()); }
    
    pqxx::connection C("dbname = session user = borges password = JSG3bor_g873sqlptgs78b \
      hostaddr = 127.0.0.1 port = 5432");
    pqxx::work W{C};
    init_quote(W);
    xrunX(sql_key, erun::run1, W);
    commit(W);
 } catch (const std::exception &e) { throw err(e.what()); }
}

void d::obj::nrun0(const std::vector<std::string>& sql_key,
	const std::unordered_map<std::string, std::variant<D_FIELD_TYPES>>& _field_)
{ try {
	try { set(_field_); }
    catch (const std::exception &e) { print(); throw err(e.what()); }
    
    pqxx::connection C("dbname = session user = borges password = JSG3bor_g873sqlptgs78b \
      hostaddr = 127.0.0.1 port = 5432");
    pqxx::nontransaction N{C};
    init_quote(N);
    xrunX(sql_key, erun::run0, N);
    commit(N);
 } catch (const std::exception &e) { throw err(e.what()); }
}

void d::obj::nrun1(const std::vector<std::string>& sql_key,
	const std::unordered_map<std::string, std::variant<D_FIELD_TYPES>>& _field_)
{ try {
	try { set(_field_); }
    catch (const std::exception &e) { print(); throw err(e.what()); }
    
    pqxx::connection C("dbname = session user = borges password = JSG3bor_g873sqlptgs78b \
      hostaddr = 127.0.0.1 port = 5432");
    pqxx::nontransaction N{C};
    init_quote(N);
    xrunX(sql_key, erun::run1, N);
    commit(N);
 } catch (const std::exception &e) { throw err(e.what()); }
}


////////////////////////////////////////////////////////////////////////////////
// private functions - auxiliar functions
////////////////////////////////////////////////////////////////////////////////
void
d::obj::check_equal_key_sql_real_fake()
{ try {
	for(auto const& F : _sql_fake)
	{
		const auto R = _sql_real.find(F.first);
		if(R != _sql_real.end()) {
			throw err("Equal Keys in sql_fake and sql_real: \"%s\"", F.first); }
	}
 } catch (const std::exception &e) { throw err(e.what()); }
}







