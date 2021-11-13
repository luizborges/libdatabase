/**
 *
 * @descripion: funciona como um header que contém todas as bibliotecas, é necessário apenas adicionar
 * essa biblioteca para ter acesso a todas a biblioteca.
 * é feita com base no banco de dados postgres - posteriormente fazer ela ficar genérica para qualquer banco de dados.
 */
#ifndef DATABASEPP_H
#define DATABASEPP_H

////////////////////////////////////////////////////////////////////////////////
// Includes - default libraries - C
////////////////////////////////////////////////////////////////////////////////

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
#include <optional>

#include <pqxx/pqxx>

////////////////////////////////////////////////////////////////////////////////
// Includes - system dependent libraries
////////////////////////////////////////////////////////////////////////////////
#if defined(unix) || defined(__unix) || defined(__unix__) || (defined(__APPLE__) && defined(__MACH__)) // Unix (Linux, *BSD, Mac OS X)
#include <unistd.h>                                                                                    // unix standard library
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
fprintf(stderr, "TRACE FUNC:: \"%s\" (%database, \"%s\")\n",\
__PRETTY_FUNCTION__, __LINE__, __FILE__);\
fprintf(stderr, "*****************************************\n");
*/

////////////////////////////////////////////////////////////////////////////////
// namespace
////////////////////////////////////////////////////////////////////////////////
namespace database
{
    ////////////////////////////////////////////////////////////////////////////////
    //  general variables
    ////////////////////////////////////////////////////////////////////////////////
    extern std::string database_connection; // init in general.cpp

    ////////////////////////////////////////////////////////////////////////////////
    //  general functions - file: general.cpp
    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
    //  auxliar functions
    ////////////////////////////////////////////////////////////////////////////////
    /**
     * Realiza o quote ou escaping of a string.
     * @obs: a primeira vez que a função é usada é necessário ou estar definido já a
     * variável database::database_connection ou passar uma string para abertura da conexão válida.
     * isto é uma exigência da função quote do pqxx.
     * Entretanto, apenas na primeira vez isto é feito, nas outras não é necessário.
     * A conexão é criada e desconectada em seguida, para nas outras vezes, apenas a função quote ser usada sem
     * a necessidade de abrir e fechar uma conexão.
     * @arg str: string que será submetida ao quote().
     * @arg database_connection: string necessária para abrir uma conexão com o banco de dados.
     * caso nenhuma string é fornecida, é utilizado a variável global do namespace database::database_connection.
     * @return: a string passada em @str que foi tratada pela pqxx::connection_base.quote(str)
     */
    std::string quote(const std::string &str, const std::string &database_connection = database::database_connection);

    ////////////////////////////////////////////////////////////////////////////////
    //  sql functions
    ////////////////////////////////////////////////////////////////////////////////
    /**
     * Inserts a obj_map in table name.
     * execute a command sql insert.
     * Cria o comando sql insert e executa ele com base nas chaves do obj_map.
     * @obs: Executa função: exec0() -> se o banco de dados retornar algum resultado dispara uma exceção.
     * @obs: todas as chaves do obj_map são usadas para a criação do sql que será executado.
     * @obs: todas os valores, por default, são colocados em quotes pela função quote do PGSQL.
     * para retirar algum valor do quote(obj_map["KEY"]) inserir a KEY no argumento no_quote.
     * @obs: é esperado que todos os valores de obj_map["KEY"] sejam std::string
     * @arg table_name: nome da tabela que será inserido os dados constantes em obj_map.
     * @arg obj_map: objeto tipo map que contém os dados que serão inseridos na tabela.
     * O obj_map segue algumas funções básicas do std::map<> e std::unordered_map<>,  etc..
     * É esperado que o obj_map mapeie strings para strings.
     * Exemplos de obj_map válidos: std::map<std::string,std::string> e std::unordered_map<std::string,std::string>
     * O obj_map deve responder as seguintes funções:
     * for(const auto& elem : obj_map) -> retorna cada elemento do obj_map
     * elem.first -> retorna a chave do elemento -> elem.first = "KEY"
     * elem.second -> valor da chave no elemento -> obj_map["KEY"] >> a função obj_map["KEY"] não é necessária.
     * obj_map.empty() -> para verificação se existe algum elemente a ser inserido ou não
     * @arg no_quote: values in obj_map que não serão colocados in quote() function para serem inseridos no sql.
     * é necessário apenas inserir o nome da chave.
     * ex: no_quote = {"id", "money"} => NÃO será executado: quote(obj_map["id"]) e quote(obj_map["money"]) para
     * inserir os valores no sql
     * @arg database_connection: string necessária para abrir uma conexão com o banco de dados.
     * caso nenhuma string é fornecida, é utilizado a variável global do namespace database::database_connection.
     */
    template <typename MAP_T>
    void insert(const std::string &table_name, const MAP_T &obj_map,
                const std::unordered_set<std::string> &no_quote = {},
                const std::string &database_connection = database::database_connection);

    /**
     * Run the select sql on database and retrieve data, and put data on SET_T collection.
     * @obs: os valores são transformados em std::string para a inserção nas estruturas de resposta.
     * @obs: os valores do tipo NULL são transformados em uma string vazia -> "".
     * @obs SET_T: é a estrutura que conterá os MAP_T que foram as linhas da resposta do sql.
     * a estrutura deve conter a seguinte interface:
     * SET_T set = {}; set.push_back(map); onde map é do tipo MAP_T.
     * para não interfeir na ordem dos resultados do sql, a função push_back(), espera-se que
     * ela insira os dados ao final da estrutura, para manter a ordem retornada pelo sql.
     * exemplos de estruturas compatíveis: std::vector<> e std::list<>
     * @obs MAP_T: é a estrutura que conterá a resposta de uma linha do resultado do sql.
     * deve ser possível a seguinte inserção:
     * MAP_T map = {}; map["column_name"] = column_value; -> onde:
     * column_name: é o nome da coluna.
     * column_value: é o valor da coluna.
     * a cada chamada de map["column_name"] = column_value; deve-se inserir uma nova tupla na estrutura.
     * exemplos de estruturas compatíveis: std::map<> e std::unordered_map<>
     * @obs: exemplo de uso da função:
     * const auto R = database::select<std::vector, std::unordered_map<std::string, std::string>>(sql, DATABASE_CONNECTION);
     * com isto o SET_T será ao final: std::vector<std::unordered_map<std::string, std::string>>, pois ele é
     * passado como um template para a função
     * @arg sql: string contendo o sql que será executado no banco de dados.
     * @arg database_connection: string necessária para abrir uma conexão com o banco de dados.
     * caso nenhuma string é fornecida, é utilizado a variável global do namespace database::database_connection.
     * @return uma coleção definida do tipo SET_T que conterá a resposta do sql.
     */
    template <template <typename> typename SET_T, template <typename, typename> typename MAP_T>
    SET_T<MAP_T<std::string, std::string>> select(const std::string &sql, const std::string &database_connection = database::database_connection);

    /**
     * Run the select sql on database and retrieve data, and put data on MAP_T collection.
     * Esta função espera apenas que o sql retorne uma única linha, caso retorne mais, é lançaco uma exceção.
     * @obs: os valores são transformados em std::string para a inserção nas estruturas de resposta.
     * @obs: os valores do tipo NULL são transformados em uma string vazia -> "".
     * @obs MAP_T: é a estrutura que conterá a resposta de uma linha do resultado do sql.
     * deve ser possível a seguinte inserção:
     * MAP_T map = {}; map["column_name"] = column_value; -> onde:
     * column_name: é o nome da coluna.
     * column_value: é o valor da coluna.
     * a cada chamada de map["column_name"] = column_value; deve-se inserir uma nova tupla na estrutura.
     * exemplos de estruturas compatíveis: std::map<> e std::unordered_map<>
     * @obs: exemplo de uso da função:
     * const auto R = database::select1<std::unordered_map<std::string, std::string>>(sql, DATABASE_CONNECTION);
     * @obs: esta função executa o sql pela seguinte função:
     * pqxx::nontransaction::exec1(sql);
     * isto significa que se o sql retornar mais de uma linha, é lançado uma exceção.
     * @arg sql: string contendo o sql que será executado no banco de dados.
     * @arg database_connection: string necessária para abrir uma conexão com o banco de dados.
     * caso nenhuma string é fornecida, é utilizado a variável global do namespace database::database_connection.
     * @return uma estrutura do tipo MAP_T que conterá a resposta do sql.
     */
    template <template <typename, typename> typename MAP_T>
    MAP_T<std::string, std::string> select1(const std::string &sql, const std::string &database_connection = database::database_connection);

    /**
     * Run the select sql on database and retrieve data, and return the object of the library of database.
     * A letra 'r' representa result, ou seja, este select retorna o resultado original que a biblioteca do banco de dados retorna.
     * Executa a função "exec"
     * Como a biblioteca, até o momento tem sido feita baseada no banco de dados postgres, e na biblioteca que o implementa em C++
     * a libpqxx, o resultado de retorno é um pqxx::result
     * @arg sql: string contendo o sql que será executado no banco de dados.
     * @arg database_connection: string necessária para abrir uma conexão com o banco de dados.
     * caso nenhuma string é fornecida, é utilizado a variável global do namespace database::database_connection.
     * @return retorna o objeto da biblioteca do banco de dados que representa o resultado do sql.
     * Por exemplo, para a biblioteca "pqxx" que representa o banco de dados "postgres", o objeto que
     * representa o resultado da busca sql é o "pqxx::result". é este objeto que é retornado neste caso.
     */
    pqxx::result selectr(const std::string &sql, const std::string &database_connection = database::database_connection);

    /**
     * Run the select sql on database and retrieve data, and return the object of the library of database.
     * A letra 'r' representa raw result, ou seja, este select retorna o resultado original que a biblioteca do banco de dados retorna.
     * Executa a função "exec1"
     * Como a biblioteca, até o momento tem sido feita baseada no banco de dados postgres, e na biblioteca que o implementa em C++
     * a libpqxx, o resultado de retorno é um pqxx::row
     * O sql deve ter como resposta uma tabela que contenha apenas 1 única linha, caso contrário é lançado uma exceção.
     * pqxx::row representa uma linha da tabela.
     * Esta função é ideal para resultados que devolvem apenas 1 única linha.
     * Maior ganho de eficiência, além da checagem do número do resultado comparado com a função "selectr".
     * @arg sql: string contendo o sql que será executado no banco de dados.
     * @arg database_connection: string necessária para abrir uma conexão com o banco de dados.
     * caso nenhuma string é fornecida, é utilizado a variável global do namespace database::database_connection.
     * @return retorna o objeto da biblioteca do banco de dados que representa o resultado do sql.
     * Por exemplo, para a biblioteca "pqxx" que representa o banco de dados "postgres", o objeto que
     * representa o resultado da busca sql é o "pqxx::row". é este objeto que é retornado neste caso.
     */
    pqxx::row selectr1(const std::string &sql, const std::string &database_connection = database::database_connection);

    /**
     * Retorna os elementos de uma tabela apenas.
     * E a quantidade de elementos totais da tabela que satisfazem a cláusula WHERE.
     * Este sql foi feito para buscas em telas CRUD simples, que tem paginação e exibição do total de resultados na tela.
     * @obs: JAMAIS PASSAR PARÂMETROS DO USUÁRIO PARA ESTA FUNÇÃO DIRETAMENTE.
     * @arg table_name: nome da tabela que será objeto de busca.
     * @arg select: string que representa a parte where do SQL.
     * Se nenhuma string é passado, retorna todos os elementos da tabela.
     * @arg where: string que representa a parte where do sql.
     * se nenhuma string é passada, o sql não contém a parte WHERE.
     * exemples: @arg(where): "name LIKE 'ana%'" ou "idade > 17 AND nome ILIKE 'luiz_%'"
     * @arg order_by: string que representa a parte que será o ORDER BY.
     * @arg page: o número da página do resultado.
     * Cada página contém um número máximo de resultados setados em @arg(limit)
     * Por este campo e pelo campo @arg(limit) é calculado o OFFSET.
     * O valor do OFFSET é calculado da seguinte forma: OFFSET = @arg(page) * @arg(limit)
     * O OFFSERT representa o número de linhas do resultados que será ignorado quando a resposta for gerada.
     * - necessário para realizar paginação na tela do usuário.
     * - se o valor passado for um número negativo, o cálculo do offset é da seguinte forma: OFFSET = ABS(@arg(page)).
     * onde ABS(x), representa o valor absoluto, o módulo, do número 'x'
     * - o valor defaut do campo é 0.
     * - se nenhuma string é passada, é setado o valor default do campo.
     * @arg limit: número máximo de elementos retornados na busca.
     * - necessário para realizar paginação na tela do usuário.
     * - o valor do campos deve ser > 0, caso contrário é lançado uma exceção
     * - se passado o valor -1, o sql gerado não conterá LIMIT.
     * - se nenhum valor é passado, o valor default do campo é 10.
     * @return tupla contendo dois elementos, que são:
     * get<0>: pqxx::result -> resultado da busca no formato de retorno do postgres
     * get<1>: std::string- > número total de linhas (objetos) na tabela que uma busca sem limit e offset retornam.
     * é o resultado do select (*) WHERE @where, onde @where é o @arg(where) passado na função.
     * - em caso de erro - retornar NULL, o valor passado é "0".
     */
    std::tuple<pqxx::result, std::string>
    selectt(const std::string &table_name,
            const std::string &select = "",
            const std::string &where = "",
            const std::string &order_by = "",
            const std::string &page_str = "",
            const std::string &limit_str = "",
            const std::string &database_connection = database::database_connection);

    /**
     * Run the update sql on the database.
     * @obs: Executa função: exec0() -> se o banco de dados retornar algum resultado dispara uma exceção.
     * @obs: todas os valores, por default, são colocados em quotes pela função quote do PGSQL.
     * para retirar algum valor do quote(obj_map["KEY"]) inserir a KEY no argumento no_quote.
     * @obs: é esperado que todos os valores de obj_map["KEY"] sejam std::string
     * @arg table_name: nome da tabela que será inserido os dados constantes em obj_map.
     * @arg obj_map: objeto tipo map que contém os dados que serão atualizados na tabela.
     * O obj_map segue algumas funções básicas do std::map<> e std::unordered_map<>,  etc..
     * É esperado que o obj_map mapeie strings para strings.
     * Exemplos de obj_map válidos: std::map<std::string,std::string> e std::unordered_map<std::string,std::string>
     * O obj_map deve responder as seguintes funções:
     * for(const auto& elem : obj_map) -> retorna cada elemento do obj_map
     * elem.first -> retorna a chave do elemento -> elem.first = "KEY"
     * elem.second -> retorna o valor da chave no elemento -> obj_map["KEY"].
     * obj_map.empty() -> para verificação se existe algum elemente a ser inserido ou não
     * obj_map["KEY"] -> retorna o valor correspondente a chave "KEY" no map
     * @arg key_where: as chaves do map que irão ser colocados na parte WHERE do SQL.
     * deve-se inserir as chaves do map neste conjunto, exemplo: key_where = {"key1", "key20"}
     * se a variável key_where for vazia, key_where = {}, nenhum valor do map é inserido de maneira automática na
     * parte WHERE do SQL.
     * as chaves serão sempre conjungadas pelo operador 'AND' do SQL.
     * exemplo:
     * key_where = {"key1", "key20"} -> gera: key1 = 'obj_map["key1"]' AND key2 = 'obj_map["key20"]'
     * @arg key_set: as chaves do map que irão ser colocados na parte SET do SQL.
     * deve-se inserir as chaves do map neste conjunto, exemplo: key_set = {"key1", "key20"}
     * se uma chave for inserida tanto na variável key_set e key_where, tal chave com seu respectivo valor somente
     * será inserida na parte WHERE do SQL, será como se ela não foi inserida na variável key_set.
     * se a variável key_where for vazia, key_where = {}, todos os valores do obj_map serão inseridos na parte SET, desde
     * que a chave não esteja também na variável key_where.
     * se a key_set for vazio, todos os valores serão inseridos em key_set, desde que não estejam em key_where.
     * exemplo: obj_map tem as seguintes chaves: {"key1", "key2", "key3"}
     * se: key_set = {"key1"} e key_where = {"key2"}
     * tem-se o sql terá: SET key1 = 'obj_map["key1"]' WHERE key2 = 'obj_map["key2"]'
     * se: key_set = {} e key_where = {"key2"}
     * tem-se o sql terá: SET key1 = 'obj_map["key1"]', key3 = 'obj_map["key3"]' WHERE key2 = 'obj_map["key2"]'
     * @arg no_quote: values in obj_map que não serão colocados in quote() function para serem inseridos no sql.
     * é necessário apenas inserir o nome da chave.
     * ex: no_quote = {"id", "money"} => NÃO será executado: quote(obj_map["id"]) e quote(obj_map["money"]) para
     * inserir os valores no sql
     * @arg where_sql: string que é adiciona ao final das inserções do key_where na parte WHERE do sql.
     * esta string é para o caso do WHERE precisar ser mais complexo.
     * a operação que é feita, caso where_sql não seja vazio é a seguinte:
     * where += where_sql;
     * ou seja, o usuário deve informar no sql qual é o operador de ligação com a parte anterior do where.
     * exemplo: where_sql = "AND id > 3" ou where_sql = "OR NOT id < 20"
     * se a string for vazia, ela é ignorada
     * @arg database_connection: string necessária para abrir uma conexão com o banco de dados.
     * caso nenhuma string é fornecida, é utilizado a variável global do namespace database::database_connection.
     */
    template <typename MAP_T>
    void update(const std::string &table_name, const MAP_T &obj_map,
                const std::unordered_set<std::string> &key_where = {},
                const std::vector<std::string> &key_set = {},
                const std::unordered_set<std::string> &no_quote = {},
                const std::string &where_sql = "",
                const std::string &database_connection = database::database_connection);

    /**
     * Run the delete sql on the database.
     * @obs: Executa função: exec0() -> se o banco de dados retornar algum resultado dispara uma exceção.
     * @obs: todas os valores, por default, são colocados em quotes pela função quote do PGSQL.
     * para retirar algum valor do quote(obj_map["KEY"]) inserir a KEY no argumento no_quote.
     * @obs: é esperado que todos os valores de obj_map["KEY"] sejam std::string
     * @arg table_name: nome da tabela que será inserido os dados constantes em obj_map.
     * @arg obj_map: objeto tipo map que contém os dados que serão deletados da tabela.
     * O obj_map segue algumas funções básicas do std::map<> e std::unordered_map<>,  etc..
     * É esperado que o obj_map mapeie strings para strings.
     * Exemplos de obj_map válidos: std::map<std::string,std::string> e std::unordered_map<std::string,std::string>
     * O obj_map deve responder as seguintes funções:
     * for(const auto& elem : obj_map) -> retorna cada elemento do obj_map
     * elem.first -> retorna a chave do elemento -> elem.first = "KEY"
     * elem.second -> retorna o valor da chave no elemento.
     * obj_map.empty() -> para verificação se existe algum elemente a ser inserido ou não
     * obj_map["KEY"] -> retorna o valor correspondente a chave "KEY" no map
     * @arg key_where: as chaves do map que irão ser colocados na parte WHERE do SQL.
     * deve-se inserir as chaves do map neste conjunto, exemplo: key_where = {"key1", "key20"}
     * se a variável key_where for vazia, key_where = {}, todos os valores do map são inseridos de maneira automática na
     * parte WHERE do SQL.
     * as chaves serão sempre conjungadas pelo operador 'AND' do SQL.
     * exemplo:
     * key_where = {"key1", "key20"} -> gera: key1 = 'obj_map["key1"]' AND key2 = 'obj_map["key20"]'
     * @arg no_quote: values in obj_map que não serão colocados in quote() function para serem inseridos no sql.
     * é necessário apenas inserir o nome da chave.
     * ex: no_quote = {"id", "money"} => NÃO será executado: quote(obj_map["id"]) e quote(obj_map["money"]) para
     * inserir os valores no sql
     * @arg where_sql: string que é adiciona ao final das inserções do key_where na parte WHERE do sql.
     * esta string é para o caso do WHERE precisar ser mais complexo.
     * a operação que é feita, caso where_sql não seja vazio é a seguinte:
     * where += where_sql;
     * ou seja, o usuário deve informar no sql qual é o operador de ligação com a parte anterior do where.
     * exemplo: where_sql = "AND id > 3" ou where_sql = "OR NOT id < 20"
     * se a string for vazia, ela é ignorada
     * @arg database_connection: string necessária para abrir uma conexão com o banco de dados.
     * caso nenhuma string é fornecida, é utilizado a variável global do namespace database::database_connection.
     */
    template <typename MAP_T>
    void del(const std::string &table_name, const MAP_T &obj_map,
             const std::unordered_set<std::string> &key_where = {},
             const std::unordered_set<std::string> &no_quote = {},
             const std::string &where_sql = "",
             const std::string &database_connection = database::database_connection);

    /**
     * Executa um sql que não pode obter resposta do servidor.
     * Se o sql retornar alguma resposta levanta exceção.
     * O sql deve estar totalmente contido na string sql.
     * Executa roolback em caso de falha.
     * no postgres executa: pqxx::work.exec0(sql)
     * Esta função foi feita para executar updates, inserts, deletes, que envolvam mais de uma tabela ou outros
     * sqls complexos, que não necessitem de retorno
     * @arg sql: Sql que será executado.
     * @arg database_connection: string necessária para abrir uma conexão com o banco de dados.
     * caso nenhuma string é fornecida, é utilizado a variável global do namespace database::database_connection.
     */
    void exec0(const std::string &sql, const std::string &database_connection = database::database_connection);
    ////////////////////////////////////////////////////////////////////////////////
    //  end namespace database database::
    ////////////////////////////////////////////////////////////////////////////////
} // end namespace database database::

////////////////////////////////////////////////////////////////////////////////
// namespace online
////////////////////////////////////////////////////////////////////////////////
#include "database_online.hpp"

////////////////////////////////////////////////////////////////////////////////
// namespace obj_str
////////////////////////////////////////////////////////////////////////////////
#include "database_obj_str.hpp"

////////////////////////////////////////////////////////////////////////////////
// namespace offline
////////////////////////////////////////////////////////////////////////////////
#include "database_offline.hpp"

////////////////////////////////////////////////////////////////////////////////
// namespace database
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Implementation of templates and inline functions
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//  general functions:: templates and inline functions
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// templates
////////////////////////////////////////////////////////////////////////////////
template <typename MAP_T>
void database::insert(const std::string &table_name, const MAP_T &obj_map,
                      const std::unordered_set<std::string> &no_quote,
                      const std::string &database_connection)
{
    try
    {
        if (obj_map.empty())
            return;
        if (table_name.empty())
            throw err("table name is empty");
        if (database_connection.empty())
            throw err("database connection string is empty");

        pqxx::connection C(database_connection);
        std::unique_ptr<pqxx::work> W; // necessário para não precisar de outro try-catch to do roolback - W->abort()
        try
        { // necessário for run C.disconnect() in catch()
            if (!C.is_open())
                throw err("Can't open session database.\nconnection_arg: \"%s\"", database_connection.c_str());
            W = std::make_unique<pqxx::work>(C); // inicia uma transação com o banco de dados - Create a transactional object.

            ////////////////////////////////////////////////////////////////////////////////
            // monta o sql e executa
            ////////////////////////////////////////////////////////////////////////////////
            std::string sql = "INSERT INTO " + table_name + "(";
            std::string val = ") VALUES (";
            bool all_values_is_empty = true;
            for (const auto &E : obj_map)
            {
                if (E.second.empty())
                    continue; // não insere uma chave/valor vazio no sql
                all_values_is_empty = false;

                sql += E.first + ",";
                if (no_quote.find(E.first) == no_quote.end())
                { // não encontrou a chave - faz o quote no valor do elemento
                    val += W->quote(E.second) + ",";
                }
                else
                { // para fazer o quote() no valor do elemento - chave do Elemento não está no no_quote
                    val += E.second + ",";
                }
            }
            if (all_values_is_empty)
            { // se todos os valores do obj_map são vazios, não faz nada
                C.disconnect();
                return;
            }

            sql.pop_back();    // retira o último character ',' da string
            val.pop_back();    // retira o último character ',' da string
            sql += val + ");"; // acaba de montar o sql quer será executado: une os nomes dos campos com seus valores

            W->exec0(sql); // executa o sql - se houver respota retorna erro
            W->commit();   // somente faz o commit se NÃO ocorreu erro algum no salvamento
            C.disconnect();
        }
        catch (pqxx::sql_error const &e)
        {
            W->abort();
            C.disconnect();
            throw err("Rollback the transaction. SQL error: %s\nQuery was: \"%s\"", e.what(), e.query().c_str());
        }
        catch (std::exception const &e)
        {
            C.disconnect();
            throw err(e.what());
        }
    }
    catch (const std::exception &e)
    {
        throw err(e.what());
    }
}

template <template <typename> typename SET_T, template <typename, typename> typename MAP_T>
SET_T<MAP_T<std::string, std::string>>
database::select(const std::string &sql, const std::string &database_connection)
{
    try
    {
        if (sql.empty())
            throw err("sql of select query is empty");
        if (database_connection.empty())
            throw err("database connection string is empty");

        pqxx::connection C(database_connection);
        try
        { // necessário for run C.disconnect() in catch()
            if (!C.is_open())
                throw err("Can't open session database.\nconnection_arg: \"%s\"", database_connection.c_str());
            pqxx::nontransaction N(C); // inicia uma transação com o BD /* Create a notransactional object. */

            ////////////////////////////////////////////////////////////////////////////////
            // executa o sql e insere o resultado nas estruturas
            ////////////////////////////////////////////////////////////////////////////////
            auto R{N.exec(sql)}; // executa o sql

            SET_T<MAP_T<std::string, std::string>> set;
            for (const auto &r : R)
            {
                MAP_T<std::string, std::string> map;
                for (const auto &f : r)
                { // field of row
                    std::string column_name = u::to_str(f.name());
                    std::string value = f.is_null() ? "" : f.as<std::string>();
                    map[column_name] = value;
                }
                set.push_back(map);
            }

            C.disconnect();
            return set;
        }
        catch (pqxx::sql_error const &e)
        {
            C.disconnect();
            throw err("SQL error: %s\nQuery was: \"%s\"", e.what(), e.query().c_str());
        }
        catch (std::exception const &e)
        {
            C.disconnect();
            throw err(e.what());
        }
    }
    catch (const std::exception &e)
    {
        throw err(e.what());
    }
}

template <template <typename, typename> typename MAP_T>
MAP_T<std::string, std::string>
database::select1(const std::string &sql, const std::string &database_connection)
{
    try
    {
        if (sql.empty())
            throw err("sql of select query is empty");
        if (database_connection.empty())
            throw err("database connection string is empty");

        pqxx::connection C{database_connection};
        try
        { // necessário for run C.disconnect() in catch()
            if (!C.is_open())
                throw err("Can't open session database.\nconnection_arg: \"%s\"", database_connection.c_str());
            pqxx::nontransaction N{C}; // inicia uma transação com o BD - Create a notransactional object.

            ////////////////////////////////////////////////////////////////////////////////
            // executa o sql e insere o resultado nas estruturas
            ////////////////////////////////////////////////////////////////////////////////
            auto R{N.exec1(sql)}; // executa o sql

            MAP_T<std::string, std::string> map;
            for (const auto &f : R)
            { // field of row
                std::string column_name = u::to_str(f.name());
                std::string value = f.is_null() ? "" : f.as<std::string>();
                map[column_name] = value;
            }
            C.disconnect();
            return map;
        }
        catch (pqxx::sql_error const &e)
        {
            C.disconnect();
            throw err("SQL error: %s\nQuery was: \"%s\"", e.what(), e.query().c_str());
        }
        catch (std::exception const &e)
        {
            C.disconnect();
            throw err(e.what());
        }
    }
    catch (const std::exception &e)
    {
        throw err(e.what());
    }
}

// select inner join interface
// template<typename MAP_T, typename SET_T>
// pqxx::result database::selectij(const std::string& select,
//     const std::vector<std::tuple<std::string, MAP_T obj_map, std::optional<std::string>, std::optional<std::string>>, // elementos da tupla em ordem: tuple<nome da tabela, mapa que representa o objeto, nome da coluna da tabela que participará do inner join, TABELA.COLUNA - que será feito o inner join>
//     const std::unordered_set<std::tuple<std::string, std::string>& key_where,
//     const std::unordered_set<std::tuple<std::string, std::string>& no_quote,
//     const std::string& where_sql,
//     const std::vector<std::string>& order_by, // o order by pode ter várias regras, com vários argumentos
//     const std::string& limit,
//     const std::string& offset,
//     const std::string& end_sql, // fim do sql, pois pode ter sqls que necessitam disso - ex: SELECT EXISTS ( SELECT ....);
//     const std::string& database_connection)

template <typename MAP_T>
void database::update(const std::string &table_name, const MAP_T &obj_map,
                      const std::unordered_set<std::string> &key_where,
                      const std::vector<std::string> &key_set,
                      const std::unordered_set<std::string> &no_quote,
                      const std::string &where_sql,
                      const std::string &database_connection)
{
    try
    {
        if (obj_map.empty())
            throw err("object map is empty");
        if (table_name.empty())
            throw err("table name is empty");
        if (database_connection.empty())
            throw err("database connection string is empty");

        pqxx::connection C(database_connection);
        std::unique_ptr<pqxx::work> W; // necessário para não precisar de outro try-catch to do roolback - W->abort()
        try
        { // necessário for run C.disconnect() in catch()
            if (!C.is_open())
                throw err("Can't open session database.\nconnection_arg: \"%s\"", database_connection.c_str());
            W = std::make_unique<pqxx::work>(C); // inicia uma transação com o banco de dados - Create a transactional object.

            std::string sql = "UPDATE " + table_name + " SET"; // inicializa o sql

            ////////////////////////////////////////////////////////////////////////////////
            // monta a parte SET do sql
            ////////////////////////////////////////////////////////////////////////////////
            std::string set = "";
            if (key_set.empty())
            { // incluir todas as chaves de obj_map - as chaves no key_where
                for (const auto &E : obj_map)
                {
                    if (key_where.find(E.first) != key_where.end())
                        continue; // não é para inserir a chave

                    set += " " + E.first + "=";
                    if (no_quote.find(E.first) == no_quote.end())
                    { // não encontrou a chave - faz o quote no valor do elemento
                        set += W->quote(E.second) + ",";
                    }
                    else
                    { // para fazer o quote() no valor do elemento - chave do Elemento não está no no_quote
                        set += E.second + ",";
                    }
                }
            }
            else
            { // incluir somente as que estão no array key_set
                for (const auto &KEY : key_set)
                {
                    if (key_where.find(KEY) == key_where.end())
                        continue; // não é para inserir a chave

                    set += " " + KEY + "=";
                    if (no_quote.find(KEY) == no_quote.end())
                    { // não encontrou a chave - faz o quote no valor do elemento
                        set += W->quote(obj_map.at(KEY)) + ",";
                    }
                    else
                    { // para fazer o quote() no valor do elemento - chave do Elemento não está no no_quote
                        set += obj_map.at(KEY) + ",";
                    }
                }
            }

            if (set.empty())
                throw err("SET part of sql is empty");
            set.pop_back(); // retira o último character ',' da string
            sql += set;
            ////////////////////////////////////////////////////////////////////////////////
            // monta a parte WHERE do sql
            ////////////////////////////////////////////////////////////////////////////////
            std::string where = "";
            for (const auto &KEY : key_where)
            {
                where += " " + KEY + " = ";
                if (no_quote.find(KEY) == no_quote.end())
                { // não encontrou a chave - faz o quote no valor do elemento
                    where += W->quote(obj_map.at(KEY)) + " AND ";
                }
                else
                { // para fazer o quote() no valor do elemento - chave do Elemento não está no no_quote
                    where += obj_map.at(KEY) + " AND ";
                }
            }
            if (!where.empty())
                where.erase(where.size() - 4); // retira do final a substring "AND "
            if (!where_sql.empty())
                where += where_sql;
            if (where.empty())
                throw err("WHERE part of sql is empty");

            ////////////////////////////////////////////////////////////////////////////////
            // finaliza o sql e executa
            ////////////////////////////////////////////////////////////////////////////////
            sql += " WHERE " + where + ";"; // finaliza o sql
                                            // throw err("sql: \"%s\"", sql.c_str());
            W->exec0(sql);                  // executa o sql - se houver respota retorna erro
            W->commit();                    // somente faz o commit se NÃO ocorreu erro algum no salvamento
            C.disconnect();
        }
        catch (pqxx::sql_error const &e)
        {
            W->abort();
            C.disconnect();
            throw err("Rollback the transaction. SQL error: %s\nQuery was: \"%s\"", e.what(), e.query().c_str());
        }
        catch (std::exception const &e)
        {
            C.disconnect();
            throw err(e.what());
        }
    }
    catch (const std::exception &e)
    {
        throw err(e.what());
    }
}

template <typename MAP_T>
void database::del(const std::string &table_name, const MAP_T &obj_map,
                   const std::unordered_set<std::string> &key_where,
                   const std::unordered_set<std::string> &no_quote,
                   const std::string &where_sql,
                   const std::string &database_connection)
{
    try
    {
        if (obj_map.empty())
            throw err("object map is empty");
        if (table_name.empty())
            throw err("table name is empty");
        if (database_connection.empty())
            throw err("database connection string is empty");

        pqxx::connection C(database_connection);
        std::unique_ptr<pqxx::work> W; // necessário para não precisar de outro try-catch to do roolback - W->abort()
        try
        { // necessário for run C.disconnect() in catch()
            if (!C.is_open())
                throw err("Can't open session database.\nconnection_arg: \"%s\"", database_connection.c_str());
            W = std::make_unique<pqxx::work>(C); // inicia uma transação com o banco de dados - Create a transactional object.

            std::string sql = "DELETE FROM " + table_name + " WHERE "; // inicializa o sql

            ////////////////////////////////////////////////////////////////////////////////
            // monta a parte WHERE do sql
            ////////////////////////////////////////////////////////////////////////////////
            std::string where = "";
            if (key_where.empty())
            { // incluir todas as chaves de obj_map - as chaves no key_where
                for (const auto &E : obj_map)
                {
                    where += " " + E.first + "=";
                    if (no_quote.find(E.first) == no_quote.end())
                    { // não encontrou a chave - faz o quote no valor do elemento
                        where += W->quote(E.second) + " AND ";
                    }
                    else
                    { // para fazer o quote() no valor do elemento - chave do Elemento não está no no_quote
                        where += E.second + " AND ";
                    }
                }
            }
            else
            { // incluir somente as que estão no array key_where
                for (const auto &KEY : key_where)
                {
                    where += " " + KEY + "=";
                    if (no_quote.find(KEY) == no_quote.end())
                    { // não encontrou a chave - faz o quote no valor do elemento
                        where += W->quote(obj_map.at(KEY)) + " AND ";
                    }
                    else
                    { // para fazer o quote() no valor do elemento - chave do Elemento não está no no_quote
                        where += obj_map.at(KEY) + " AND ";
                    }
                }
            }

            if (!where.empty())
                where.erase(where.size() - 4); // retira do final a substring "AND "
            if (!where_sql.empty())
                where += where_sql;
            if (where.empty())
                throw err("WHERE part of sql is empty");

            ////////////////////////////////////////////////////////////////////////////////
            // finaliza o sql e executa
            ////////////////////////////////////////////////////////////////////////////////
            sql += where + ";"; // finaliza o sql
                                // throw err("sql: \"%s\"", sql.c_str());
            W->exec0(sql);      // executa o sql - se houver respota retorna erro
            W->commit();        // somente faz o commit se NÃO ocorreu erro algum no salvamento
            C.disconnect();
        }
        catch (pqxx::sql_error const &e)
        {
            W->abort();
            C.disconnect();
            throw err("Rollback the transaction. SQL error: %s\nQuery was: \"%s\"", e.what(), e.query().c_str());
        }
        catch (std::exception const &e)
        {
            C.disconnect();
            throw err(e.what());
        }
    }
    catch (const std::exception &e)
    {
        throw err(e.what());
    }
}
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////



#endif // DATABASEPP_H
/*
// void database_online::walk()
{ try {
 } catch (const std::exception &e) { throw err(e.what()); }
}
*/

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
