#include "database.hpp"

////////////////////////////////////////////////////////////////////////////////
// public global variables
////////////////////////////////////////////////////////////////////////////////
std::unordered_map<std::string, database_online::Table> database_online::database_graph;

////////////////////////////////////////////////////////////////////////////////
// private functions - head
////////////////////////////////////////////////////////////////////////////////
std::string 
database_online::get_primary_key(const std::string& table_name, const std::string& database_connection)
{ try {
	if(table_name.empty()) throw err("table name is empty");

	const std::string sql = "SELECT kcu.column_name "
	"FROM "
		"information_schema.table_constraints AS tc "
		"JOIN information_schema.key_column_usage AS kcu "
			"ON tc.constraint_name = kcu.constraint_name "
			"AND tc.table_schema = kcu.table_schema "
		"JOIN information_schema.constraint_column_usage AS ccu "
			"ON ccu.constraint_name = tc.constraint_name "
			"AND ccu.table_schema = tc.table_schema "
	"WHERE tc.constraint_type = 'PRIMARY KEY' "
		"AND tc.table_name=" + database::quote(table_name) + ";";
	
	const auto result = database::selectr1(sql, database_connection); // executa o sql e pega o resultado

	return result[0].is_null() ? "" : result[0].as<std::string>();
 } catch (const std::exception &e) { throw err(e.what()); }
}


void 
database_online::init_database_graph(
	std::unordered_map<std::string, Table>& database_graph,
	const std::string& database_connection)
{ try {
	const auto sql = make_sql_init_database_graph();
	const auto R = database::selectr(sql, database_connection);

	for(const auto& row : R) {
		const auto& table_name = row[0].is_null() ? "" : row[0].as<std::string>();
		const auto& column_name = row[1].is_null() ? "" : row[1].as<std::string>();
		const auto& foreign_table_name = row[2].is_null() ? "" : row[2].as<std::string>();
		const auto& foreign_column_name = row[3].is_null() ? "" : row[3].as<std::string>();

		// cria um grafo não-direcional. Cria a conexão em ambas as direções.
		auto& table = database_graph[table_name];
		table.set_name(table_name);
		const std::tuple<std::string, std::string> col_name = {foreign_column_name, column_name};
		table.foreign_table.emplace(foreign_table_name, col_name);

		auto& ftable = database_graph[foreign_table_name];
		ftable.set_name(foreign_table_name);
		const std::tuple<std::string, std::string> fcol_name = {column_name, foreign_column_name};
		ftable.foreign_table.emplace(table_name, fcol_name);
	}
 } catch (const std::exception &e) { throw err(e.what()); }
}

std::string 
database_online::init_database_graph_str(const std::string &database_connection)
{ try {
	std::unordered_map<std::string, Table> db_graph;
	init_database_graph(db_graph, database_connection); // inicializa o banco de dados

	////////////////////////////////////////////////////////////////////////////////
	//
	////////////////////////////////////////////////////////////////////////////////
	std::string database_graph_str = "{ ";

	for(const auto& [table_name, table] : db_graph)
		database_graph_str += "{ \"" + table_name + "\", " + table.to_str() + " }, ";
	
	database_graph_str.pop_back(); database_graph_str.pop_back(); // remove the string ", "
	database_graph_str += " }"; // finaliza a string -> fechando o map
	return database_graph_str;
 } catch (const std::exception &e) { throw err(e.what()); }
}

////////////////////////////////////////////////////////////////////////////////
// inner joint functions
////////////////////////////////////////////////////////////////////////////////
std::string 
database_online::join(const std::unordered_set<std::string>& target_table)
{ try {
	if(target_table.empty()) throw err("Target tables do create the inner join cannot be empty.");
	if(target_table.size() < 2) throw err("Target tables do create the inner join must be greater than one table. %zu", target_table.size());

	// verifica se todas as tabelas do target_table estão dentro do database_graph -> ou seja, se estão dentro do grafo que representa o banco de dados
	for(const auto& table : target_table)
		if(!database_graph.count(table))
			throw err("NO table in database graph representation. Table: \"%s\".", table.c_str());
	
	
	////////////////////////////////////////////////////////////////////////////////
	// cria o grafo virtual das tabelas alvos &
	// caso o grafo virtual possa ser unido sem auxílio de nenhuma outra tabela não alvo, cria o sql INNER JOIN e retorna.
	////////////////////////////////////////////////////////////////////////////////
	auto [virtual_graph_target, has_one_node] = check_if_target_table_join_themselves(target_table);
	// std::cout << "size of virtual_graph_target: " << virtual_graph_target.size() << "\n";
	if(has_one_node)
		return inner_join_make_sql(virtual_graph_target);
	
	////////////////////////////////////////////////////////////////////////////////
	// cria o grafo de busca para os nós não virtuais
	// o grafo de busca contém apenas as tabelas não-alvos
	////////////////////////////////////////////////////////////////////////////////
	auto inner_join_graph_search = inner_join_create_graph_search(virtual_graph_target);

	const auto [connector_join_node, exists_spanning_tree] = inner_join_graph_search_run(virtual_graph_target, inner_join_graph_search);

	// std::cout << "\nis_one_join_node: " << u::to_str(exists_spanning_tree) << "\n";

	// std::cout << "\nconnector_join_node\n[";
	// for(const auto& s : connector_join_node) { std::cout << s << ", "; }
	// std::cout << "]\n";

	// std::cout << "\ngraph search:\n";
	// for(auto [n, g] : inner_join_graph_search) {
	// 	g.print(virtual_graph_target);
	// }

	if(!exists_spanning_tree) {
		std::string str;
		for(const auto& tables : target_table) str += tables + ", ";
		str.pop_back(); str.pop_back(); // retira os characteres ", "
		throw err("NO exists spanning tree that connect all virtual tables. Tables to connect: %s", str.c_str()); // todo fazer uma classe de erro específico
	}
	auto spanning_tree = inner_join_get_shortest_spanning_tree(virtual_graph_target, inner_join_graph_search, connector_join_node);

	////////////////////////////////////////////////////////////////////////////////
	// insere todos os nós alvos na spanning_tree
	// necessário, pois pode ser, que por causa da algutinação feita no grafo virtual,
	// algumas tabelas alvos não foram inseridas na árvore geradora mínima
	////////////////////////////////////////////////////////////////////////////////
	for(const auto& node : target_table)
		spanning_tree.emplace(node);

	////////////////////////////////////////////////////////////////////////////////
	// gera a parte FROM do sql, que contém o INNER JOIN.
	// o sql gerado NÃO contém o comando FROM
	////////////////////////////////////////////////////////////////////////////////
	auto sql = inner_join_make_sql(spanning_tree);

	// std::cout << "\nspanning_tree\n[";
	// for(const auto& s : spanning_tree) { std::cout << s << ", "; }
	// std::cout << "]\n";

	// std::cout << "\nSQL INNER JOIN:\n";
	// std::cout << sql << "\n";

	return sql;
 } catch (const std::exception &e) { throw err(e.what()); }
}

std::tuple<std::vector<database_online::Inner_Join_Table_Virtual_Node>, bool>
database_online::check_if_target_table_join_themselves(const std::unordered_set<std::string>& target_table)
{ try {
	std::unordered_map<std::string, bool> to_reach;
	for(const auto& table : target_table)
		to_reach[table] = true;

	// realiza a aglutinação de todos os nós alvos que podem se unir diretamente
	std::vector<Inner_Join_Table_Virtual_Node> virtual_target_table;
	for(auto vtable_id = -1; auto& [table, is_to_reach] : to_reach) {
		if(!is_to_reach) continue; // caso o nó já está inserido na tabela virtual

		is_to_reach = false; // retira o nó alvo que será buscado do conjunto dos nós alvos que ainda não foram encontrados
		Inner_Join_Table_Virtual_Node vnode (++vtable_id); // adiciona o id na tabela virtual e incrementa a variável que gerencia os ids
		vnode.add_table(table); // insere o nome da tabela como uma tabela do nó virtual
		virtual_target_table.push_back(vnode); // insere o nó virtual no último nó da tabela virtual dos nós alvos

		check_if_target_table_join_themselves_run(table, to_reach, virtual_target_table);
	}

	if(virtual_target_table.size() == 1) return { virtual_target_table, true };
	return { virtual_target_table, false };
 } catch (const std::exception &e) { throw err(e.what()); }
}

void 
database_online::check_if_target_table_join_themselves_run(
		const std::string& current_node,
		std::unordered_map<std::string, bool>& to_reach,
		std::vector<Inner_Join_Table_Virtual_Node>& virtual_target_table)
{ try {
	////////////////////////////////////////////////////////////////////////////////
	// realiza uma busca em largura para verificação se o nó alvo se conecta diretamente
	// a outros nós alvos.
	////////////////////////////////////////////////////////////////////////////////
	std::unordered_set<std::string> reached;
	const auto& foreign_table = database_graph[current_node].foreign_table;
	for(auto& [table, is_to_reach] : to_reach) {
		if(!is_to_reach) continue; // o nó já foi encontrado e inserido na tabela virtual

		const auto& range = foreign_table.equal_range(table);
		if(range.first == range.second) continue; // não existe ligação entre as tabelas

		////////////////////////////////////////////////////////////////////////////////
		// insere o nó na tabela virtual
		////////////////////////////////////////////////////////////////////////////////
		is_to_reach = false; // marca o nó como já encontrado
		reached.emplace(table); // insere o vértice alvo encontrado, no conjunto de véritices encontrados para o vértice atual
		auto& vnode = virtual_target_table.back(); // recupera o nó virtual atual
		vnode.add_table(table); // adiciona a tabela alvo encontrada no nó virtual da tabela que a alcançou
		Inner_Join_Path_Node path_node (current_node, table);

		for(auto it = range.first; it != range.second; ++it) // insere todos as colunas de ligação entre as tabelas
			path_node.add(it->second);

		vnode.path.push_back(path_node); // insere o caminho com todas as colunas de ligação entre as tabelas no caminho
	}

	if(reached.empty()) return; // não tem nós alvos que se conectam ao véritce atual que está sendo verificado
	
	////////////////////////////////////////////////////////////////////////////////
	// caso haja conexão com algum vértice alvo.
	// realiza a busca de largura nos nós encontrados
	////////////////////////////////////////////////////////////////////////////////
	for(const auto& table : reached)
		check_if_target_table_join_themselves_run(table, to_reach, virtual_target_table);
	
 } catch (const std::exception &e) { throw err(e.what()); }
}

std::string
database_online::inner_join_make_sql(const std::vector<database_online::Inner_Join_Table_Virtual_Node>& virtual_table)
{ try {
	if(virtual_table.size() != 1) throw err("Virtual table has more than one node. %zu", virtual_table.size());
	
	const auto& path = virtual_table.front().path; // recebe o vetor que contem as tabelas que serão unidas no inner join
	std::string sql = path.front().table_left; // SQL DO INNER JOIN
	for (const auto& inner_join_node : path) {
		sql += " JOIN " + inner_join_node.table_right + " ON "; // JOIN = INNER JOIN -> sql standard

		for(const auto& [col_table_right, col_table_left] : inner_join_node.column_join) {
			sql += inner_join_node.table_right + "." + col_table_right + 
				"=" + inner_join_node.table_left + "." + col_table_left +
				" AND ";
		}

		sql.pop_back(); sql.pop_back(); sql.pop_back(); sql.pop_back(); sql.pop_back(); // strips " AND " from sql
	}

	return sql;
 } catch (const std::exception &e) { throw err(e.what()); }
}

std::unordered_map<std::string, database_online::Inner_Join_Graph_Search_Node>
database_online::inner_join_create_graph_search(
	const std::vector<database_online::Inner_Join_Table_Virtual_Node>& virtual_graph_target)
{ try {
	std::unordered_map<std::string, Inner_Join_Graph_Search_Node> graph_search;
	for(const auto& [table_name, table_obj] : database_graph) {
			Inner_Join_Graph_Search_Node node (table_name, virtual_graph_target);
			graph_search.emplace(table_name, node);
	}
	return graph_search;
 } catch (const std::exception &e) { throw err(e.what()); }
}

std::tuple<std::vector<std::string>, bool>
database_online::inner_join_graph_search_run(
	const std::vector<database_online::Inner_Join_Table_Virtual_Node>& virtual_graph_target,
	std::unordered_map<std::string, database_online::Inner_Join_Graph_Search_Node>& graph_search)
{ try {
	////////////////////////////////////////////////////////////////////////////////
	// inicializa a busca.
	// realiza o primeiro loop, que é encontrar quais são os nós não-alvos que estão ligados aos nós alvos
	//////////////////////////////////////////////////////////////////////////////
	// o primeiro elemento da tuple é o nome do current_node e o segundo elemento da tuple é o nome da tabela que chamou o current_node
	std::unordered_map<int, std::unordered_set<std::string>> current_node;
	std::unordered_map<int, std::unordered_set<std::string>> next_node;

	for(const auto& vtable : virtual_graph_target) {
		std::unordered_set<std::string> vtable_join_node;
		for(const auto& table : vtable.for_range_table()) { // recebe quais são as tabelas alvos - raízes da busca
			for(const auto& [join_table, join_column] : database_graph.at(table).foreign_table) {
				if(!vtable.has(join_table)) { // a table não faz parte do conjunto da tabela virtual que está sendo interada.
					graph_search.at(join_table).join(vtable.get_id(), table, 0); // a distância de uma tabela virtual até ela mesma é sempre 0
					vtable_join_node.emplace(join_table);
				}
			}
		}
		current_node.emplace(vtable.get_id() ,vtable_join_node);
	}

	////////////////////////////////////////////////////////////////////////////////
	// realiza a busca por largura para encontrar o primeiro nó que conecta todas as tabelas alvos.
	// encontrado este nó, se tem deste nó até as tabelas alvos o menor caminho.
	// pois o menor caminho neste caso é o caminho que contenha menos nó, pois
	// todos os nós e todos os vértices tem o mesmo peso, e tal peso é positivo.
	////////////////////////////////////////////////////////////////////////////////

	// for(auto [n, g] : graph_search) {
	//     g.print(virtual_graph_target);
	// }

	// auto c = 0;
	while(true) {
		// ++c;

		// std::cout << "\nPRINT PATH CURRENT_NODE " << c << "\n";
		// for(auto i = 0; const auto& vtable : virtual_graph_target) {
		//     std::string u = "| ";
		//     for(auto f : vtable.for_range_table())
		//         u += f + " | ";
		//     std::cout << "\nLevel: " << u << "\n\t[";
		//     for(auto i : current_node[i]) {
		//         std::cout << i << ", ";
		//     }
		//     std::cout << "]\n";
		//     ++i;
		// }


		const auto [connect_join_node, has_stree] = 
			inner_join_check_if_exists_spanning_tree_to_virtual_graph_target(graph_search, virtual_graph_target);
		if(has_stree)
			return { connect_join_node, true };
		if(inner_join_is_current_node_empty(current_node)) // não existe caminho que ligue todas as tabelas virtuais do grafo.
			return { {}, false };

		for(const auto& vtable : virtual_graph_target) {
			std::unordered_set<std::string> vtable_join_node;
			for(const auto& node : current_node.at(vtable.get_id())) { // recebe quais são as tabelas alvos - raízes da busca
				auto node_distance = graph_search.at(node).get_distance(vtable.get_id()); // recebe a distância do nó atual para a tabela virtual corrente
				for(const auto& [join_table, join_column] : database_graph.at(node).foreign_table) {
					// a table não faz parte da tabela virtual que está sendo analisada & o nó ainda não foi percorrido em iterações anteriores
					if(graph_search.at(join_table).compare_distance_to_virtual_table(vtable.get_id(), node_distance)) {
						graph_search.at(join_table).join(vtable.get_id(), node, node_distance);
						vtable_join_node.emplace(join_table);
					}
				}
			}
			next_node.emplace(vtable.get_id(), vtable_join_node);
		}
		current_node = std::move(next_node);
		// next_node.clear(); // move already clear the next_node
	}
	return {{}, false};
 } catch (const std::exception &e) { throw err(e.what()); }
}

std::tuple<std::vector<std::string>, bool>
database_online::inner_join_check_if_exists_spanning_tree_to_virtual_graph_target(
	const std::unordered_map<std::string, database_online::Inner_Join_Graph_Search_Node>& graph_search,
	const std::vector<database_online::Inner_Join_Table_Virtual_Node>& virtual_graph_target)
{ try {
	bool is_stree = false;
	std::vector<std::string> connect_join_node;

	/**
	 * Para descobrir se existe uma árvore geradora mínima para os nós alvos virtuais, basta descobrir se um nó do grafo de busca
	 * se conecta com eles.
	 * Caso um único nó se conecta, existe a árvore geradora mínima, pois os nós no grafo de busca referem outros nós, e basta seguir
	 * a lista para os nós serem descobertos.
	 */
	for(const auto& [node_name, node] : graph_search) { // pesquisa cada nó do grafo de busca se ele se conecta a todos os nós-alvos
		bool is_join_to_all_virtual_tables = true;
		for(const auto& vtable : virtual_graph_target) { // pesquisa se o nó do grafo de busca se conecta com todos os nós alvos
			if(!node.is_reach(vtable.get_id())) { // se não está conectado com algum nó-alvo, continua a busca com outro nó do grafo de busca
				is_join_to_all_virtual_tables = false;
				break;
			}
		}
		if(is_join_to_all_virtual_tables) { // verifica se é um nó de conexão.
			is_stree = true;
			connect_join_node.push_back(node_name); // guarda o nome do nó de conexão
		}
	}

	return { connect_join_node, is_stree };
 } catch (const std::exception &e) { throw err(e.what()); }
}

bool
database_online::inner_join_is_current_node_empty(
	const std::unordered_map<int, std::unordered_set<std::string>>& current_node)
{ try {
	for(const auto& [vtable_id, set] : current_node)
		if(!set.empty()) 
			return false;
	
	return true;
 } catch (const std::exception &e) { throw err(e.what()); }
}

std::unordered_set<std::string>
database_online::inner_join_get_shortest_spanning_tree(
	const std::vector<database_online::Inner_Join_Table_Virtual_Node>& virtual_graph_target,
	const std::unordered_map<std::string, database_online::Inner_Join_Graph_Search_Node>& graph_search,
	const std::vector<std::string>& join_nodes)
{ try {
	std::unordered_set<std::string> best_spanning_tree;
	int best_spanning_tree_weight = std::numeric_limits<int>::max();
	for(const auto& join_node : join_nodes) {
		// std::cout << "\njoin_node: " << join_node << "\n";
		const auto [spanning_tree, tree_weight] = graph_search.at(join_node).get_virtual_spanning_tree(graph_search, virtual_graph_target);
		if(tree_weight < best_spanning_tree_weight) {
			best_spanning_tree_weight = tree_weight;
			best_spanning_tree = std::move(spanning_tree);
		}
	}

	return best_spanning_tree;
 } catch (const std::exception &e) { throw err(e.what()); }
}

std::string
database_online::inner_join_make_sql(const std::unordered_set<std::string>& spanning_tree)
{ try {
	std::unordered_set<std::string> joined; // tabelas que já fazem parte do sql do inner join
	std::unordered_set<std::string> to_join; // tableas que ainda não foram juntadas no sql do inner join

	// inicializa ambas as estruturas.
	for(bool init = true; const auto& node : spanning_tree) {
		if(init) {
			init = false;
			joined.emplace(node);
		}
		else to_join.emplace(node);
	}

	// inicializa o sql - o joined neste ponto tem somente uma tabela.
	std::string sql;
	for(const auto& node : joined) sql = node + " ";

	while(!to_join.empty()) { // enquanto ainda existirem tabelas para serem inseridas no inner join
		std::string table;
		for(const auto& node : to_join) { // encontra uma tabela que se liga a alguma das tabelas que estão no joined
			for(const auto& link_table : joined) {
				if(database_graph.at(node).inner_join(link_table)) {
					table = node;
					goto JOIN_FIND_TABLE;
				}
			}
		}

		JOIN_FIND_TABLE: // insere a tabela encontrada na string do INNER JOIN
		sql += "JOIN " + table + " ON "; // JOIN = INNER JOIN - sql standard
		for(const auto& node : joined) {
			const auto [begin, end] = database_graph.at(table).foreign_table.equal_range(node);

			for(auto it = begin; it != end; ++it) {
				const auto& [node_column, table_column] = it->second; // recebe o nome das colunas
				sql += node + "." + node_column + "=" + table + "." + table_column + " AND ";
			}
		}
		// retira a string "AND " do final do SQL
		sql.pop_back(); sql.pop_back(); sql.pop_back(); sql.pop_back();

		joined.emplace(table); // insere no grupo das tabelas que já fazem parte do INNER JOIN
		to_join.erase(table); // retira do grupo das tabelas que ainda serão inseridas no INNER JOIN
	}

	sql.pop_back(); // retira o character ' ' do final da string
	return sql;
 } catch (const std::exception &e) { throw err(e.what()); }
}
////////////////////////////////////////////////////////////////////////////////
// class database_online::Table
////////////////////////////////////////////////////////////////////////////////
database_online::Table::Table(
	const std::string& table_name,
	const std::unordered_multimap<std::string, std::tuple<std::string, std::string>>& foreign_table)
{ try {
	set_name(table_name);
	this->foreign_table = foreign_table;
 } catch (const std::exception &e) { throw err(e.what()); }
}

void 
database_online::Table::set_name(const std::string& table_name)
{ try {
	if(table_name.empty()) throw err("Table name cannot be an empty string.");
	this->name = table_name;
 } catch (const std::exception &e) { throw err(e.what()); }
}

std::string 
database_online::Table::to_str() const
{ try {
	std::string table_str = "{ "; // inicializa o map
	table_str += "\"" + this->name + "\", "; // insere o nome da tabela

	table_str += "{ ";
	for(const auto& [foreign_table_name, columns_name] : foreign_table) {
		const auto& [foreign_column_name, column_name] = columns_name;
		table_str += "{ \"" + foreign_table_name + "\", { \"" +
			foreign_column_name + "\", \"" + column_name + "\" } }, ";
	}
	table_str.pop_back(); table_str.pop_back(); // remove string ", " at the end
	table_str += " }"; // finaliza o multimap que representa o foreign_table.
	table_str += " }"; // finaliza a estrutura Table.
	return table_str;
 } catch (const std::exception &e) { throw err(e.what()); }
}
////////////////////////////////////////////////////////////////////////////////
// class database_online::Inner_Join_Graph_Search_Node
////////////////////////////////////////////////////////////////////////////////
int 
database_online::Inner_Join_Graph_Search_Node::get_distance(const int vtable_id) const
{ try {
	if(this->is_target_table && this->vtable_id == vtable_id) return 0; // a distância de um nó até ele mesmo é sempre zero
	return vtable_target.at(vtable_id).get_distance();
 } catch (const std::exception &e) { throw err(e.what()); }
}

bool 
database_online::Inner_Join_Graph_Search_Node::compare_distance_to_virtual_table(const int vtable_id, const int table_dist) const
{ try {
	if(table_dist < 0) throw err("Distance to compare is invalid. Distance: %d | table name: \"%s\"", table_dist, table_name);
	if(this->is_target_table && this->vtable_id == vtable_id) return false; // qualquer que seja o nó, a distância dele que se autoreferencia é menor que a de qualquer outro nó fora da vtable dele
	int my_dist = vtable_target.at(vtable_id).get_distance();
	if(my_dist == -1) return true; // não foi conectado com a tabela virtual em análise
	if(my_dist < 1) throw err("Invalid value of distance. Distance: %d | table name: \"%s\"", my_dist, table_name);
	if(my_dist > table_dist) return true; // a distância em comparação é mais perto da tabela virtual alvo do que a distance do nó this
	else return false; // a distância em comparação é igual ou maior da tabela virtual alvo do que a distance do nó this
 } catch (const std::exception &e) { throw err(e.what()); }
}

void 
database_online::Inner_Join_Graph_Search_Node::join(const int vtable_id, const std::string& previous_table,  const int previous_table_distance)
{ try {
	if(this->is_target_table && this->vtable_id == vtable_id) return; // não faz nada pois este nó já está dentro da vtable.
	vtable_target.at(vtable_id).join(previous_table, previous_table_distance);
 } catch (const std::exception &e) { throw err(e.what()); }
}

 bool 
 database_online::Inner_Join_Graph_Search_Node::is_reach(const int vtable_id) const
{ try {
	if(is_target_table && this->vtable_id == vtable_id) return true;
	return vtable_target.at(vtable_id).is_reach();
 } catch (const std::exception &e) { throw err(e.what()); }
}

std::tuple<std::unordered_set<std::string>, int>
database_online::Inner_Join_Graph_Search_Node::get_virtual_spanning_tree(
	const std::unordered_map<std::string, database_online::Inner_Join_Graph_Search_Node>& graph_search,
	const std::vector<database_online::Inner_Join_Table_Virtual_Node>& virtual_graph_target) const
{ try {
	std::unordered_map<int, std::vector<std::unordered_set<std::string>>> tree;

	for(const auto& vtable : virtual_graph_target) { // encontra os caminhos para cada tabela virtual do grafo virtual
		std::vector<std::unordered_set<std::string>> set_path;
		std::vector<std::string> path;
		make_path_to_generate_virtual_spanning_tree(graph_search, virtual_graph_target, vtable.get_id(), this->table_name, path, set_path);
		tree.emplace(vtable.get_id(), set_path);
	}

	std::vector<int> vtable_idx;
	for(const auto& vtable : virtual_graph_target) { // recebe todos os ids do grafo virtual alvo
		vtable_idx.push_back(vtable.get_id());
	}

	////////////////////////////////////////////////////////////////////////////////
	// gera as árvores geradoras para os nós (tableas) do grafo virtual
	// retorna a árvore geradora mínima das árvores geradas
	////////////////////////////////////////////////////////////////////////////////    
	std::vector<std::string> spanning_tree;
	std::unordered_set<std::string> best_spanning_tree;
	int best_spanning_tree_weight = std::numeric_limits<int>::max();
	make_spanning_tree_and_find_best_spanning_tree(graph_search, tree, vtable_idx, 0, spanning_tree, best_spanning_tree, best_spanning_tree_weight);

	// TODO retornar os tipos certos: std::unordered_set<std::string> best_spanning_tree & int best_spanning_tree_weight
	return { best_spanning_tree, best_spanning_tree_weight };
 } catch (const std::exception &e) { throw err(e.what()); }
}

void
database_online::Inner_Join_Graph_Search_Node::make_path_to_generate_virtual_spanning_tree(
	const std::unordered_map<std::string, database_online::Inner_Join_Graph_Search_Node>& graph_search,
	const std::vector<database_online::Inner_Join_Table_Virtual_Node>& virtual_graph_target,
	const int vtable_id,
	const std::string& table,
	std::vector<std::string>& path,
	std::vector<std::unordered_set<std::string>>& set_path) const
{ try {
	path.push_back(table); // insere a tabela no caminho
	if(graph_search.at(table).get_distance(vtable_id) == 0) {
		std::unordered_set<std::string> set;
		for(const auto& node : path) set.emplace(node);
		set_path.push_back(set);
	} else if(graph_search.at(table).get_distance(vtable_id) > 0) {
		auto path_size = path.size();
		for(const auto& previous_table : graph_search.at(table).for_range_previous_table(vtable_id)) {
			make_path_to_generate_virtual_spanning_tree(graph_search, virtual_graph_target, vtable_id, previous_table, path, set_path);
			path.erase(path.begin()+path_size, path.end()); // erase all elements bigger than path.size
		}
	} else { // is -> (graph_search.at(table).get_distance(vtable_id) < 0) {
		throw err("Distance is incorrect. Distance: %d, Table: \"%s\", virtual table id: %d, tables of virtual table: %s", 
			graph_search.at(table).get_distance(vtable_id), table.c_str(), vtable_id, virtual_graph_target.at(vtable_id).get_tables());
	}
 } catch (const std::exception &e) { throw err(e.what()); }
}

void
database_online::Inner_Join_Graph_Search_Node::make_spanning_tree_and_find_best_spanning_tree(
	const std::unordered_map<std::string, database_online::Inner_Join_Graph_Search_Node>& graph_search,
	const std::unordered_map<int, std::vector<std::unordered_set<std::string>>>& tree,
	const std::vector<int>& vtable_idx,
	const size_t vtable_idx_pos,
	std::vector<std::string>& spanning_tree,
	std::unordered_set<std::string>& best_spanning_tree,
	int& best_spanning_tree_weight) const
{ try {
	if(vtable_idx_pos >= vtable_idx.size()) { // verifica se a árvore geradora mínima gerada é melhor que a melhor até o momento
		// a utilização de um unordered_set é com o fim de eliminar os nós que são repetidos nos diversos caminhos
		// o vetor é necessário para a montagem total.
		std::unordered_set<std::string> stree;
		for(const auto& node : spanning_tree) stree.emplace(node);

		int total_weight_tree = 0;
		for(const auto& node : stree) // encontra o peso da árvore
			total_weight_tree += graph_search.at(node).get_weight();
		
		if(best_spanning_tree_weight > total_weight_tree) { // verifica se a árvore geradora mínima gerada é melhor que a atual
			best_spanning_tree = std::move(stree);
			best_spanning_tree_weight = total_weight_tree;
		}
	}
	else { // não montou a árvore geradora mínima ainda, continua a montagem dela
		const int spanning_tree_size = spanning_tree.size();
		for(const auto& path : tree.at(vtable_idx.at(vtable_idx_pos))) { // cada conjunto de nós
			for(const auto& node : path) // adiciona os nós deste caminho na árvore geradora mínima
				spanning_tree.push_back(node);


			make_spanning_tree_and_find_best_spanning_tree( // realiza a chamada recursiva para construir as combinações do caminho
				graph_search, tree, vtable_idx, 
				vtable_idx_pos+1, spanning_tree, 
				best_spanning_tree, best_spanning_tree_weight);
			
			// retira todos os elementos bigger than spanning_tree_size - necessário limpar para inserir o caminho da nova interação deste for e continuar as combinações que faltam
			spanning_tree.erase(spanning_tree.begin()+spanning_tree_size, spanning_tree.end());
		}
	}
 } catch (const std::exception &e) { throw err(e.what()); }
}


////////////////////////////////////////////////////////////////////////////////
// class database_online::Inner_Join_Graph_Search_Node_Info
////////////////////////////////////////////////////////////////////////////////
void 
database_online::Inner_Join_Graph_Search_Node_Info::join(const std::string& node, const int node_distance)
{ try {
	if(_is_reach) { // o nó já foi encontrado na iteração
		if(_distance_vtable_target != node_distance +1) { // verifica se o nó já foi encontrado e setado em outra iteração
			std::string str = "[";
			if(_previous_node.empty()) throw err("Node is reach, but its previous node is empty.");
			for(const auto& n : _previous_node) str += n + ", ";
			str.pop_back(); str.pop_back(); // remove string ", "
			throw err("Node already setting. Join previous node: \"%s\", join previous node distance: %d.\n"
				"Node distance: %d, List of previous node: %s", node.c_str(), node_distance, _distance_vtable_target, str.c_str());
		}
		// if(_previous_node.count(node)) return; // já possui o node
	} else { // primeira vez que o nó é alcançado
		_is_reach = true;
		_distance_vtable_target = node_distance +1;
	}
	_previous_node.emplace(node); // sempre quando o join é válido, o node é inserido na lista de previous_node.
 } catch (const std::exception &e) { throw err(e.what()); }
}

////////////////////////////////////////////////////////////////////////////////
// class database_online::Inner_Join_Table_Virtual_Node
////////////////////////////////////////////////////////////////////////////////
bool database_online::Inner_Join_Table_Virtual_Node::has(const std::string& table_name) const
{ try {
	if(table.count(table_name)) return true;
	return false;
 } catch (const std::exception &e) { throw err(e.what()); }
}

void database_online::Inner_Join_Table_Virtual_Node::add_table(const std::string& table_name)
{ try {
	table.emplace(table_name);
 } catch (const std::exception &e) { throw err(e.what()); }
}

std::string database_online::Inner_Join_Table_Virtual_Node::get_tables() const
{ try {
	std::string str = "| ";
	for(const auto& t : table)
		str += t + " | ";
	if(str.size() == 2) return "||"; // string representando que não tem tabelas.
	return str;
 } catch (const std::exception &e) { throw err(e.what()); }
}

/*
// void database_online::walk()
{ try {
 } catch (const std::exception &e) { throw err(e.what()); }
}
*/






























