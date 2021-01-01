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
	
	enum class fopt { // options to field
		notnull, primary_key, foreign_key, // column options
		trim, rtrim, ltrim, upper, lower, // string options
		in, not_in, //
		quote // execute quote - always is the last option to be executed
	};
	
	/**
	 * O objetivo desta classe é ser usada de parâmetro para configurar quais serão os resultados dos campos do
	 * resultado de uma query, ou seja, quais são os tipos esperados e que serão convertidos para a classe field.
	 * ou seja, é utilizado para guardar qual será o tipo do valor de uma instância da classe field.
	 * é utilizado como um sintaxe suggar e para deixar mais genérico, os parâmetros passados para as funções de sql
	 * da classe obj, que retornam um resultado (ex: select()), para mostrar qual é o tipo de dados esperado que
	 * deverá o resultado ser convertido para ser guardado por uma instância da classe field.
	 */
	class field_query_result {
	 private:
		std::string column_name = ""; //
		field_type  _etype = field_type::STR; ////std::string type = "string";// int index = std::variant<..>.index()
	 public:
	 	// constructos of class
		field_query_result(const std::string& column_name, const field_type& type = field_type::STR);
		field_query_result(const std::string& column_name, const std::string& type);
		field_query_result(const std::string& column_name, const int type);
		
		inline field_type etype() const { return _etype; }
		std::string type() const;
		inline int index() const { return static_cast<int>(_etype); } // compatibilidade com std::variant
		inline std::string& get_column_name() { return column_name; }
		
		/*inline void set_column_name(const std::string& column_name)  const {
		 try{ if(column_name.empty()) throw err("DATABASE: column name cannot be an empty string");
		 	  this->column_name = column_name;
		 } catch (const std::exception &e) { throw err(e.what()); }} */
	};
	
	class field {
	 private:
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
	
	enum class kind {
		NONE, INSERT, UPDATE, SELECT, DELETE, CONCATENATION, CONCATENATION_OUT
	};
	
	enum class sql_stype {
		NOT_TYPE, STR, SQL_ARG
	};
	
	enum class sql_earg {
		value, name
	};
	
	class sql_arg {
	 private:
		std::string _field_key = "";
		std::string _field_name = "";
		sql_earg _type = sql_earg::value;
	 public:
		sql_arg(const std::string& field_key) : _field_key(field_key)
		{ if(field_key.empty()) throw err("field key cannot be an empty string."); }
		
		sql_arg(const std::string& field_key, const std::string& field_name) :
			_field_key(field_key), _field_name(field_name), _type(sql_earg::name) {
			if(field_key.empty()) throw err("field key cannot be an empty string.");
			if(field_name.empty()) throw err("field name cannot be an empty string in this constructor.");
		}
		
		inline std::string& key()  { return _field_key; }
		inline std::string& name() { return _field_name; }
		inline sql_earg&    type() { return _type; }
	};
	
	class sql_result_arg {
		private:
			std::string _field_key = "";
			std::string _null = "";
		public:
			sql_result_arg(const std::string& field_key) : _field_key(field_key)
			{ if(field_key.empty()) throw err("field key cannot be an empty string."); }
				
			sql_result_arg(const std::string& field_key, const std::string& _null) 
				: _field_key(field_key), _null(_null)
			{	if(field_key.empty()) throw err("field key cannot be an empty string."); }
			
			inline std::string& key()  { return _field_key; }
			inline std::string& null() { return _null; }
	};
	
	class sql {
	 private:
		kind Kind = kind::NONE;
		
		std::vector<std::variant<std::monostate, std::string, sql_arg>> statement = {};
		std::vector<sql_result_arg> result_arg = {};
		
	 public:
	 	////////////////////////////////////////////////////////////////////////////////
	 	// constructors
	 	////////////////////////////////////////////////////////////////////////////////
	 	/**
	 	 * The order of arguments in string statement has to be the same order in sql_arg
	 	 * exemples:
	 	 * sql_statement = "SELECT nome FROM pessoa WHERE id = %s"
	 	 * sql_arg = { {"id"} } - sql_arg.type() = field_value
	 	 * sql_statement = "SELECT nome FROM pessoa WHERE %s = %s"
	 	 * sql_arg = { {"id", "id"}, {"id"} } - sql_arg.type() = {field_name, field_value}
	 	 * sql_statement = "SELECT nome FROM pessoa WHERE id = %s"
	 	 * sql_arg = { {""} } - sql_arg.type() = no_field
	 	 */
		sql(const kind& _kind,
			const std::vector<std::variant<std::monostate, std::string, sql_arg>>& statement,
			const std::vector<sql_result_arg>& result_arg = {});
		
		////////////////////////////////////////////////////////////////////////////////
	 	// run functions
	 	////////////////////////////////////////////////////////////////////////////////
	 	void run0(
			const std::map<std::string, field>& m1, const std::map<std::string, field>& m2);

	 	std::map<std::string, std::string> run1(
	 		std::map<std::string, field>& main, const std::map<std::string, field>& m2);

	 private:
		////////////////////////////////////////////////////////////////////////////////
	 	// auxiliar functions
	 	////////////////////////////////////////////////////////////////////////////////
	 	std::string make_query(
			const std::map<std::string, field>& m1, const std::map<std::string, field>& m2);
		
		std::map<std::string, std::string>
			copy_result(const pqxx::row& From, std::map<std::string, field>& dest);
		
		std::string make_query_concatenation(const std::map<std::string, field>& map);
		
		std::string make_query_concatenation_out(
		const std::map<std::string, field>& m1, const std::map<std::string, field>& m2);
	
		std::map<std::string, std::string> copy_result_concatenation_out(
			const pqxx::row& From, std::map<std::string, field>& dest);
	};
	
	/**
	 * a ideia é transformar cada linha de uma tabela em um objeto.
	 * pode-se fazer uma "coleção de objetos" que não é nada mais que um vetor de objetos
	 * esta classe representa apenas uma linha de uma tabela
	 * 
	 */
	class obj {
	 protected:
		std::map<std::string, field> _field; // col = column -> [column_name] = column_value
		std::map<std::string, sql> _sql;
		
	  public:
	  	////////////////////////////////////////////////////////////////////////////////
		// constructors
		////////////////////////////////////////////////////////////////////////////////
	  	obj(const std::vector<std::string>& field_key);
	
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
	  	void set(const std::map<std::string, std::variant<D_FIELD_TYPES>>& row);
	  	
	  	////////////////////////////////////////////////////////////////////////////////
		// public functions - sql functions
		////////////////////////////////////////////////////////////////////////////////
	  	/**
	  	 * performs insert a operation.
	  	 * if a value is a empty string, this value is not put inside to sql statement.
	  	 * @arg row: change the values of obj (this->col)
	  	 * a linha deve ser igual ou um subconjunto (column_name) do map this->col da classe.
	  	 * caso haja alguma chave que exista na row e não em this->col, é lançado uma exceção.
	  	 * caso haja uma ou mais chaves no this->col e não no row, não é lançado exceção.
	  	 * os valores de this->col são atualizados, em que as chaves forem as mesmas, para ficarem iguais aos de row.
	  	 * @obs: if a value is a empty string or this->col[].etype() = filed_type::NO_TYPE and:
	  	 * item is isPrimaryKey = true => throw an error
	  	 * item is notNull = true => throw an error
	  	 */
	  	//void insert(const std::map<std::string, std::variant<D_FIELD_TYPES>>& row = {});
	  	
	  	////////////////////////////////////////////////////////////////////////////////
		// select functions
		////////////////////////////////////////////////////////////////////////////////
		/**
		 * performs the select sql.
		 * must return excately one line -> otherwise throw error
		 * o row atualiza os valores de this->col, antes da execução do select.
		 * todas as chaves de row devem conter em this->col, se não ocorre erro, o contrário
		 * não é verdadeiro:
		 * os resultados do select serão colocados em ordem nos campos de this->col,
		 * indicados no vector select_column.
		 * A função verifica qual é menor, resultado do select ou select_column, e
		 * apenas insere no this->col, o menor resultado.
		 * exemplo: caso o resulado do select tenha apenas 2 campos, e o vector select_column tenha 3 de tamanho,
		 * será atualizdo apenas as duas primeiras chaves em this->col, indicados no vector select_column
		 * que contem os dois primeiros campos do select.
		 *
	  	void select(
	  		const std::string& sql_statement,
	  		const std::vector<field_query_result>& select_column = {},
	  		const std::map<std::string, std::variant<D_FIELD_TYPES>>& row = {});
	  	
	  	void select(
			const std::map<std::string, std::variant<D_FIELD_TYPES>>& row = {},
			const std::vector<std::string>& where_column = {},
			const std::vector<std::string>& select_column = {},
			const std::string& optional_append_in_where_statement = "",
			const std::string& optional_append_in_select_statement = "");
		
		
		void query(
	  		const std::string& sql_statement,
	  		const std::vector<field_query_result>& select_column = {},
	  		const std::map<std::string, std::variant<D_FIELD_TYPES>>& row = {}); */
	  	////////////////////////////////////////////////////////////////////////////////
		// private functions
		////////////////////////////////////////////////////////////////////////////////
	  	private:
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

#endif // DATABASEPP_H

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////


































