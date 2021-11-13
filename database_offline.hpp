/**
 *
 * @descripion: Contém o namespace e os templates das funções e classes do namespace database_offline.
 * Não é necessário incluir este header, ele já faz parte do database.hpp.
 * o namespace database_online contém as funções de criação automática, em tempo de compilação, de criação de:
 * > INNER JOINS
 * > quais são as colunas e os tipos da tabela.
 * > representação do banco de dados em um grafo, que contém as tabelas e as chaves estrangeiras.
 */
#ifndef DATABASE_OFFLINE_HPP
#define DATABASE_OFFLINE_HPP

#include <string>
#include <unordered_set>
#include <unordered_map>
// #include "database_online.hpp"
// #include "database_obj_str.hpp"


/**
 * Este namespace cuida das funções que 
 */
namespace database_offline
{
    ////////////////////////////////////////////////////////////////////////////////
    // class
    ////////////////////////////////////////////////////////////////////////////////

    std::string
    inline sql_inner_join(
        const std::unordered_set<std::string>& tables_name, 
        const std::string &database_connection = database::database_connection);

    // template<template<typename, typename> typename MAP_T>
    // MAP_T<std::string, database_obj_str::Field>
    // auto
    std::unordered_map<std::string, database_obj_str::Field>
    inline map_table(
        const std::string& table_name, 
        const std::string &database_connection = database::database_connection);
    

    // template<template<typename, typename> typename MAP_T>
    // MAP_T<std::string, database_online::Table>
    // auto
    std::unordered_map<std::string, database_online::Table>
    inline init_database_graph(
        const std::string &database_connection = database::database_connection);
   
}


////////////////////////////////////////////////////////////////////////////////
// namespace database_offline
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Implementation of templates and inline functions
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//  general functions:: templates and inline functions
////////////////////////////////////////////////////////////////////////////////
std::string 
database_offline::sql_inner_join(
    const std::unordered_set<std::string>& tables_name, 
    const std::string &database_connection)
{ try {
    throw err("database_offline::%s() must be generate on compiling time. See documentation to more details.", __func__);
    std::cout << tables_name.size() << database_connection;
 } catch (const std::exception &e) { throw err(e.what()); }
}

////////////////////////////////////////////////////////////////////////////////
// templates
////////////////////////////////////////////////////////////////////////////////


// template<template<typename, typename> typename MAP_T>
// MAP_T<std::string, database_obj_str::Field>
// auto
std::unordered_map<std::string, database_obj_str::Field>
database_offline::map_table(
    const std::string& table_name,
    const std::string &database_connection)
{ try {
    throw err("database_offline::%s() must be generate on compiling time. See documentation to more details.", __func__);
    std::cout << table_name << database_connection;
 } catch (const std::exception &e) { throw err(e.what()); }
}

// template<template<typename, typename> typename MAP_T>
// MAP_T<std::string, database_online::Table>
// auto
std::unordered_map<std::string, database_online::Table>
database_offline::init_database_graph(
    const std::string &database_connection)
{ try {
    throw err("database_offline::%s() must be generate on compiling time. See documentation to more details.", __func__);
    std::cout << database_connection;
 } catch (const std::exception &e) { throw err(e.what()); }
}

#endif // DATABASE_OFFLINE_HPP
/*
// void database_online::walk()
{ try {
 } catch (const std::exception &e) { throw err(e.what()); }
}
*/
/*
 SQLs 



*/
