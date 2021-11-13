/**
 *
 * @descripion: Contém o namespace e os templates das funções e classes do namespace database_obj_str.
 * Este namespace é desenhado como uma forma de representar as tabelas e objetos do banco de dados na aplicação C++.
 * Promove uma série de funções que visam a facilitação da comunicação e manipulação das funções.
 */
#ifndef DATABASE_OBJ_STR_HPP
#define DATABASE_OBJ_STR_HPP

/**
 * Este namespace cria um objeto para automatizar a interação com o banco de dados
 * Os valores (fields) das colunas do banco de dados são convertidos para std::string
 * @obs: O valor NULL é transformado automaticamente para uma string vazia.
 * @obs: A string vazia é tratada como NULL.
 */
namespace database_obj_str
{
    ////////////////////////////////////////////////////////////////////////////////
    // enum
    ////////////////////////////////////////////////////////////////////////////////
    enum class Constraint {
        none, // nenhuma constraint
        primary_key,
        unique
    };

    ////////////////////////////////////////////////////////////////////////////////
    // class
    ////////////////////////////////////////////////////////////////////////////////
    class Field {
     private:
        Constraint constraint = Constraint::none;
        bool not_null;
        bool quote; // se deve realizar a função quote() no valor antes de ser passado para o sql
        std::string table_name; // guardar o nome para fins de log e msgs de erro e verificaçẽos
        std::string column_name;  // guardar o nome para fins de log, msgs de erro e verificações
        std::string type;
        std::string data;

     public:

        Field(const std::string& table_name, const std::string& column_name, 
            const std::string& type = "character varying",
            const bool not_null = false, 
            const Constraint constraint = Constraint::none);
        
        Field(const std::string& table_name, const std::string& column_name, 
            const std::string& type, const std::string& is_nullable);

        /**
         * @brief Set the constraint object
         * Seta qual é o tipo de constraint que a coluna tem.
         * São suportados dois tipos: "UNIQUE" e "PRIMARY KEY".
         * Qualquer outro tipo de constraint é lançado uma exceção.
         * @param constraint: nome da constraint do objeto.
         */
        void set_constraint(const std::string& constraint);

        /**
         * @brief Print the values and status of object.
         * The print is in std::cerr.
         * The same print is returned in a string.
         * @return std::string 
         */
        std::string print() const;

        /**
         * @brief Same as print(), 
         * but only print the output in the returned string.
         * @return std::string 
         */
        std::string sprint() const;

        /**
         * @brief Set the data object
         * 
         * @param data 
         */
        void set_data(const std::string& data);

        /**
         * @brief syntatic suggar for setting the data on object.
         * 
         * @param data 
         */
        void operator=(const std::string& data)
        { try {
            set_data(data);
         } catch (const std::exception &e) { throw err(e.what()); }
        }

        /**
         * @brief Get the copy of data object
         * 
         * @return std::string 
         */
        std::string get_data() const {
            return this->data;
        };

        /**
         * @brief Se: a variável bool quote = true -> database::quote(this->get_data())
         * se não: return this->get_data();
         * @return std::string: retorna o dado do campos, condicionalmente se é um campo
         * que deve ter seu valor em quote() no sql ou não.
         */
        std::string get_data_quote() const
        { try {
            return quote ? database::quote(data) : data;
         } catch (const std::exception &e) { throw err(e.what()); }
        }

        /**
         * @brief Get the table name object
         * Return the name of table.
         * @return std::string 
         */
        std::string get_table_name() const {
            return table_name;
        }

        /**
         * @brief Get the column name object
         * O nome da coluna que este campo representa na Tabela do Banco de dados.
         * @return std::string 
         */
        std::string get_column_name() const {
            return column_name;
        }

        /**
         * @brief Verifica se o campo é uma chave primária.
         * @return true: se o campo for uma chave primária -> this->constraint == Constraint::primary_key
         * @return false: otherwise. 
         */
        bool is_primary_key() const
        { try {
            if(this->constraint == Constraint::primary_key) return true;
            else return false;
         } catch (const std::exception &e) { throw err(e.what()); }
        }

        /**
         * @brief Retorna se o campo é NOT NULL.
         * 
         * @return true : se o campo é NOT NULL.
         * @return false: se o campo não é NOT NULL.
         */
        bool is_not_null() const {
            return this->not_null;
        }
    };

    ////////////////////////////////////////////////////////////////////////////////
    // variables
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // functions
    ////////////////////////////////////////////////////////////////////////////////
    /**
     * Dado um tipo do banco de dados postgres, retorna se é necessário
     * fazer o quote() do tipo, antes de inserir uma variável deste tipo na string do sql.
     */
    bool is_quote(const std::string& type);

    /**
     * @brief Transforma o valor da constraint em string.
     * 
     * @param constraint 
     * @return std::string 
     */
    std::string to_str(const Constraint constraint);

    /**
     * @brief Transforma uma string no padrão da constraint, para uma constraint.
     * São os valores de string aceitas:
     * none = "NONE" or "NO CONSTRAINT"
     * primary_key = "PRIMARY KEY"
     * unique = "UNIQUE"
     * @param constraint 
     * @return Constraint 
     */
    Constraint to_constraint(const std::string& constraint);

    /**
     * @brief Mapeia a tabela do banco de dados para um map.
     * Cada campo é do tipo Field.
     * O @arg(MAP_T) deve ser uma estrutura map. ex: std::map e std::unordered_map.
     * É necessário que a estrutura @arg(MAP_T) tenha os seguintes atributos:
     * 1. poder ser interada pelo for_range do tipo C++17 -> 'for(const auto& [key, field] : table)'
     * 2. ter a função 'at(KEY)' em que retorna o elemento do map cuja chave é KEY.
     * 3. poder inserir elementos no map, com a função: 'emplace(KEY, VALUE)' -> onde, KEY é a chave e VALUE é o valor da chave.
     */
    template<template<typename,typename> typename MAP_T>
    MAP_T<std::string, Field> map_table(const std::string& table_name, const std::string &database_connection = database::database_connection);

    /**
     * @brief Insert the data of table in database.
     * Executa um sql insert com os dados do mapa que representa a tabela.
     * @obs: se ovalor do campo é vazio, tal campo não é incluído no sql.
     * Nem o campo, nem a coluna do campo no sql.
     * @obs: não é necessário que o mapa que representa a tabela tenha as chaves como sendo o nome das colunas,
     * pois o nome das colunas são retirados dos próprios campos (database_obj_str::Field)
     * @tparam MAP_T: estrutura que representa a tabela: ex: std::map, std::unordered_map, etc.
     * É necessário que o map realize as seguintes operações:
     * for(const auto& [key, field] : table) {...
     * const auto& [key_1, field1] = table.begin();
     * @param table: Objeto que representa a tabela.
     * @param database_connection: string necessária para abrir uma conexão com o banco de dados.
     * caso nenhuma string é fornecida, é utilizado a variável global do namespace database::database_connection.
     */
    template<template<typename,typename> typename MAP_T>
    void insert(const MAP_T<std::string, database_obj_str::Field>& table, const std::string &database_connection = database::database_connection);

    /**
     * @brief Update the data of table in database.
     * Executa um sql update com os dados do mapa que representa a tabela.
     * Todos os dados da tabela que não forem chave primária, serão atualizados.
     * A chave primária é informada, por cada campo, por meio da função database_obj_str::Field::is_primary_key()
     * @throw u::error: Caso não haja chave primária na tabela, é lançada uma exceção.
     * @throw u::error: Caso todos os campos da tabela sejam chaves primárias, é lançada uma exceção.
     * @obs: não é necessário que o mapa que representa a tabela tenha as chaves como sendo o nome das colunas,
     * pois o nome das colunas são retirados dos próprios campos (database_obj_str::Field)
     * @obs: se o campo contém um valor vazio:
     * > if: not_null == false && get_data().empty() THEN CAMPO = NULL,
     * > if: not_null == true && get_data().empty() THEN throw exception -> malformed sql statement
     * @tparam MAP_T: estrutura que representa a tabela: ex: std::map, std::unordered_map, etc.
     * É necessário que o map realize as seguintes operações:
     * for(const auto& [key, field] : table) {...
     * const auto& [key_1, field1] = table.begin();
     * @param table: Objeto que representa a tabela.
     * @param database_connection: string necessária para abrir uma conexão com o banco de dados.
     * caso nenhuma string é fornecida, é utilizado a variável global do namespace database::database_connection.
     */
    template<template<typename,typename> typename MAP_T>
    void update(const MAP_T<std::string, database_obj_str::Field>& table, const std::string &database_connection = database::database_connection);

    /**
     * @brief Delete the data of table in database.
     * Executa um sql delete com os dados do mapa que representa a tabela.
     * Deleta o objeto por meio da chave primária, ou seja, a parte WHERE do sql somente
     * conterá a chave primária do objeto.
     * A chave primária é informada, por cada campo, por meio da função database_obj_str::Field::is_primary_key()
     * @throw u::error: Caso não haja chave primária na tabela, é lançada uma exceção.
     * @throw u::error: if: is_primary_key() == true && get_data().empty() THEN throw exception -> malformed sql statement
     * O campo que é chave primária, não pode ser uma string vazia, NULL value.
     * @obs: não verifica se o valor dos campos é vazio.
     * @obs: não é necessário que o mapa que representa a tabela tenha as chaves como sendo o nome das colunas,
     * pois o nome das colunas são retirados dos próprios campos (database_obj_str::Field)
     * @tparam MAP_T: estrutura que representa a tabela: ex: std::map, std::unordered_map, etc.
     * É necessário que o map realize as seguintes operações:
     * for(const auto& [key, field] : table) {...
     * const auto& [key_1, field1] = table.begin();
     * @param table: Objeto que representa a tabela.
     * @param database_connection: string necessária para abrir uma conexão com o banco de dados.
     * caso nenhuma string é fornecida, é utilizado a variável global do namespace database::database_connection.
     */
    template<template<typename,typename> typename MAP_T>
    void del(const MAP_T<std::string, database_obj_str::Field>& table, const std::string &database_connection = database::database_connection);

    ////////////////////////////////////////////////////////////////////////////////
    // funções para realizar uma string de criação do objeto
    ////////////////////////////////////////////////////////////////////////////////
    /**
     * Duas funções para retornar o sql que gera as informações sobre a tabela, para criação do objeto que representa a tabela.
     * Estas funções foram colocados neste arquivo, pois elas geraram informação sobre como gerar uma class database_obj_str::Field.
     */ 
    std::string inline make_sql_information_schema_column(const std::string& table_name);
    std::string inline make_sql_information_schema_constraint_type(const std::string& table_name);

    /**
     * @brief Executa a função map_table(), porém retorna o resultado em uma std::string.
     * Esta função é para ser utilizada pelo projeto database_compiler e é chamada por ele quando se encontra a função: database_offline::map_table
     * Esta função retorna uma string, que representa como se inicializa o um MAP_T<std::string, Field>, onde MAP_T é um template.
     * Se pegar o resultado desta função, e inserir em um arquivo .hpp ou .cpp e compilar o programa, e não houver modificação na estrutura da tabela,
     * é a mesma coisa que em tempo de execução chamar a função 'map_table()'.
     * @param database_connection 
     * @return std::string 
     */
    std::string map_table_str(const std::string& table_name, const std::string &database_connection = database::database_connection);
}

////////////////////////////////////////////////////////////////////////////////
// namespace database_obj_str
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Implementation of templates and inline functions
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//  general functions:: templates and inline functions
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// inline functions
////////////////////////////////////////////////////////////////////////////////
std::string
database_obj_str::make_sql_information_schema_column(const std::string& table_name)
{ try {
    if(table_name.empty()) throw err("Table is an empty string.");

    return "SELECT column_name, data_type, is_nullable FROM INFORMATION_SCHEMA.COLUMNS WHERE table_name="
        + database::quote(table_name) + ";";
 } catch (const std::exception &e) { throw err(e.what()); }
}

std::string
database_obj_str::make_sql_information_schema_constraint_type(const std::string& table_name)
{ try {
    if(table_name.empty()) throw err("Table is an empty string.");

    return "SELECT distinct "
        "kcu.column_name,"
	    "tc.constraint_type "
        "FROM "
            "information_schema.table_constraints AS tc "
            "JOIN information_schema.key_column_usage AS kcu "
            "ON tc.constraint_name = kcu.constraint_name "
            "AND tc.table_schema = kcu.table_schema "
        "WHERE (tc.constraint_type='UNIQUE' OR tc.constraint_type='PRIMARY KEY') AND tc.table_name=" +
        database::quote(table_name) + ";";
 } catch (const std::exception &e) { throw err(e.what()); }
}
////////////////////////////////////////////////////////////////////////////////
// templates
////////////////////////////////////////////////////////////////////////////////
template<template<typename,typename> typename MAP_T>
MAP_T<std::string, database_obj_str::Field> database_obj_str::map_table(const std::string& table_name, const std::string &database_connection)
{ try {
    // a checagem se a table_name é vazio, é feita dentro das funções que geram o sql
    ////////////////////////////////////////////////////////////////////////////////
    // descobre as informações da tabela: nome das colunas, tipos de dados e se pode ser NULL
    ////////////////////////////////////////////////////////////////////////////////
    const auto sql1 = make_sql_information_schema_column(table_name);
    const auto R = database::selectr(sql1, database_connection);

    MAP_T<std::string, database_obj_str::Field>  map;
    for(const auto& col : R) { // column_name, data_type, is_nullable
        const auto column_name = !col["column_name"].is_null() ? col["column_name"].as<std::string>() : 
            throw err("Column name is NULL. - Table: \"%s\".", table_name.c_str());
        const auto data_type = !col["data_type"].is_null() ? col["data_type"].as<std::string>() : 
            throw err("Column data type is NULL. - Table is: \"%s\".", table_name.c_str());
        const auto is_nullable = !col["is_nullable"].is_null() ? col["is_nullable"].as<std::string>() : 
            throw err("Column property is_nullable is NULL. - Table is: \"%s\".", table_name.c_str());
        
        database_obj_str::Field obj { table_name, column_name, data_type, is_nullable };

        map.emplace(column_name, obj);
    }

    ////////////////////////////////////////////////////////////////////////////////
    // recebe as constraints das colunas - UNIQUE and PRIMARY KEY
    ////////////////////////////////////////////////////////////////////////////////
    const auto sql2 = make_sql_information_schema_constraint_type(table_name);
    const auto C = database::selectr(sql2, database_connection);

    for(auto const col : C) {
        const auto column_name = !col["column_name"].is_null() ? col["column_name"].as<std::string>() : 
            throw err("Column name is NULL. - Table: \"%s\".", table_name.c_str());
        const auto constraint_type = !col["constraint_type"].is_null() ? col["constraint_type"].as<std::string>() : 
            throw err("constraint_type is NULL. - Table: \"%s\".", table_name.c_str());
        
        map.at(column_name).set_constraint(constraint_type);
    }

    return map;
 } catch (const std::exception &e) { throw err(e.what()); }
}

template<template<typename,typename> typename MAP_T>
void database_obj_str::insert(const MAP_T<std::string, database_obj_str::Field>& table, const std::string &database_connection)
{ try {
    if(table.empty()) throw err("Map table object is empty.");
    if(database_connection.empty()) throw err("database connection string is empty");

    ////////////////////////////////////////////////////////////////////////////////
    // monta o sql
    ////////////////////////////////////////////////////////////////////////////////
    // recebe o nome da tabela
    const auto& field1 = table.begin()->second;
    const std::string table_name = field1.get_table_name();

    std::string sql = "INSERT INTO " + table_name + "("; // sql
    std::string sql_values = "VALUES(";

    for(const auto& [key, field] : table) {
        if(field.get_data().empty()) continue; // não insere no sql o campo com valor vazio
        sql += field.get_column_name() + ",";
        sql_values += field.get_data_quote() + ",";
    }
    sql.pop_back(); // retira o character ","
    sql_values.pop_back(); // retira o character ","

    sql += ") " + sql_values + ");"; // finaliza o sql
    
    ////////////////////////////////////////////////////////////////////////////////
    // executa o sql
    ////////////////////////////////////////////////////////////////////////////////
    pqxx::connection C(database_connection);
    std::unique_ptr<pqxx::work> W; // necessário para não precisar de outro try-catch to do roolback - W->abort()
    try { // necessário for run C.disconnect() in catch()
        if (!C.is_open())
            throw err("Can't open session database.\nconnection_arg: \"%s\"", database_connection.c_str());
        W = std::make_unique<pqxx::work>(C); // inicia uma transação com o banco de dados - Create a transactional object.
        W->exec0(sql); // executa o sql - se houver respota retorna erro
        W->commit();   // somente faz o commit se NÃO ocorreu erro algum no salvamento
        C.disconnect();
    } catch (pqxx::sql_error const &e) {
        W->abort();
        C.disconnect();
        throw err("Rollback the transaction. SQL error: %s\nQuery was: \"%s\"", e.what(), e.query().c_str());
    } catch (std::exception const &e) {
        C.disconnect();
        throw err(e.what());
    }
 } catch (const std::exception &e) { throw err(e.what()); }
}

template<template<typename,typename> typename MAP_T>
void database_obj_str::update(const MAP_T<std::string, database_obj_str::Field>& table, const std::string &database_connection)
    { try {
    if(table.empty()) throw err("Map table object is empty.");
    if(database_connection.empty()) throw err("database connection string is empty");

    ////////////////////////////////////////////////////////////////////////////////
    // monta o sql
    ////////////////////////////////////////////////////////////////////////////////
    std::string sql_set;
    std::string sql_where;

    for(const auto& [key, field] : table) {
        std::string field_data_quote;
        if(field.get_data().empty()) {
            if(field.is_not_null()) {
                throw err("UPDATE SQL statement is malformed. Field is NOT NULL but its value is NULL (empty string) - see documentation.\n"
                "Table: \'%s\', Column: \'%s\', Map Key: \'%s\'", field.get_table_name().c_str(), field.get_column_name().c_str(), key.c_str());
            } else
                field_data_quote = "NULL";
        }
        else field_data_quote = field.get_data_quote();

        if(field.is_primary_key())
            sql_where += field.get_column_name() + "=" + field_data_quote + " AND ";
        else
            sql_set += field.get_column_name() + "=" + field_data_quote + ",";
    }

    if(sql_set.empty()) throw err("Part SET of sql is empty. - Part WHERE of sql is: \"%s\"", sql_where.c_str());
    if(sql_where.empty()) throw err("Part WHERE of sql is empty. - Part SET of sql is: \"%s\"", sql_set.c_str());

    sql_set.pop_back(); // retira o character ","
    sql_where.erase(sql_where.end()-5, sql_where.end()); // retira a string " AND "

    // recebe o nome da tabela
    const auto& field1 = table.begin()->second;
    const std::string table_name = field1.get_table_name();
    std::string sql = "UPDATE " + table_name + " SET " + sql_set + " WHERE " + sql_where + ";"; // monta o sql
    
    database::exec0(sql, database_connection); // executa o sql
 } catch (const std::exception &e) { throw err(e.what()); }
}

template<template<typename,typename> typename MAP_T>
void database_obj_str::del(const MAP_T<std::string, database_obj_str::Field>& table, const std::string &database_connection)
{ try {
    if(table.empty()) throw err("Map table object is empty.");
    if(database_connection.empty()) throw err("database connection string is empty");

    ////////////////////////////////////////////////////////////////////////////////
    // monta o sql
    ////////////////////////////////////////////////////////////////////////////////
    std::string sql_where;
    for(const auto& [key, field] : table) {
        if(field.is_primary_key()) {
            if(field.get_data().empty()) {
               throw err("DELETE SQL statement is malformed. Field is PRIMRAY KEY but its value is NULL (empty string) - see documentation.\n"
                "Table: \'%s\', Column: \'%s\', Map Key: \'%s\'", field.get_table_name().c_str(), field.get_column_name().c_str(), key.c_str()); 
            }
            sql_where += field.get_column_name() + "=" + field.get_data_quote() + " AND ";
        }
    }

    if(sql_where.empty()) throw err("Part WHERE of sql is empty.");

    sql_where.erase(sql_where.end()-5, sql_where.end()); // retira a string " AND "

    // recebe o nome da tabela
    const auto& field1 = table.begin()->second;
    const std::string table_name = field1.get_table_name();
    std::string sql = "DELETE FROM " + table_name + " WHERE " + sql_where + ";"; // monta o sql
    
    database::exec0(sql, database_connection); // executa o sql
 } catch (const std::exception &e) { throw err(e.what()); }
}


#endif // DATABASE_OBJ_STR_HPP
/*
// void database_online::walk()
{ try {
 } catch (const std::exception &e) { throw err(e.what()); }
}
*/

