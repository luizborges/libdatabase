/******************************************************************************

                              Online C++ Compiler.
               Code, Compile, Run and Debug C++ program online.
Write your code in this editor and press "Run" button to compile and execute it.

*******************************************************************************/
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <exception>
#include <map>
#include <vector>
///**
 *Esta classe mapaeia um objeto para uma única tabela.
 * ou seja, deve ser utilizada quando há a correspondência de um único objeto para uma única tabela.
 */
class one {
 protected:
    std::vector<std::string> column; // column name - syntatic sugar for create class
    std::map<std::string,std::string> field; // column is the key
    std::string table = "";
    bool is_field_init = false;
 public:
    virtual std::string& table_name() {
        return table;
    }
    
    virtual std::vector<std::string>& column_name() {
        return column;
    }
    
    // para o usuário inserir quais são as verificações necessárias antes do salvamento - insert e update
    // lembrar que por default a biblioteca insere já a verificação de esc() e quote() - pgxx
    virtual bool check(){ return true; }
    
    
    virtual std::string set(const std::string& key, const std::string& val) {
        if(!is_field_init) init_field();
        
        auto const& e = field.find(key);
        if(e == field.end()) return ""; // não encontrou a chave
        
        std::string old_val = e->second;
        e->second = val;
        return old_val;
    }
    
    virtual std::string& get(const std::string& key) {
        if(!is_field_init) init_field();
        
        auto const& e = field.find(key);
        if(e == field.end()) return ""; // não encontrou a chave
        
        return e->second; // encontrou a chave
    }
    
    virtual void init_field() {
        for(auto const& e:column) {
            field[e] = "";
        }
        is_field_init = true;
    }
    
    virtual std::vector<std::vector<std::string>> collection() {
        std::vector<std::vector<std::string>> r;
        return r;
    }
    
    virtual void select() {}
    virtual void insert() {}
    virtual void update() {}
    virtual void del() {}
    
};

struct users : one {
    users() {
        one::table = "users";
        one::column = {"id", "email", "passwd"};
    }
    
     
   /* virtual bool check() {
        if(table == "users") return false;
    }*/
};


int
main ()
{
    users u;
    one& b = u;
    b.table_name();
    b.column_name();
    std::cout << "check is " << (b.check() ? "true" : "false") << "\n";
    b.set("id", "3");
    b.set("id", std::to_string(7));
    std::cout << "[id]: " << b.get("id") << "\n";
    return 0;
}

///**
 * Essa classe é para se tratar o caso de coleções.
 * ou seja, quando se deseja manipular várias classes ao mesmo tempo.
 * inicialmente foi pensada para manipular variás classes de um mesmo tipo de objeto, mas isso
 * pode mudar no futuro.
 * foi pensada para: 
 * > quando um select retorna várias linhas, utilizar a coleção, para receber
 * um vetor com vários objetos ( cada objeto representa uma linha da resposta do banco de dados)
 * > quando se deseja deletar, inserir ou atualizar, várias classes com um mesmo comando.
 */
template <class T>
struct collection {
    T b;
    std::vector<T> c;
    
    collection(T t) {
        this.t = t;
    }
    
    virtual std::vector<T> init(T& i) {
        b = i;
        const auto v = b.collection();
        c.clean();
        for(auto const& e : v) {
            c.emplace_back(e);
        }
    }
};








