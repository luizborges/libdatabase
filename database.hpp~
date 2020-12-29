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
#include <map>
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
#define D_FIELD_TYPES std::monostate, std::string, bool, char, int, long, long long, \
	unsigned char, unsigned int, unsigned long, unsigned long long

////////////////////////////////////////////////////////////////////////////////
// namespace
////////////////////////////////////////////////////////////////////////////////
namespace d 
{

	//extern void config(const std::vector<std::string>&& conf);

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
		NO_TYPE, STR, BOOL, CHAR, INT, LONG, LONG_LONG,
	 	UCHAR, UINT, ULONG, ULONG_LONG
	};
	
	enum class field_key {
		no_key, primary_key, foreign_key
	};
		
	struct field {
		std::variant<D_FIELD_TYPES> val;
	 	
		bool notNull = false;
	 	field_key key = no_key;
	 	
	 	/**
	 	 * return a string that represents the index.
	 	 * ex: index = 1 -> returned: "std::string"
	 	 */
		std::string type();
		
		/**
		 * val.index()
		 */
		inline int index() { return val.index(); }
		
		/**
		 * return the variable in type of variant.
		 * ex: if val is int - the returned is a int variable with the value of val
		 * if val is string - the returned is a string variable with the value of val
		 */
		auto get();
		
		/**
		 * set the value of variant type.
		 */
		inline void set(auto& val) { this->val = val; };
		
		std::string key_str();
	};
	
	
	/**
	 * a ideia é transformar cada linha de uma tabela em um objeto.
	 * pode-se fazer uma "coleção de objetos" que não é nada mais que um vetor de objetos
	 * esta classe representa apenas uma linha de uma tabela
	 * 
	 */
	class obj
	{ protected:
	
		std::string table;
		std::map<std::string, item> col; // col = column -> [column_name] = column_value
		
	  public:
	  	////////////////////////////////////////////////////////////////////////////////
		// constructors
		////////////////////////////////////////////////////////////////////////////////
	  	obj(const std::string& table, const std::vector<std::string>& column_name);
	
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
	  	 * como o tipo do campo é um std::variant - o valor para leitura varia
	  	 * para uma maior flexibilidade, se utilizou auto&, para escrita, porém o tipo de retorno é sempre std::variant
	  	 * verifica se existe a coluna na table, e retorna a coluna + o item dela.
	  	 * caso não exista, levanta uma exeção e imprime o nome da coluna e todo os valores da tabela mais o nome dela.
	  	 */
	  	auto& operator [] (const std::string& column_name); // write mode
	  	auto  operator [] (const std::string& column_name); // read mode
	  	
	  	////////////////////////////////////////////////////////////////////////////////
		// public functions - sql functions
		////////////////////////////////////////////////////////////////////////////////
	  	/**
	  	 * performs insert a operation.
	  	 * if a value is a empty string, this value is not put inside to sql statement.
	  	 * @arg row: insert the row in table.
	  	 * a linha deve ser igual ou um subconjunto (column_name) do map this->col da classe.
	  	 * caso haja alguma chave que exista na row e não em this->col, é lançado uma exceção.
	  	 * caso haja uma ou mais chaves no this->col e não no row, não é lançado exceção.
	  	 * os valores de this->col são atualizados, em que as chaves forem as mesmas, para ficarem iguais aos de row.
	  	 * @obs: if a value is a empty string = null and:
	  	 * item is isPrimaryKey = true => throw an error
	  	 * item is notNull = true => throw an error
	  	 */
	  	void insert(const std::map<std::string, std::variant<D_FIELD_TYPES>>& row = {});
	  	
	  	
	  	/*void select(
			const std::map<std::string, std::variant<D_FIELD_TYPES>>& row = {},
			const std::vector<std::string>& result_column = {},
			const std::vector<std::string>& );*/
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


































