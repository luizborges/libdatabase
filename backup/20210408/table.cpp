#include "database.hpp"


////////////////////////////////////////////////////////////////////////////////
// public functions - constructor
////////////////////////////////////////////////////////////////////////////////
d::table::table(const obj& Obj)
{ try {
	_model = Obj;
 } catch (const std::exception &e) { throw err(e.what()); }
}

////////////////////////////////////////////////////////////////////////////////
// public functions
////////////////////////////////////////////////////////////////////////////////
d::obj&
d::table::move(const obj& Obj, const size_t idx)
{ try {
	if(idx >= _obj.size())
		throw err("No index in table. idx: %lu | table size: %lu", idx, _obj.size());
	
	_obj[idx] = std::move(Obj);
	return _obj[idx];
 } catch (const std::exception &e) { throw err(e.what()); }
}

void
d::table::print() const
{ try {
	// desabilita o cabeçalho - para não exibir que é mensagem de erro.
	// ideal utilizar esta função pois além de exibir em stderr pode também exibir em outra saída se desejar (tela)
	u::error::set_header(false);
	err("Print d::table: HEAD - Informations of model object. BODY: only values of objects in table.\nHEAD:");
	u::error::set_header(true);
	_model.print();
	u::error::set_header(false); err("BODY:"); u::error::set_header(true);
	int i = 0;
	for(const auto& it : _obj) it.printv("["+std::to_string(++i)+"]: ");
 } catch (const std::exception &e) { throw err(e.what()); }
}

////////////////////////////////////////////////////////////////////////////////
// public functions - overloading operators
////////////////////////////////////////////////////////////////////////////////
d::obj& 
d::table::operator[] (const size_t idx)
{ try {
	if(idx >= _obj.size())
		throw err("No index in table. idx: %lu | table size: %lu", idx, _obj.size());
	
	return _obj[idx];
 } catch (const std::exception &e) { throw err(e.what()); }
}

const d::obj& 
d::table::operator[] (const size_t idx) const
{ try {
	if(idx >= _obj.size())
		throw err("No index in table. idx: %lu | table size: %lu", idx, _obj.size());
	
	return _obj[idx];
 } catch (const std::exception &e) { throw err(e.what()); }
}
////////////////////////////////////////////////////////////////////////////////
// public functions - sql/run functions
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// private functions - auxiliar functions
////////////////////////////////////////////////////////////////////////////////
void
d::table::check_model(const obj& Model)
{ try {
	if(Model.empty())
	{
		if(_model.empty() == false) return;
		else throw err("No valid model. Model passed and inner model of Table is empty");
	}
	_model = Model;
 } catch (const std::exception &e) { throw err(e.what()); }
}

void
d::table::copy_result(const pqxx::result& T, const sql& Query)
{ try {
	//std::printf("COLUMNS: %d | ROWS: %d\n", T.columns(), T.size());
	if(T.empty()) return;
	if(T.columns() > _model.size()) {
		throw err("Table of result postres is different from model object. "
		"Number of column in postgres: %d - Number of fields in object: %lu", T.columns(), _obj.size()); }
	
	_obj.resize(T.size(), _model); // reseta e inicializa a tabela já com N linhas e no modelo correto de objeto
	//std::printf("COLUMNS: %d | ROWS: %d\n", T.columns(), T.size());
	std::vector<obj> V(1);
	int i = 0;
	for(auto const& R : T)
	{
		V[0] = std::move(_obj[i]); // to not copy the vector, but pass reference
		const_cast<sql&>(Query).copy_result(R, V);
		_obj[i] = std::move(V[0]); // recupera a referência
	}
 } catch (const std::exception &e) { throw err(e.what()); }
}


































