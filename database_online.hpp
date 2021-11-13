/**
 *
 * @descripion: Contém o namespace e os templates das funções e classes do namespace database_online.
 * Não é necessário incluir este header, ele já faz parte do database.hpp.
 * o namespace database_online contém as funções de criação automática, em tempo de execução, de criação de:
 * > INNER JOINS
 * > quais são as colunas e os tipos da tabela.
 * > representação do banco de dados em um grafo, que contém as tabelas e as chaves estrangeiras.
 */
#ifndef DATABASE_ONLINE_HPP
#define DATABASE_ONLINE_HPP

/**
 * Este namespace cuida das funções que conectam com o banco de dados
 * para dar um esquema do banco de dados e facilitar a criação de classes e
 * estruturas que representam o banco de dados no programa.
 * O nome online vem do fato de que ele pega as informações do banco em tempo de execução.
 */
namespace database_online
{
    ////////////////////////////////////////////////////////////////////////////////
    // class
    ////////////////////////////////////////////////////////////////////////////////

    /**
     * @brief Representa uma tabela do banco de dados.
     * A tabela contém quais são as tabelas e quais as colunas em que existem relacionamento entre elas, ou seja,
     * que a tabela possui chave estrangeira.
     * A chave estrangeira pode ser referenciada pela Tabela, ou, pode referenciar uma coluna da tabela.
     */
    struct Table
    {
        std::string name;

        /**
         * Esta estrutura está sendo construída da seguinte maneira
         * @arg key: a chave do mapa é o nome da tabela que se comunica com a tabela.
         * pode ser uma tabela que referencia ou é referenciada pela tabela que a classe representa (this->)
         * @arg tuple<str1, str2>: são as colunas necessárias para realizar o INNER JOIN
         * @arg str1: é a coluna da tabela key, foreign_column_name que se une no inner join com a tabela que a classe representa (this->)
         * @arg str2: é a coluna da tabela que a classe representa (this->) que se liga com a tabela @arg(key)
         * exemplo:
         * name = "table1"
         * foreign_table["table2"] = {"colX", "colY"}.
         * table1 INNER JOIN table2 ON table2.colX = table1.colY
         */
        std::unordered_multimap<std::string, std::tuple<std::string, std::string>> foreign_table;


        /**
         * @brief Default construct for Table object.
         * @param table_name 
         * @param foreign_table 
         */
        Table() {}
        
        /**
         * @brief Construct a new Table object.
         * @throw u::error: se o @param(table_name) for uma string vazia.
         * @param table_name 
         * @param foreign_table 
         */
        Table(
            const std::string& table_name,
            const std::unordered_multimap<std::string, std::tuple<std::string, std::string>>& foreign_table);

        const std::unordered_multimap<std::string, std::tuple<std::string, std::string>>
        get_join_tables() const
        {
            return foreign_table;
        }

        std::unordered_multimap<std::string, std::tuple<std::string, std::string>>
        get_join_tables()
        {
            return foreign_table;
        }

        const decltype(foreign_table) &for_range_foreign_table() const
        {
            return foreign_table;
        }

        bool inner_join(const std::string &table) const
        {
            try
            {
                if (foreign_table.count(table))
                    return true;
                else
                    return false;
            }
            catch (const std::exception &e)
            {
                throw err(e.what());
            }
        }

        /**
         * @brief Retorna o conteúdo da tabela no formato de string.
         * Retorna quais são as tabelas que esta tabela (this) se conecta por chave estrangeira, e quais 
         * são as colunas que ela referencia e quais são as colunas dela que referenciam as chaves estrangeiras.
         * O formato da string é o formato de uma string que representa a inicialização de uma
         * estrutura do tipo std::multimap, onde a chave do map é a tabela referenciada e o conteúdo é uma tupla
         * que contém as colunas referenciadas e que referencia.
         * Exemplo:
         * Table1 = this
         * Table2 = tabela referenciada pela Table1.
         * ColX = coluna da tabela Table2
         * ColY = coluna da tabela Table1
         * foreign_table["table2"] = {"colX", "colY"}.
         * SQL INNER JOIN: table1 INNER JOIN table2 ON table2.colX = table1.colY
         * @return = { "Table1", { "Table2", { "ColX", "ColY" } } }
         * @return std::string 
         */
        std::string to_str() const;

        /**
         * @brief Set the name object
         * Set the name of table.
         * @throw u::error: se o @param(table_name) for uma string vazia.
         * @param table_name 
         */
        void set_name(const std::string& table_name);
    };

    /**
     * Representa o caminho no grafo para realização do INNER JOIN.
     * Não é necessário guardar as colunas de união (podem ser mais de uma), pois elas serão extraídas no momento
     * da montagem do INNER JOIN.
     * O caminho é representado pela seguinte estrutura:
     * std::vector<Inner_Join_Path_Node> path;
     * pois assim, a montagem do caminho para o inner join se torna mais eficiente.
     * A repetição dos nomes da tabela de união (table_left) é devido ao fato de que o caminho geralmente é uma árvore,
     * E a repetição favorece a construção do caminho.
     */
    struct Inner_Join_Path_Node
    {
        std::string table_left;
        std::string table_right;
        // segue o padrão firmado na classe Table
        // std::tuple<std::string, std::string> = colunas que fazem o inner join entre as tabelas.
        // table_left INNER JOIN table_right ON table_right.col_table_right = table_left.col_table_left
        // std::tuple<std::string, std::string> = { col_table_right, col_table_left };
        std::vector<std::tuple<std::string, std::string>> column_join;

        Inner_Join_Path_Node(){};

        Inner_Join_Path_Node(const std::string &table_left, const std::string &table_right)
        {
            this->table_left = table_left;
            this->table_right = table_right;
        }

        Inner_Join_Path_Node(const std::string &table_left, const std::string &table_right,
                             const std::tuple<std::string, std::string> &column)
        {
            this->table_left = table_left;
            this->table_right = table_right;
            column_join.push_back(column);
        }

        void add(const std::tuple<std::string, std::string> &column)
        {
            column_join.push_back(column);
        }

        void add(const std::string &col_table_left, const std::string &col_table_right)
        {
            column_join.push_back({col_table_right, col_table_left});
        }
    };

    /**
     * A ideia do nó virtual é fazer uma união das tabelas alvos (target) que conseguem se unir pelo INNER JOIN imediatamente,
     * sem qualquer necessicidade de união com outra tabela, que não seja também uma tabela alvo.
     */
    class Inner_Join_Table_Virtual_Node
    {
    private:
        int id;
        std::unordered_set<std::string> table;

    public:
        std::vector<Inner_Join_Path_Node> path;

        Inner_Join_Table_Virtual_Node(const int id)
        {
            this->id = id;
        }

        // // retorna um vetor que representa as tabelas que fazem parte do nó virtual
        // std::vector<std::string> get_table() {
        //     return table;
        // }

        // // retorna um vetor que representa as tabelas que fazem parte do nó virtual
        // std::vector<std::string> get_table() const {
        //     return table;
        // }

        const decltype(table) &for_range_table() const
        {
            return table;
        }

        decltype(table) &for_range_table()
        {
            return table;
        }

        // verifica se a this->table tem uma certa tabela
        bool has(const std::string &table_name) const;

        /**
         * Adiciona uma tabela em this->table
         */
        void add_table(const std::string &table_name);

        int get_id() const
        {
            return id;
        }

        /**
         * Retorna uma string contendo as tabelas que foram esta tabela virtual.
         * ex: a tabela virtual é formada pelas tabelas: "category" e "person".
         * Esta função retorna a seguinte string: "| category | person |"
         * Caso não exista tabelas, é retornado a seguinte string: "||"
         */
        std::string get_tables() const;
    };

    /**
     * Nó (vértice) do grafo de busca para realização do INNER JOIN.
     * Este nó é construído exclusivamente para a busca dos caminhos que farão o INNER JOIN.
     * São duas estruturas: Inner_Join_Graph_Search_Node_Info e Inner_Join_Graph_Search_Node
     */
    class Inner_Join_Graph_Search_Node_Info
    {
    private:
        bool _is_reach;              // se existe conexão com o nó virtual
        int _distance_vtable_target; // número de nós de distância -> inicía com o valor zero.
        // map<key, value>: a chave do map é o nome da tabela que é o previous_node, e o valor é o caminho
        std::unordered_set<std::string> _previous_node;

    public:
        Inner_Join_Graph_Search_Node_Info()
        {
            _is_reach = false;
            _distance_vtable_target = -1;
        }

        bool is_reach() const
        {
            return _is_reach;
        }

        int get_distance() const
        {
            return _distance_vtable_target;
        }

        void join(const std::string &node,  const int node_distance);

        const decltype(_previous_node) &for_range_previous_table() const
        {
            return _previous_node;
        }

        void print() const
        {
            std::string str = "\t\t\tis reach: " + u::to_str(_is_reach);
            str += "\n\t\t\tdistance to vtable target: " + std::to_string(_distance_vtable_target);
            str += "\n\t\t\t_previous_node: [";
            if(!_previous_node.empty()) {
                for (const auto &node : _previous_node)
                    str += node + ", ";
                str.pop_back(); str.pop_back(); // remove at the endo of string str -> ", "
            }
            str += "]";
            std::cout << str;
        }
    };
    class Inner_Join_Graph_Search_Node
    {
    private:
        std::string table_name;
        std::unordered_map<int, Inner_Join_Graph_Search_Node_Info> vtable_target;
        bool is_target_table;
        int vtable_id; // o id da virtual table do node, quando ele é uma target_table
        /**
         * O peso é importante, pois um nó (table) será considerada adicional no INNER JOIN somente se ela não for uma tabela alvo do INNER JOIN.
         * Neste caso, o ideal é reduzir o máximo o peso total das tabelas não essenciais.
         * O valor padrão como 1, indica que é necessário reduzir o número de tabelas.
         * No futuro, cada tabela poderá ter um peso diferente, de acordo com o seu tamanho.
         */
        int weight; // se o node for um target_table, weight = 0; se não: weight = 1.
    public:
        // template<typename ARRAY_T>
        // Inner_Join_Graph_Search_Node(const ARRAY_T& table_target);
        template <template <typename> typename ARRAY_T>
        Inner_Join_Graph_Search_Node(const std::string &table_name, const ARRAY_T<Inner_Join_Table_Virtual_Node> &virtual_graph);

        /**
         * Retorna se o nó foi encontrado na busca para a tabela virtual.
         * Caso a tabela faça parte da tabela virtual analisada, o resultado é sempre true.
         */
        bool is_reach(const int vtable_id) const;

        /**
         * Une dois nós (tabelas) em relação a uma mesma tabela (nó) virtual.
         * A distância do nó em relação a tabela virtual será a distancia da tabela prévia +1.
         * Caso a tabela já tenha sido adicionada como tabela prévia, para o caminho em relação a tabela virtual, nenhuma operação é realizada.
         */
        void join(const int vtable_id, const std::string &previous_table, const int previous_table_distance);

        /**
         * Compara a distância entre dois nós (tables) até uma tabela virtual.
         * @arg vtable_id: id da tabela virtual que se analisará a distância entre os nós.
         * @arg table_dist: distância da tabela virtual.
         * @return: true: se a distância de table_dist é menor que a distância do nó this.
         * Ou, se o nó this não é conectado ainda a tabela virtual que se analisa.
         * false: se o nó this, faz parte da tabela virtual em análise.
         * Ou, se a distância do nó this é menor, ou seja, mais próximo da table virtual em análise do que a distância passada.
         */
        bool compare_distance_to_virtual_table(const int vtable_id, const int table_dist) const;

        int get_weight() const
        {
            return weight;
        }

        /**
         * Número de arestas até a o nó alvo da tabela virtual.
         * Se o nó (table) faz parte da table virtual em análise, a distância é zero.
         */
        int get_distance(const int vtable_id) const; // refazer para o caso do nó target-table

        const std::unordered_set<std::string> &for_range_previous_table(const int vtable_id) const
        {
            try
            {
                return vtable_target.at(vtable_id).for_range_previous_table();
            }
            catch (const std::exception &e)
            {
                throw err(e.what());
            }
        }

        /**
         *
         */
        void print() const
        {
            try
            {
                std::cout << "Inner_Join_Graph_Search_Node:: (" << this->table_name << ")\n";
                std::cout << "\tis target table: " << u::to_str(is_target_table) << "\n";
                std::cout << "\tvtable_id: " << vtable_id << "\n";
                std::cout << "\tweight: " << weight << "\n";
                std::cout << "\tvirtual tables target:\n";
                for (const auto &[vtable_id, vtable] : vtable_target)
                {
                    std::cout << "\t\tvtable_id: " << vtable_id << "{\n";
                    vtable.print();
                    std::cout << "\n\t\t}\n";
                }
                std::cout << "\n";
            }
            catch (const std::exception &e)
            {
                throw err(e.what());
            }
        }

        /**
         *
         */
        template <template <typename> typename ARRAY_T>
        void print(const ARRAY_T<database_online::Inner_Join_Table_Virtual_Node> &virtual_graph_target) const;

        /**
         * Retorna a menor árvore geradora mínima para o grafo virtual.
         * Ou seja, a menor árvore que liga todos os nós do grafo virtual.
         * o segundo elemento da tupla, representa o peso da árvore gerada.
         */
        std::tuple<std::unordered_set<std::string>, int>
        get_virtual_spanning_tree(
            const std::unordered_map<std::string, database_online::Inner_Join_Graph_Search_Node> &graph_search,
            const std::vector<database_online::Inner_Join_Table_Virtual_Node> &virtual_graph_target) const;

        ////////////////////////////////////////////////////////////////////////////////
        // private functions
        ////////////////////////////////////////////////////////////////////////////////
    private:
        /**
         * Gera os caminhos que ligam o nó até os nós do grafo virtual.
         * Gera todas as combinações válidas dos caminhos que foram gerados no grafo de busca.
         */
        void make_path_to_generate_virtual_spanning_tree(
            const std::unordered_map<std::string, database_online::Inner_Join_Graph_Search_Node> &graph_search,
            const std::vector<database_online::Inner_Join_Table_Virtual_Node> &virtual_graph_target,
            const int vtable_id,
            const std::string &table,
            std::vector<std::string> &path,
            std::vector<std::unordered_set<std::string>> &set_path) const;

        /**
         * Gera as árvores geradoras mínimas para os nós do grafo virtual.
         * Retorna a menor árvore gerada e o peso dela.
         */
        void make_spanning_tree_and_find_best_spanning_tree(
            const std::unordered_map<std::string, database_online::Inner_Join_Graph_Search_Node> &graph_search,
            const std::unordered_map<int, std::vector<std::unordered_set<std::string>>> &tree,
            const std::vector<int> &vtable_idx,
            const size_t vtable_idx_pos,
            std::vector<std::string> &spanning_tree,
            std::unordered_set<std::string> &best_spanning_tree,
            int &best_spanning_tree_weight) const;
    };

    ////////////////////////////////////////////////////////////////////////////////
    // variables
    ////////////////////////////////////////////////////////////////////////////////
    extern std::unordered_map<std::string, Table> database_graph; // init in file online.cpp

    ////////////////////////////////////////////////////////////////////////////////
    // functions
    ////////////////////////////////////////////////////////////////////////////////

    /**
     * Retorna o nome das colunas da tabela.
     * @obs: o nome das colunas será inserido na estrutura ARRAY_T pela função: push_back().
     * @arg table_name: o nome da tabela que se deseja recuperar o nome das colunas.
     * @arg table_scheme: o nome do esquema em que fica a tabela no banco de dados.
     * @return: estrutura, vetor, lista, etc. que contenha o nome da das colunas da tabela.
     */
    template <template <typename> typename ARRAY_T>
    ARRAY_T<std::string> get_column_name(
        const std::string &table_name, const std::string &table_scheme = "public",
        const std::string &database_connection = database::database_connection);

    /**
     * Retorna um mapa que representa a tabela.
     * As chaves do mapa serão o nome das colunas da tabela.
     * O número de chaves será o número de colunas da tabela.
     * O map é inicializado com todos os valores das chaves sendo uma string vazia.
     * Ex: map["id"] = ""; é inserido na seguinte forma no map: map.emplace("id", "");
     * @obs: A função que inicializa o map é a função emplace(key, value)
     * @arg table_name: o nome da tabela que se deseja mapear.
     * @arg table_scheme: o nome do esquema em que fica a tabela no banco de dados.
     * @return: estrutura, map, unordered_map, etc. que mapeará a tabela.
     */
    template <template <typename, typename> typename MAP_T>
    MAP_T<std::string, std::string> get_map_table_str(
        const std::string &table_name, const std::string &table_scheme = "public", 
        const std::string &database_connection = database::database_connection);

    /**
     * Retorna o nome da coluna que representa a chave primária (primary key) da tabela.
     * @arg table_name: o nome da tabela que se deseja recuperar o nome das colunas.
     * @return: o nome da coluna que representa a chave primária da tabela.
     * Se a tabela não tem chave primária, é retornado uma string vazia "".
     */
    std::string get_primary_key(const std::string &table_name, const std::string &database_connection = database::database_connection);

    ////////////////////////////////////////////////////////////////////////////////
    // functions of inner join database
    ////////////////////////////////////////////////////////////////////////////////
    /**
     * Inicializa a representação do banco de dados do namespace.
     * Inicializa a variável database.
     * A inicialização do banco de dados, quer dizer representar as tables do scheme.
     * @obs: NÃO reincializa o mapa database_graph passado.
     * @param database_graph: variável que será inicializada com o banco de dados cuja conexão é @arg(database_connection)
     * @param database_connection: string que representa a conexão o parâmetro para se conectar com banco de dados.
     */
    void init_database_graph(
        std::unordered_map<std::string, Table>& database_graph = database_online::database_graph,
        const std::string &database_connection = database::database_connection);

    /**
     * @brief Retorna uma string que contém o esquema todo do database_graph.
     * Esta string é utilizada para se inicializar em tempo de compilação o database_graph.
     * @target: Essa string contém os valores da variável database_graph para que a busca no banco de dados
     * não necessite ser feita em tempo de execução.
     * A string já é em formato tal que simboliza como uma pessoa escreveria em código C++ como iniciar a variável database_graph
     * com seus respectivos valores.
     * @param database_connection 
     * @return std::string 
     */
    std::string init_database_graph_str(const std::string &database_connection = database::database_connection);

    /**
     * @brief Retorna a string que representa o sql que faz o seguinte.
     * Retorna quais as tabelas que contém chave estrangeira e quais são as tabelas e as colunas que ela referencia.
     * @return std::string 
     */
    std::string inline make_sql_init_database_graph();

    /**
     */
    std::string join(const std::unordered_set<std::string> &target_table);

    /**
     * FUNÇÃO INTERNA DO JOIN
     * Verifica se as tabelas alvos passadas para a função join() podem ser unidas sem o auxílio, intermediação,
     * de nenhuma outra tabela alvo.
     * Aglutina, uni, as tabelas alvos que podem ser unidas, sofrer o inner join, sem o auxílio de nenhuma outra tabela não alvo.
     * @return: um vetor com as tabelas virtuais que representam as tabelas alvo algutinadas (unidas).
     */
    std::tuple<std::vector<Inner_Join_Table_Virtual_Node>, bool>
    check_if_target_table_join_themselves(const std::unordered_set<std::string> &target_table);

    /**
     * FUNÇÃO INTERNA DO JOIN
     * Função que realiza de fato a união (aglutinação) das tabelas alvos que podem ser unidas, sofrer o inner join, sem o auxílio
     * de nenhuma outra tabela não alvo.
     */
    void check_if_target_table_join_themselves_run(
        const std::string &current_node,
        std::unordered_map<std::string, bool> &to_reach,
        std::vector<Inner_Join_Table_Virtual_Node> &virtual_target_table);

    /**
     * FUNÇÃO INTERNA DO JOIN
     * Monta o sql do INNER JOIN, somente a parte do inner JOIN, não coloca o FROM no sql.
     * ex: realiza o join de duas tabelas: product e product_type: join("product", "product_type") ->
     * virtual_table será composta pelas tabelas alvos: "product", "product_type" e a saída da função inner_join_make_sql() será:
     * "product INNER JOIN product_type ON product_type.id=product.produt_type_id"
     * @obs: esta função funciona apenas quando as tableas alvos, passadas para a função join(),
     * podem ser todas alcançadas sem intermediação, ou seja, sem mediação ou INNER JOIN, com qualquer
     * outra tabela que não seja alvo.
     * @arg virtual_table: banco de dados virtual que contém as tabelas alvos, no caso em que todas podem ser ligadas (unidas) diretamente.
     * se: virtual_table.size() > 1. -> esta função retorna erro.
     * @return: string contendo o sql do inner join entre as tabelas alvos.
     */
    std::string inner_join_make_sql(const std::vector<Inner_Join_Table_Virtual_Node> &virtual_table);

    /**
     * FUNÇÃO INTERNA DO JOIN
     * Monta o sql do INNER JOIN, somente a parte do inner JOIN, não coloca o FROM no sql.
     * ex: realiza o join de duas tabelas: product e product_type: join("product", "product_type") ->
     * virtual_table será composta pelas tabelas alvos: "product", "product_type" e a saída da função inner_join_make_sql() será:
     * "product INNER JOIN product_type ON product_type.id=product.produt_type_id"
     * Esta função é para o caso de quando o inner join necessita de utilizar outras tabelas que não as tabelas alvos para fazer
     * a conexão entre todas as tabelas alvos.
     * Tenta conectar o máximo de colunas possíveis para a árvore geradora mínima dos nós (tables) alvos passada.
     */
    std::string inner_join_make_sql(const std::unordered_set<std::string> &spanning_tree);

    /**
     * Cria um grafo de busca.
     * Neste grafo se encontram somente os nós que não fazem parte dos nós-alvos, ou seja, as tabelas alvos,
     * As tabelas alvos são as tabelas que fazem parte do select que contém o inner join.
     */
    std::unordered_map<std::string, Inner_Join_Graph_Search_Node>
    inner_join_create_graph_search(
        const std::vector<Inner_Join_Table_Virtual_Node> &virtual_graph_target);

    /**
     * Gerencia a busca no grafo de busca, para gerar a árvore mínima para ligar todas as tabelas (nós) alvos.
     */
    std::tuple<std::vector<std::string>, bool> inner_join_graph_search_run(
        const std::vector<database_online::Inner_Join_Table_Virtual_Node> &virtual_graph_target,
        std::unordered_map<std::string, database_online::Inner_Join_Graph_Search_Node> &graph_search);

    /**
     * Verifica se existe uma árvore geradora mínima que conecta todos os nós(tabelas) do grafo virtual.
     * o virtual_graph_target é o grafo virtual formado pelas tabelas virtuais que por sua vez é formado pelas tabelas alvos.
     * As tabelas alvos são as tabelas que são passadas para a função join(A, ..., N) para serem unidas por INNER JOIN.
     * @arg graph_search: o grafo de busca do inner join. é formado pelas tabelas não alvo que fazem parto do banco de dados.
     * Cada nó deste grafo é uma tabela.
     * @return: uma tupla com as seguintes variáveis:
     * std::vector<std::string>: vetor com o nome dos nós do grafo de busca, que são nós de conexão.
     * Não contém todos os nós da árvore geradora mínima, apenas o nó de conexão.
     * O nó de conexão é o nó do grafo de busca que identifica que a árvore existe, ou seja, é o último nó que é adiconado a árvore.
     * bool: true -> if exists the spanning tree.
     * false -> otherwise.
     */
    std::tuple<std::vector<std::string>, bool>
    inner_join_check_if_exists_spanning_tree_to_virtual_graph_target(
        const std::unordered_map<std::string, database_online::Inner_Join_Graph_Search_Node> &graph_search,
        const std::vector<database_online::Inner_Join_Table_Virtual_Node> &vtable_target);

    /**
     * Esta função verifica se ainda existem nós a serem alcançados na busca por largura da função inner_join_graph_search_run()
     * @arg current_node: é a estrutura que contém os próximos nós a serem examinados de cada interação das tabelas do grafo virtual alvo.
     * @return: true: se não existem mais nós a serem alcançados.
     * false: se ainda existem nós a serem alcançados.
     */
    // bool inner_join_is_current_node_empty(const std::vector<std::unordered_set<std::string>> &current_node);
    bool inner_join_is_current_node_empty(const std::unordered_map<int, std::unordered_set<std::string>>& current_node);

    /**
     * Consegue a árvore geradora mínima para as tabelas virtuais
     */
    std::unordered_set<std::string>
    inner_join_get_shortest_spanning_tree(
        const std::vector<database_online::Inner_Join_Table_Virtual_Node> &virtual_graph_target,
        const std::unordered_map<std::string, database_online::Inner_Join_Graph_Search_Node> &graph_search,
        const std::vector<std::string> &join_nodes);

    /**
     * Verifica se a árvore gerada até o momento é mais leve, ou seja, menor de acordo com a função de pesos, que a melhor árvore geradora mínima.
     * @arg graph_search: grafo de busca que contém os nós.
     * @arg spanning_tree: árvore geradora mínima gerada até o momento da busca.
     * @arg path: caminho ou pedaço que será adiconado a árvore geradora mínima criada até o momento.
     * Verifica se na construção da árvore geradora mínima, a adição de mais um caminho, fará com que ela fique píor (mais pesada) que a melhor
     * árvore geradora mínima encontrada até o momento.
     * @arg best_spanning_tree_weight: peso da melhor árvore geradora mínima gerada.
     * @return: true -> se e somente se o peso da spanning_tree + path é menor que o da melhor árvore geradora mínima.
     * false: em todos os outros casos, ou seja, se for igual ou maior.
     */
    template <template <typename, typename> typename MAP_T, template <typename> typename ARRAY_T1, template <typename> typename ARRAY_T2>
    bool inner_join_is_this_tree_lighter_than_best_tree(
        const MAP_T<std::string, database_online::Inner_Join_Graph_Search_Node> &graph_search,
        const ARRAY_T1<std::string> &spanning_tree,
        const ARRAY_T2<std::string> &path,
        const int best_spanning_tree_weight);
}


////////////////////////////////////////////////////////////////////////////////
// namespace database_online
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Implementation of templates and inline functions
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//  general functions:: templates and inline functions
////////////////////////////////////////////////////////////////////////////////
std::string 
database_online::make_sql_init_database_graph()
{ try {
    return "SELECT "
			"tc.table_name, "
			"kcu.column_name, "
			"ccu.table_name AS foreign_table_name, "
			"ccu.column_name AS foreign_column_name "
		"FROM "
				"information_schema.table_constraints AS tc "
			"JOIN information_schema.key_column_usage AS kcu "
				"ON tc.constraint_name = kcu.constraint_name "
				"AND tc.table_schema = kcu.table_schema "
			"JOIN information_schema.constraint_column_usage AS ccu "
				"ON ccu.constraint_name = tc.constraint_name "
				"AND ccu.table_schema = tc.table_schema "
		"WHERE tc.constraint_type = 'FOREIGN KEY';";
 } catch (const std::exception &e) { throw err(e.what()); }
}
////////////////////////////////////////////////////////////////////////////////
// templates
////////////////////////////////////////////////////////////////////////////////
template <template <typename> typename ARRAY_T>
ARRAY_T<std::string> database_online::get_column_name(
    const std::string &table_name, const std::string &table_scheme, const std::string &database_connection)
{
    try
    {
        if (table_name.empty())
            throw err("table name is empty");
        if (table_scheme.empty())
            throw err("table scheme is empty");

        const std::string sql = "SELECT column_name "
                                "FROM information_schema.columns"
                                " WHERE "
                                "table_schema=" +
                                database::quote(table_scheme) +
                                " AND table_name=" + database::quote(table_name) + ";";

        const auto result = database::selectr(sql, database_connection);

        ARRAY_T<std::string> array;
        for (const auto &row : result)
            for (const auto &field : row)
            {
                const std::string column_name = field.is_null() ? "" : field.as<std::string>();
                array.push_back(column_name);
            }

        return array;
    }
    catch (const std::exception &e)
    {
        throw err(e.what());
    }
}

template <template <typename, typename> typename MAP_T>
MAP_T<std::string, std::string> database_online::get_map_table_str(
    const std::string &table_name, const std::string &table_scheme, const std::string& database_connection)
{
    try
    {
        if (table_name.empty())
            throw err("table name is empty");
        if (table_scheme.empty())
            throw err("table scheme is empty");

        const std::string sql = "SELECT column_name "
                                "FROM information_schema.columns"
                                " WHERE "
                                "table_schema=" +
                                database::quote(table_scheme) +
                                " AND table_name=" + database::quote(table_name) + ";";

        const auto result = database::selectr(sql, database_connection);

        MAP_T<std::string, std::string> map;
        for (const auto &row : result)
            for (const auto &field : row)
            {
                const std::string column_name = field.is_null() ? "" : field.as<std::string>();
                map.emplace(column_name, "");
            }

        return map;
    }
    catch (const std::exception &e)
    {
        throw err(e.what());
    }
}

template <template <typename, typename> typename MAP_T, template <typename> typename ARRAY_T1, template <typename> typename ARRAY_T2>
bool database_online::inner_join_is_this_tree_lighter_than_best_tree(
    const MAP_T<std::string, database_online::Inner_Join_Graph_Search_Node> &graph_search,
    const ARRAY_T1<std::string> &spanning_tree,
    const ARRAY_T2<std::string> &path,
    const int best_spanning_tree_weight)
{
    try
    {
        std::unordered_set<std::string> stree;

        ////////////////////////////////////////////////////////////////////////////////
        // constroi a árvore geradora mínima gerada até o momento com a inserção do caminho.
        // os dois for abaixo são para conseguir apenas os nós não repetidos tanto da
        // árvore gerada até o momento, quanto do caminho que será a ela adicionado.
        ////////////////////////////////////////////////////////////////////////////////
        for (const auto &node : spanning_tree)
            stree.emplace(node);

        for (const auto &node : path)
            stree.emplace(node);

        int total_weight_tree = 0;
        for (const auto &node : stree) // encontra o peso da árvore gerada até o momento
            total_weight_tree += graph_search.at(node).get_weight();

        if (total_weight_tree < best_spanning_tree_weight)
            return true;
        return false;
    }
    catch (const std::exception &e)
    {
        throw err(e.what());
    }
}

////////////////////////////////////////////////////////////////////////////////
// templates class database_online::Inner_Join_Graph_Search_Node
////////////////////////////////////////////////////////////////////////////////
template <template <typename> typename ARRAY_T>
database_online::Inner_Join_Graph_Search_Node::Inner_Join_Graph_Search_Node(
    const std::string &table_name,
    const ARRAY_T<database_online::Inner_Join_Table_Virtual_Node> &virtual_graph)
{
    try
    {
        this->table_name = table_name;
        is_target_table = false;
        vtable_id = -1;
        weight = 1;
        for (const auto &vtable : virtual_graph)
        {
            if (!is_target_table && vtable.has(table_name))
            {
                is_target_table = true;
                vtable_id = vtable.get_id();
                weight = 0;
            }
            Inner_Join_Graph_Search_Node_Info i;
            vtable_target.emplace(vtable.get_id(), i);
        }
    }
    catch (const std::exception &e)
    {
        throw err(e.what());
    }
}

template <template <typename> typename ARRAY_T>
void database_online::Inner_Join_Graph_Search_Node::print(
    const ARRAY_T<database_online::Inner_Join_Table_Virtual_Node> &virtual_graph_target) const
{
    try
    {
        std::cout << "Inner_Join_Graph_Search_Node:: (" << this->table_name << ")\n";
        std::cout << "\tis target table: " << u::to_str(is_target_table) << "\n";
        std::cout << "\tvtable_id: " << vtable_id << "\n";
        std::cout << "\tweight: " << weight << "\n";
        std::cout << "\tvirtual tables target:\n";
        for(const auto& vtable : virtual_graph_target)
        {
            const auto& vtablet = vtable_target.at(vtable.get_id());
            std::cout << "\t\tvtable_id: " << vtable.get_id() << " - tables in virtual node: " << vtable.get_tables() << " {\n";
            vtablet.print();
            std::cout << "\n\t\t}\n";
        }
        std::cout << "\n";
    }
    catch (const std::exception &e)
    {
        throw err(e.what());
    }
}

#endif // DATABASE_ONLINE_HPP
/*
// void database_online::walk()
{ try {
 } catch (const std::exception &e) { throw err(e.what()); }
}
*/
/*
 SQLs 

// retorna quais são as colunas que são chaves estrangeiras de uma tabela, e qual é a tabela e a coluna que ela referencia.

 SELECT
    tc.table_schema, 
    tc.constraint_name, 
    tc.table_name, 
    kcu.column_name, 
    ccu.table_schema AS foreign_table_schema,
    ccu.table_name AS foreign_table_name,
    ccu.column_name AS foreign_column_name 
FROM 
    information_schema.table_constraints AS tc 
    JOIN information_schema.key_column_usage AS kcu
      ON tc.constraint_name = kcu.constraint_name
      AND tc.table_schema = kcu.table_schema
    JOIN information_schema.constraint_column_usage AS ccu
      ON ccu.constraint_name = tc.constraint_name
      AND ccu.table_schema = tc.table_schema
WHERE tc.constraint_type = 'FOREIGN KEY' AND tc.table_name='mytable';


//  how to retrieve information of columns from table in select statement

select * from INFORMATION_SCHEMA.COLUMNS where table_name = 'users';

*/
