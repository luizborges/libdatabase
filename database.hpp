/**
 *
 * @descripion: funciona como um header que contém todas as bibliotecas, é necessário apenas adicionar
 * essa biblioteca para ter acesso a todas a biblioteca.
 */
#ifndef DATABASEPP_H
#define DATABASEPP_H


////////////////////////////////////////////////////////////////////////////////
// Includes - default libraries - C
////////////////////////////////////////////////////////////////////////////////
#include <math.h>
#include <ctype.h>
#include <time.h>
#include <errno.h>


////////////////////////////////////////////////////////////////////////////////
// Includes - default libraries - C++
////////////////////////////////////////////////////////////////////////////////
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <map> 
#include <deque>
#include <memory>
#include <vector>
#include <string>
#include <variant>

#include <pqxx/pqxx>

////////////////////////////////////////////////////////////////////////////////
// Includes - system dependent libraries
////////////////////////////////////////////////////////////////////////////////
#if defined(unix) || defined(__unix) || defined(__unix__) || (defined (__APPLE__) && defined (__MACH__)) // Unix (Linux, *BSD, Mac OS X)
#include <unistd.h> // unix standard library
#include <sys/syscall.h>
#include <sys/types.h>
#include <dirent.h>
#endif

////////////////////////////////////////////////////////////////////////////////
// Includes - my libraries
////////////////////////////////////////////////////////////////////////////////
#include <util.hpp>
//#include <headers/stackTracer.h>
////////////////////////////////////////////////////////////////////////////////
// Includes - namespace
////////////////////////////////////////////////////////////////////////////////
/**#define TRACE_FUNC \
fprintf(stderr, "\n*****************************************\n");\
fprintf(stderr, "TRACE FUNC:: \"%s\" (%d, \"%s\")\n",\
__PRETTY_FUNCTION__, __LINE__, __FILE__);\
fprintf(stderr, "*****************************************\n");
*/

/**
 * macro utilizada para deixar mais legível o código.
 * ao invés de escrever: std::variant<std::monostate, std::string, bool, char, int, long, long long,
 *	unsigned char, unsigned int, unsigned long, unsigned long long>
 * basta escrever: std::variant<D_FIELD_TYPES>
 */
#define D_FIELD_TYPES std::monostate, std::string, char, int, long, long long, \
	unsigned char, unsigned int, unsigned long, unsigned long long, float, double, long double
//#define D_FIELD_TYPES std::monostate, std::string, double, int, char

////////////////////////////////////////////////////////////////////////////////
// namespace
////////////////////////////////////////////////////////////////////////////////
namespace d 
{

	//extern void config(const std::vector<std::string>&& conf);
	////////////////////////////////////////////////////////////////////////////////
	// interfaces of the library
	////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////
	// general functions of library
	////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////
	// general class/interface to abtract the database
	////////////////////////////////////////////////////////////////////////////////
	
	
	////////////////////////////////////////////////////////////////////////////////
	// general class to bind a row of a line in a object
	////////////////////////////////////////////////////////////////////////////////
	/**
	 * o objetivo desta classe é representar o campo da linha de maneira mais
	 * uniforme e transparente possível.
	 * obs: item_type = representa os tipos que o valor 
	 */
	enum class field_type {
		NO_TYPE, STR, CHAR, INT, LONG, LONG_LONG,
		UCHAR, UINT, ULONG, ULONG_LONG,
		FLOAT, DOUBLE, LONG_DOUBLE
	};
	
	/**
	 * by default quote is always executed in check_write(), for not executed quote use: no_quote
	 */
	enum class fopt { // options to field
		notnull, primary_key, foreign_key, // column options
		trim, rtrim, ltrim, upper, lower, // string options
		null0, // numeric options: null0 > replace null or empty string with 0
		in, not_in, // lists or vectors that value must be one element or not be
		quote, no_quote // execute quote - always in check_write() - is the last option to be executed
	};
	
	class field {
	 protected:
		std::variant<D_FIELD_TYPES> val = "";
		std::map<std::string, std::string> _name = {}; // column name of field
	 	std::vector<fopt> opt = {}; // options of field
	 	
	 public:
	 
	 	inline std::map<std::string, std::string>& name() { return _name; }
	 	/**
	 	 * return a string that represents the index.
	 	 * ex: index = 1 -> returned: "string"
	 	 */
		std::string type() const;
		
		/**
		 * return the representation of type in enum field_type.
		 * example: type is string: etype() = field_type::STR | index: 1 | type() = "string" | 
		 */
		inline field_type etype() const { return static_cast<field_type>(val.index()); }
		
		/**
		 * val.index()
		 */
		inline int index() const { return val.index(); }

		/**
		 * returned the key in string format.
		 */
		std::string key_str() const;
		
		/**
		 * Check and trata the values to write in database.
		 * Check and trata the values to read from database.
		 */
		void check_write();
		void check_read();
		
		/**
		 * cast the string type to another type.
		 * example: the value of field is a string "3"
		 * afther str_to(field_type::INT) -> the value of field is a int 3
		 * if: type = NO_TYPE -> throw error
		 * if: type = STR -> does nothing
		 * if: this->etype() != STR -> throw error
		 */
		void str_to(const field_type& type);
		
		void str_to(const std::string& type);
		
		
		/**
		 * create a string with the val value.
		 * @obs: if val is string format, then returned: get<string>(val)
		 * @obs: this function does not change the val, the val format or its value.
		 * @obs: if the val is std::monostate is returned "" - empty string
		 */
		std::string str() const;
				
		/**
		 * set the value of variant type.
		 * example: var["column_name"].set() = "string";
		 * var["column_name"].set() = 3;
		 */
		inline std::variant<D_FIELD_TYPES>& set() { return val; };
		
		////////////////////////////////////////////////////////////////////////////////
		// GET functions
		////////////////////////////////////////////////////////////////////////////////
		/**
		 * return the variable in type of variant.
		 * @obs: for string only: var["column_name"].get() = var["column_name"].str()
		 * example -string: string str = var["column_name"].get() + "test"; or 
		 * string str = var["column_name"].get<std::string>() + "test";
		 * example -int: int i = var["column_name"].geti() + 5; or 
		 * int i = var["column_name"].get<int>() + 5;
		 * example -char: char c = var["column_name"].getc() + 5; or 
		 * if( i == (var["column_name"].get<char>() || 'c')) ...;
		 */
		inline std::string get()  const {
			try{ return std::get<std::string>(val); } catch (const std::exception &e) { throw err(e.what()); }}
		
		inline int geti() const {
			try { return std::get<int>(val); } catch (const std::exception &e) { throw err(e.what()); }}
		
		inline char getc() const { try { return std::get<char>(val); } catch (const std::exception &e){throw err(e.what());}}
		inline long getl() const { try { return std::get<long>(val); } catch (const std::exception &e){throw err(e.what());}}
		inline float getf() const { try { return std::get<float>(val); } catch (const std::exception &e){throw err(e.what());}}
		inline double getd() const{ try{ return std::get<double>(val); } catch (const std::exception &e){throw err(e.what());}}
		
		inline long long getll() const {
			try { return std::get<long long>(val); } catch (const std::exception &e) { throw err(e.what()); }}
		
		inline unsigned long getul() const {
			try { return std::get<unsigned long>(val); } catch (const std::exception &e) { throw err(e.what()); }}
		
		inline long double getld() const {
			try { return std::get<long double>(val); } catch (const std::exception &e) { throw err(e.what()); }}
		
		template<typename T>
		inline T get() const{ try{ return std::get<T>(val); } catch (const std::exception &e) { throw err(e.what()); }}
	};
	
	/**
	 * significa qual tipo de parser será executado
	 * CONCAT: estilo cout
	 * FORMAT: estilo printf - não implementado ainda
	 */
	enum class kind {
		FORMAT, CONCAT
	};
	
	class sql {
	 protected:
		kind _kind;
		std::vector<std::string> statement = {};
		
	 public:
	 	////////////////////////////////////////////////////////////////////////////////
	 	// constructors
	 	////////////////////////////////////////////////////////////////////////////////
	 	/**
	 	 * kind: tipo de sql - por enquanto somente o kind::concat está implementado
	 	 * @important: como funciona o kind::concat:
	 	 * o statement é o sql que será executado em formato bruto, ou seja, o sql
	 	 * com combinado com as chaves do objeto que sofrerão a substituição.
	 	 * a ideia deste tipo de concat é inspirada no cout << do c++.
	 	 * A primeira string do vetor statement, sempre é um sql puro (pedaço dele),
	 	 * e a outra, sempre é uma string que representa uma chave do field,
	 	 * a próxima sempre é um outro pedaço do sql puro, e assim sucessivamente.
	 	 * As possições pares do vetor, sempre serão ocupadas por sql puro
	 	 * e as posições ímpares sempre por uma string que representa uma chave de um campos do objeto.
	 	 * @obs: ao final, automaticamente se coloca o caracter ';', para demarcar o fim.
	 	 * @obs: são aceitos posições com strings vazias, porém, se for um field_key, ele
	 	 * ainda assim, será testado.
	 	 * @exemplo: o std::vector<std::string>& statement = { "SELECT * from COMPANY" }
	 	 * será transformado em: "SELECT * from COMPANY;"
	 	 * @exemplo: o statement = { "SELECT * from ", "table" }
	 	 * e no std::map<std::string, field>& M, contenha um tupla: ["table"] = "COMPANY"
	 	 * será transformado em: "SELECT * from COMPANY;"
	 	 * @obs: sempre antes de escrever no sql final, cada valor de chave, antes é chamado
	 	 * a função: check_write() de field.
	 	 * sempre depois de colocar os valores no campo de field, sempre é executado
	 	 * imediatamente após a função: check_read() de field.
	 	 * @obs: a própria função, já transforma para string as chaves que não são string (int, float, etc..)
	 	 * por meio da função: str() da classe field.
	 	 */
		sql(const kind& _kind, const std::vector<std::string>& statement);
		
		sql(const std::vector<std::string>& statement); // set this->_kind = kind::CONCAT
		
		////////////////////////////////////////////////////////////////////////////////
	 	// public functions
	 	////////////////////////////////////////////////////////////////////////////////
	 	/**
	 	 * T = pqxx::work or pqxx::nontransaction
	 	 * necessary to execute exec0 and exec1
	 	 */
	 	template<class T>
	 	void run0(const std::map<std::string, field>& M, T& W); // roda a query por meio da função pqxx::exec0
		
		template<class T>
	 	void run1(std::map<std::string, field>& M, T& W); // roda a query por meio da função pqxx::exec1
	 	
	 	/**
	 	 * exibe informações da query:
	 	 * tipo do sql: kind
	 	 * imprime o vetor statement, com divisão, entre o que é chave e o que é sql puro.
	 	 */
	 	void print();

	 private:
		////////////////////////////////////////////////////////////////////////////////
	 	// auxiliar functions
	 	////////////////////////////////////////////////////////////////////////////////
	 	std::string make_query(const std::map<std::string, field>& M);
		void copy_result(const pqxx::row& Row, std::map<std::string, field>& M);
		std::string make_query_concat(const std::map<std::string, field>& M);
		std::string make_query_format(const std::map<std::string, field>& M);
	};
	
	/**
	 * Serve para controlar internamente qual tipo de runX (exec, exec1 or exec0) será executado
	 */
	enum class erun {
		run, run0, run1
	};
	
	/**
	 * a ideia é transformar cada linha de uma tabela em um objeto.
	 * pode-se fazer uma "coleção de objetos" que não é nada mais que um vetor de objetos
	 * esta classe representa apenas uma linha de uma tabela
	 */
	class obj {
	 protected:
		std::map<std::string, field> _field = {}; // col = column -> [column_name] = column_value
		std::map<std::string, sql> _sql_real = {};
		/**
		 * Conjunto de sqls fakes ou reais.
		 * cada sql fake, executa na verdade, os sqls cujos estão ele tem guardado no seu vetor.
		 * utilizado para guardar e otimizar um conjunto de operações, agrupando-as todas sob
		 * um mesmo nome e executando elas apenas com uma chamada.
		 * quando um sql fake é chamado, o commit é executado somente após o último sql.
		 * ideal para realizar roolbacks de vários sqls.
		 */
		std::map<std::string, std::vector<std::string>> _sql_fake = {};
		
	  public:
	  	////////////////////////////////////////////////////////////////////////////////
		// constructors
		////////////////////////////////////////////////////////////////////////////////
	  	obj(const std::vector<std::string>& field_key = {},
	  		const std::map<std::string, sql>& sql_real = {},
	  		const std::map<std::string, std::vector<std::string>>& sql_fake = {});
	
		////////////////////////////////////////////////////////////////////////////////
		// public functions - auxiliar functions
		////////////////////////////////////////////////////////////////////////////////  	
		/**
		 * print all values of the class - use printf
		 * - table
		 * - [column_name] = "column_value" - std::map<std::string, std::string> col;
		 */
	  	void print();
	  	
	  	/**
	  	 * overloading para fazer um sintaxe suggar no código.
	  	 * retorna o valor do campo da coluna especificada.
	  	 * caso não exista a coluna na classe, é lançado uma exceção.
	  	 * exemplos:
	  	 * escrita, atribuição do valor: var["column_name"].set() = 3;
	  	 * leitura do valor em uma expressão: int result = 1 + var["column_name"].geti();
	  	 * escrita, atribuição do valor: var["column_name"].set() = "string_test";
	  	 * leitura do valor em uma expressão: string str = "init_str" + var["column_name"].get();
	  	 * escrita, atribuição do valor: var["column_name"].set() = 3.14;
	  	 * leitura do valor em uma expressão: int result = 1 + var["column_name"].geti();
	  	 */
	  	field& operator [] (const std::string& column_name); // write mode
	  	//auto  operator [] (const std::string& column_name); // read mode
	  	
	  	/**
	  	 * Updating the values of map this->col.
	  	 * If exists a key in row that not exists in "this->col" -> throw an exception u::error
	  	 * TODAS as chaves in row devem existir em this->col -> throw an exception u::error
	  	 * obs: row pode ser apenas um subconjunto de this->col, não precisa conter todas as chaves de this->col.
	  	 */
	  	void set(const std::map<std::string, std::variant<D_FIELD_TYPES>>& _field_);
	  	
	  	////////////////////////////////////////////////////////////////////////////////
		// public functions - sql functions
		////////////////////////////////////////////////////////////////////////////////
		/**
		 * O 'n' antes do run, significa que é o tipo de  transação é pqxx::nontransaction
		 * Somente o run, significa que é o tipo de  transação é pqxx::work
		 * O '0' depois do run, significa que é executado exec0 > não tem resposta do banco
		 * O '1' depois do run, significa que é executado o exec1 > a reposta é apenas 1 linha
		 * Caso a resposta não seja do tamanho esperado, levanta uma exceção.
		 */
	  	void run0(const std::vector<std::string>& sql_key,
	  		const std::map<std::string, std::variant<D_FIELD_TYPES>>& _field_ = {});
		void run1(const std::vector<std::string>& sql_key,
	  		const std::map<std::string, std::variant<D_FIELD_TYPES>>& _field_ = {});
	  	void nrun0(const std::vector<std::string>& sql_key,
	  		const std::map<std::string, std::variant<D_FIELD_TYPES>>& _field_ = {});
	  	void nrun1(const std::vector<std::string>& sql_key,
	  		const std::map<std::string, std::variant<D_FIELD_TYPES>>& _field_ = {});
	  	////////////////////////////////////////////////////////////////////////////////
		// private functions
		////////////////////////////////////////////////////////////////////////////////
	  	private:
	  	template<class T>
		void xrunX(const std::vector<std::string>& sql_key, const erun Type, T& W);
		
		/**
		 * verifica se existe chave iguais nos maps: _sql_real and _sql_fake
		 */
		void check_equal_key_sql_real_fake();
	};
	
	
	/**
	 * Agrupa um conjunto de objetos.
	 * Para ser utilizado quando um sql retornar mais de uma resultado.
	 */
	class set {
	 protected:
	 	obj _root;
	 	std::vector<obj> _obj;
		std::map<std::string, std::sql> _sql_real;
		std::map<std::string, std::sql> _sql_fake;
	
	 public:
	 	
	};
	
	////////////////////////////////////////////////////////////////////////////////
	//  error classes
	////////////////////////////////////////////////////////////////////////////////

	namespace error
	{
		// error classes must be defined here
	}

	////////////////////////////////////////////////////////////////////////////////
	//  end namespace cweb
	////////////////////////////////////////////////////////////////////////////////
} // end namespace cweb


////////////////////////////////////////////////////////////////////////////////
// Implementation of templates and inline functions
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// class sql
////////////////////////////////////////////////////////////////////////////////
template<class T>
void d::sql::run0(const std::map<std::string, field>& M, T& W)
{ try {
    W.exec0( make_query(M) );
 } catch (const std::exception &e) { throw err(e.what()); }
}

template<class T>
void d::sql::run1(std::map<std::string, field>& M, T& W)
{ try {
    pqxx::row R{ W.exec1( make_query(M) ) };
    copy_result(R, M);
 } catch (const std::exception &e) { throw err(e.what()); }
}

////////////////////////////////////////////////////////////////////////////////
// class sql
////////////////////////////////////////////////////////////////////////////////
template<class T>
void d::obj::xrunX(const std::vector<std::string>& sql_key, const erun Type, T& W)
{ try {
	for(auto const& key : sql_key)
    {
    	const auto v = _sql_fake.find(key);
    	if(v != _sql_fake.end()) xrunX(v->second, Type, W); // chamada recursiva, para fazer a busca em profundidade.
    	else {
	    	const auto SQL = _sql_real.find(key);
    		if(SQL == _sql_real.end()) {
    			print(); throw err("no found sql_key in field of object. - sql_key: \"%s\"", key); }
    	
    		switch(Type) {
	    		case erun::run0: SQL->second.run0(_field, W); break;
	    		case erun::run1: SQL->second.run1(_field, W); break;
	    		default: throw err("enumeration erun index does not exists: %d", static_cast<int>(Type)); }
    	}
    }
 } catch (const std::exception &e) { throw err(e.what()); }
}


#endif // DATABASEPP_H

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////


































