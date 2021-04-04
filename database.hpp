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
#include <unordered_set>
#include <unordered_map>

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
	/**
	 * Necessário para utilizar a função quote() do postgres.
	 * Somente se consegue acessar ela por meio de uma instância já criada.
	 * para não quebrar a modularidade, nem inserir demasiados parâmetros, foi
	 * criado as funções.
	 * Utilizar essas funções para garantir que o código não quebre ou ocorra
	 *  algum problema.
	 * Ela só funciona no postgres (libpqxx) e também.
	 */
	////////////////////////////////////////////////////////////////////////////////
	//  auxiliar classes - quote
	////////////////////////////////////////////////////////////////////////////////
	extern pqxx::connection_base* ___CB; // inicialization in "obj.cpp", with value nullptr
	
	template<class T>
	void inline init_quote(const T& t) { ___CB = (pqxx::connection_base*)&t; }
	
	template<class T>
	void inline commit(T& t) {
		try{ t.commit(); ___CB = nullptr; } catch (const std::exception &e) { throw err(e.what()); }}
	
	std::string inline quote(const std::string& s) {
		try{ return ___CB->quote(s); } catch (const std::exception &e) { throw err(e.what()); }}
	
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
	 * muitos ainda não foram implementados - verificar
	 * @import: quote: by default is always executed in check_write(), for not executed quote use: no_quote
	 * se o tipo não for string, no momento da executação do quote, a função check_write() levanta uma exceção.
	 * a função check_write() - sempre executa o quote como última opção de todas as opções escolhidas.
	 * a função check_read() - por default não executa o quote.
	 * o quote é executado pela função d::quote() - acima codificada.
	 * @implemented check_write: quote notnull
	 * @implemented check_read: null0 notnull
	 */
	enum class fopt { // options to field
		notnull, //primary_key, foreign_key, // column options
		trim, rtrim, ltrim, upper, lower, // string options
		null0, // numeric options: null0 > replace null or empty string with 0
		in, not_in, // lists or vectors that value must be one element or not be
		quote, no_quote // execute quote - always in check_write() - is the last option to be executed
	};
	
	class field {
	 protected:
		std::variant<D_FIELD_TYPES> val = "";
		field_type def = field_type::STR; // tipo de construção | usado para escolher qual será o padrão de conversão no select do sql, para automatizar e também deixar mais legível qual é o tipo deste campo
		std::unordered_set<std::string> _name = {}; //column name of field or others alias to identify this field in sql statement
	 	std::vector<fopt> _opt = {}; // options of field
	 public:
	 	////////////////////////////////////////////////////////////////////////////////
		// public functions - constructor
		////////////////////////////////////////////////////////////////////////////////
		field() {}
	 	/**
	 	 * somente a parte do const std::unordered_set<std::string>& name = {} está implementada
	 	 * falta implementar transformar as strings de opt em opt.
	 	 */
	 	field(const std::vector<std::string>& opt, const std::unordered_set<std::string>& name = {});
	 	
	 	//field(const std::vector<fopt>& opt, const std::unordered_set<std::string>& name);
	 	
	 	////////////////////////////////////////////////////////////////////////////////
		// public functions
		////////////////////////////////////////////////////////////////////////////////
		/**
		 * Inicializa o vetor _opt com as opções existentes no vector.
		 * Caso _opt tenha alguma opção que não esteja em VOpt, esta opção será apagada.
		 */
		void init_opt(const std::vector<std::string>& VOpt);
		
	 	// return the conteiner that get the names of this field
	 	inline std::unordered_set<std::string>& name() { return _name; }
	 	
	 	// return the conteiner that get the names of this field
	 	inline std::vector<fopt>& opt() { return _opt; }
	 	
	 	/**
	 	 * print the options of field
	 	 */
	 	void print_opt() const;
	 	
	 	/**
	 	 * print the name of fields
	 	 */
	 	void print_name() const;
	 	
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
		 * Check and trata the values to write in database.
		 * Check and trata the values to read from database.
		 */
		void check_write();
		void check_read();
		
		////////////////////////////////////////////////////////////////////////////////
		// check functions
		////////////////////////////////////////////////////////////////////////////////
		void inline check_notnull() { 
			try { if( str().empty() )
					throw err("Value of field is null or empty string. "
						"Forbidden by \"notnull\" field option."); }
			catch (const std::exception &e) { throw err(e.what()); } }
		
		void inline check_null0() { try { if( str().empty() ) set() = "0"; }
			catch (const std::exception &e) { throw err(e.what()); } }
		
		
		////////////////////////////////////////////////////////////////////////////////
		// check functions END
		////////////////////////////////////////////////////////////////////////////////
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
	
	class obj; // fowarding definition
	
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
	 	 * e a outra, sempre é uma string que representa uma chave do field ou um valor contido na estrutura field.name(),
	 	 * a próxima sempre é um outro pedaço do sql puro, e assim sucessivamente.
	 	 * As possições pares do vetor, sempre serão ocupadas por sql puro
	 	 * e as posições ímpares sempre por uma string que representa uma chave de um campo do objeto.
	 	 * @obs: a função verifica todas as chaves dos fields, e somente caso não encontrado, é que ela começa a verficiar
	 	 * pelos valores em field.name(). Caso uma chave de um field e um dos valores de field.name(), que pode conter vários,
	 	 * forem iguais, sempre será substituído pela chave do map (chave do map que aponta para o field), pois é buscado
	 	 * nele primeiro.
	 	 * @obs: ao final, automaticamente se coloca o caracter ';', para demarcar o fim.
	 	 * @obs: são aceitos posições com strings vazias, porém, se for um field_key, ele
	 	 * ainda assim, será testado.
	 	 * @exemplo: o std::vector<std::string>& statement = { "SELECT * from COMPANY" }
	 	 * será transformado em: "SELECT * from COMPANY;"
	 	 * @exemplo: o statement = { "SELECT * from ", "table" }
	 	 * e no std::unordered_map<std::string, field>& M, contenha um tupla: ["table"] = "COMPANY" ou ["table"].name() = { "COMPANY" }
	 	 * será transformado em: "SELECT * from COMPANY;"
	 	 * @obs: sempre antes de escrever no sql final, cada valor de chave, antes é chamado
	 	 * a função: check_write() de field.
	 	 * sempre depois de colocar os valores no campo de field, sempre é executado
	 	 * imediatamente após a função: check_read() de field.
	 	 * @obs: a própria função, já transforma para string as chaves que não são string (int, float, etc..)
	 	 * por meio da função: str() da classe field antes da escrita.
	 	 * @obs: as funções são sempre escritas em field.set(), como string: F.set() = R.is_null() ? "" : R.as<std::string>();
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
	 	void run0(const obj& O, T& W); // roda a query por meio da função pqxx::exec0
	 	
	 	template<class T>
		void run0(const std::vector<obj>& V, T& W);
		
		template<class T>
	 	void run1(obj& O, T& W); // roda a query por meio da função pqxx::exec1
	 	
	 	template<class T>
		void run1(std::vector<obj>& V, T& W);
	 	
	 	/**
	 	 * exibe informações da query:
	 	 * tipo do sql: kind
	 	 * imprime o vetor statement, com divisão, entre o que é chave e o que é sql puro.
	 	 */
	 	void print();
	 	
	 	/**
	 	 * imprime todas as chaves do map e todos os nomes de cada field - map<std::string e field.name()
	 	 * realiza o procedimento acima para todos os elementos do vetor.
	 	 * dado uma entrada, esta função imprime o que o sql entende que são as possíveis chaves para o statement
	 	 */
	 	void print_key(const std::vector<obj>& V);
	 	
	 	/**
	 	 * Retorna a query já pronta do sql para ser executada.
	 	 * Pega a variável statement (array de strings que são textos puros de sql + field_key (or field.name())
	 	 * e transforma este vetor em uma query.
	 	 * substitui os valores de field_key (or field.name()) pelos correspondentes existente no vetor V passado.
	 	 * @return: a string de retorno pode já ser executada: ex: W.exec(sql.make_query(V));
	 	 */
	 	std::string make_query(const std::vector<obj>& V);
	 	
	 	void copy_result(const pqxx::row& Row, std::vector<obj>& V);

	 private:
		////////////////////////////////////////////////////////////////////////////////
	 	// auxiliar functions
	 	////////////////////////////////////////////////////////////////////////////////
		field& get_field(const std::string& key, 
			const std::vector<obj>& V, const size_t idx = 0);
		field& get_field_by_name(const std::string& key,
			const std::vector<obj>& V, const size_t idx = 0);
		
		std::string make_query_concat(const std::vector<obj>& V);
		std::string make_query_format(const std::vector<obj>& V);
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
		std::unordered_map<std::string, field> _field = {}; // col = column -> [column_name] = column_value
		std::unordered_set<std::string> _primary_key = {};
		std::unordered_map<std::string, sql> _sql_real = {};
		/**
		 * Conjunto de sqls fakes ou reais.
		 * cada sql fake, executa na verdade, os sqls cujos estão ele tem guardado no seu vetor.
		 * utilizado para guardar e otimizar um conjunto de operações, agrupando-as todas sob
		 * um mesmo nome e executando elas apenas com uma chamada.
		 * quando um sql fake é chamado, o commit é executado somente após o último sql.
		 * ideal para realizar roolbacks de vários sqls.
		 */
		std::unordered_map<std::string, std::vector<std::string>> _sql_fake = {};
		
	  public:
	  	////////////////////////////////////////////////////////////////////////////////
		// constructors
		////////////////////////////////////////////////////////////////////////////////
		obj() {}
		
	  	obj(const std::vector<std::string>& field_key,
		  	const std::unordered_set<std::string>& primary_key = {},
	  		const std::unordered_map<std::string, sql>& sql_real = {},
	  		const std::unordered_map<std::string, std::vector<std::string>>& sql_fake = {});
	  	
	  	obj(const std::unordered_map<std::string, field>& _field,
	  		const std::unordered_set<std::string>& primary_key = {},
	  		const std::unordered_map<std::string, sql>& sql_real = {},
	  		const std::unordered_map<std::string, std::vector<std::string>>& sql_fake = {});
	
		////////////////////////////////////////////////////////////////////////////////
		// public functions - auxiliar functions
		////////////////////////////////////////////////////////////////////////////////
		inline std::unordered_set<std::string>& primary_key() { return _primary_key; };
		
		/**
		 * print all information about the field in the class.
		 * print all values of all fields in class (key, value, type, options, names)
		 */
	  	void print() const;
	  	
	  	/**
	  	 * print values
	  	 * print only the values of field in class.
	  	 * this functions is for print the values of table
	  	 * @arg: msg_init -> string que aparecerá no começo da linha em que se imprimirão os valores
	  	 */
	  	void printv(const std::string msg_init) const;
	  	
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
	  	field& operator [] (const std::string& column_name);   // write mode
	  	const field& operator[](const std::string& key) const; // read mode
	  	
	  	inline const bool empty() const { return _field.empty(); }
	  	
	  inline virtual const size_t size()  const {
			try{ return _field.size(); } catch (const std::exception &e) { throw err(e.what()); }}
		//try{ throw err("me*********"); } catch (const std::exception &e) { throw err(e.what()); }}
	  	
	  	/**
	  	 * Updating the values of map this->col.
	  	 * If exists a key in row that not exists in "this->col" -> throw an exception u::error
	  	 * TODAS as chaves in row devem existir em this->col -> throw an exception u::error
	  	 * obs: row pode ser apenas um subconjunto de this->col, não precisa conter todas as chaves de this->col.
	  	 */
	  	void set(const std::unordered_map<std::string, std::variant<D_FIELD_TYPES>>& _field_);
	  	
	  	/**
	  	 * Recuperar um elemento da estrutura _field, sem a necessidade de visualizar a estrutura armazenada
	  	 * Para maior modularidade, sempre usar: auto X = my_obj.find(field_key);
	  	 * Deve enviar o interator, para o usuário testar se realmente existe a chave.
	  	 * Se utilizar o operator[] em caso de falha throw error(), neste caso deve-se fazer um try-catch
	  	 * O interator já é padrão do c++, então facilita a leitura e entendimento do código
	  	 */
	  	inline  std::unordered_map<std::string, field>::iterator find(const std::string& field_key) {
	  		try { return _field.find(field_key); } catch (const std::exception &e) { throw err(e.what()); }}
	  	
	  	inline  std::unordered_map<std::string, field>::const_iterator find(const std::string& field_key) const {
	  		try { return _field.find(field_key); } catch (const std::exception &e) { throw err(e.what()); }}
	  	////////////////////////////////////////////////////////////////////////////////
		// public functions - for range interators - for work with for range loop
		////////////////////////////////////////////////////////////////////////////////
		/**
		 * This code is for use:
		 * obj O
		 * for(auto const& it : O)
		 * where it is the one element of std::unordered_map<std::string, field> _field
		 * for(auto const& it : O) = for(auto const& it : O._field)
		 */
		 inline std::unordered_map<std::string, field>::iterator begin(){
        	return _field.begin();
		 }
	     inline std::unordered_map<std::string, field>::iterator end(){
    	    return _field.end();
    	 }
    	 inline std::unordered_map<std::string, field>::const_iterator begin() const {
    	    return _field.begin();
    	 }
    	 inline std::unordered_map<std::string, field>::const_iterator end() const {
    	    return _field.end();
    	 }
    	 /**
    	  * Essas duas funções abaixo são importantes, para o interador, entretando
    	  * eu já tenho feito elas acima, pois adicionei checks para elas.
    	  * deixei elas pois são do código original que eu copei na internet
    	  * e caso queira reusar o original tenho elas
    	  *
    	 inline const int& operator[](const std::string& key) const {
    	    return _field.at(key);
    	 }
    	 inline int& operator[](const std::string& key) {
    	    return _field[key];
    	 }*/
	  	
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
	  		const std::unordered_map<std::string, std::variant<D_FIELD_TYPES>>& _field_ = {});
		void run1(const std::vector<std::string>& sql_key,
	  		const std::unordered_map<std::string, std::variant<D_FIELD_TYPES>>& _field_ = {});
	  	void nrun0(const std::vector<std::string>& sql_key,
	  		const std::unordered_map<std::string, std::variant<D_FIELD_TYPES>>& _field_ = {});
	  	void nrun1(const std::vector<std::string>& sql_key,
	  		const std::unordered_map<std::string, std::variant<D_FIELD_TYPES>>& _field_ = {});
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
	 class table {
	  protected:
	  	obj _model = {};
	  	std::vector<obj> _obj = {};
	  	/**
	  	 * Realiza 
	  	 */
	  	//std::unordered_map<std::string, std::unordered_multimap<std::string, int>> _primary_key;
		
	  public:
	  	////////////////////////////////////////////////////////////////////////////////
		// constructors
		////////////////////////////////////////////////////////////////////////////////
		table() {}
	  	table(const obj& Obj);
	
		////////////////////////////////////////////////////////////////////////////////
		// public functions - auxiliar functions
		////////////////////////////////////////////////////////////////////////////////  	
		inline virtual const size_t size()  const {
			try{ return _obj.size(); } catch (const std::exception &e) { throw err(e.what()); }}
		
		inline virtual void clear()  {
			try{ return _obj.clear(); } catch (const std::exception &e) { throw err(e.what()); }}
				
		inline virtual obj& move(const obj& Obj) {
			try { _obj.push_back(std::move(Obj)); return _obj.back(); } catch(const std::exception &e) { throw err(e.what()); }}
		
		virtual obj& move(const obj& Obj, const size_t idx);
		
		// print the table - head (_model object) and the body (values of _obj)
		virtual void print() const;
		
		////////////////////////////////////////////////////////////////////////////////
		// public functions - sql / run functions
		////////////////////////////////////////////////////////////////////////////////
		/**
		 * 
		 */
		template<class T>
		void refresh(T& Transaction, const sql& Query, 
					 const std::vector<obj>& VArgQuery, const obj& Model = {});
		//void update();
		//void upgrade();
		
		////////////////////////////////////////////////////////////////////////////////
		// public functions - overloading operators
		////////////////////////////////////////////////////////////////////////////////
		virtual obj& operator[] (const size_t idx);
		virtual const obj& operator[] (const size_t idx) const;
		
		////////////////////////////////////////////////////////////////////////////////
		// public functions - for range interators - for work with for range loop
		////////////////////////////////////////////////////////////////////////////////
		/**
		 * This code is for use:
		 * table T
		 * for(auto const& it : T)
		 * where it is the one element of vector<obj> _obj
		 * for(auto const& it : T) = for(auto const& it : T._obj)
		 */
		inline virtual std::vector<obj>::iterator begin(){
        	return _obj.begin();
    	}
	    inline virtual std::vector<obj>::iterator end(){
    		return _obj.end();
    	}
    	inline virtual std::vector<obj>::const_iterator begin() const {
    		return _obj.begin();
    	}
    	inline virtual std::vector<obj>::const_iterator end() const {
    	    return _obj.end();
    	}
    	
    	////////////////////////////////////////////////////////////////////////////////
		// private functions - auxiliar functions
		////////////////////////////////////////////////////////////////////////////////
		private:
    	void check_model(const obj& Model);
    	void copy_result(const pqxx::result& T, const sql& Query);
	 };
	
	////////////////////////////////////////////////////////////////////////////////
	//  error classes
	////////////////////////////////////////////////////////////////////////////////

	namespace error
	{
		// error classes must be defined here
	}

	////////////////////////////////////////////////////////////////////////////////
	//  end namespace database d::
	////////////////////////////////////////////////////////////////////////////////
} // end namespace database d::


////////////////////////////////////////////////////////////////////////////////
// Implementation of templates and inline functions
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// class sql
////////////////////////////////////////////////////////////////////////////////
template<class T>
void d::sql::run0(const obj& O, T& W)
{ try {
    W.exec0( make_query( { O } ) );
 } catch (const std::exception &e) { throw err(e.what()); }
}

template<class T>
void d::sql::run0(const std::vector<obj>& V, T& W)
{ try {
    W.exec0( make_query(V) );
 } catch (const std::exception &e) { throw err(e.what()); }
}

template<class T>
void d::sql::run1(obj& O, T& W)
{ try {
    pqxx::row R{ W.exec1( make_query( {O} ) ) };
    std::vector<obj> V = {};
    V.push_back(std::move(O));
    copy_result(R, V);
    O = std::move(V[0]);
    //copy_result(R, { std::forward(M) }); // TODO - erro - fazer esse código funcionar
 } catch (const std::exception &e) { throw err(e.what()); }
}

template<class T>
void d::sql::run1(std::vector<obj>& V, T& W)
{ try {
    pqxx::row R{ W.exec1( make_query(V) ) };
    copy_result(R, V);
 } catch (const std::exception &e) { throw err(e.what()); }
}

////////////////////////////////////////////////////////////////////////////////
// class obj
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
	    		case erun::run0: SQL->second.run0(*this, W); break;
	    		case erun::run1: SQL->second.run1(*this, W); break;
	    		default: throw err("enumeration erun index does not exists: %d", static_cast<int>(Type)); }
    	}
    }
 } catch (const std::exception &e) { throw err(e.what()); }
}

////////////////////////////////////////////////////////////////////////////////
// class table
////////////////////////////////////////////////////////////////////////////////
template<class T>
void d::table::refresh(T& Transaction, const sql& Query, 
	const std::vector<obj>& VArgQuery, const obj& Model)
{ try {
	check_model(Model);
	init_quote(Transaction);
	const std::string query = const_cast<sql&>(Query).make_query(VArgQuery);
	//std::printf("QUERY = \"%s\"\n", query.c_str());
	pqxx::result R( Transaction.exec( query ));
	copy_result(R, Query);
 } catch (const std::exception &e) { throw err(e.what()); }
}
#endif // DATABASEPP_H

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////


































