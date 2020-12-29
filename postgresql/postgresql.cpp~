
#include "../database.hpp"


////////////////////////////////////////////////////////////////////////////////
// namespace
////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////
// namespace
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// inner global variables
////////////////////////////////////////////////////////////////////////////////
static std::vector<std::unique_ptr<std::string>> _conf {};


////////////////////////////////////////////////////////////////////////////////
// private functions
////////////////////////////////////////////////////////////////////////////////
/**
 * retorna a string de configuração para abrir a conexão.
 * tal str está guardada no vetor de configuração, sendo a primeira posição dele.
 */
const std::string& get_config_connect()
{try {
	if(_conf.empty()) throw err("");
	if(_conf.front()->empty()) throw err("");
	
	auto const& str = *_conf.front().get();
	return str;
 } catch(std::exception const& e) { throw err(e.what()); }
}

/**
 * descobre e determina se a string é:
 * 1. comando sql que retorna
 * 2. comando sql sem retorno
 * 3. comando interno da função
 */
char resolve(const std::string& str)
{try {
	// tipo da string que executou antes:
	// 0: str anterior não era comando
	// 1: str anterior era comando e eu devo retornar e não ser comando
	//		o comando nessecaso é o "return"
	static int str_back {0};
	bool command = false;
	
	// bloco para verificar os comandos
	if(str == "return") command = true;
	
	// bloco para verificar a relação do comando com o antescessor
	// neste caso não pode haver comandos seguidos
	if(str_back != 0 && command) throw err("invalid code: \"%s\"", str);
	
	if(command)
	{
		str_back = 1;
		return 'c';
	}
	
	if(str_back == 1) {
		str_back = 0;
		return 'r';
	}
	
	return 'n'; // não é comando e é uma query que não retorna 
 } catch(std::exception const& e) { throw err(e.what()); }
}


////////////////////////////////////////////////////////////////////////////////
// public functions
////////////////////////////////////////////////////////////////////////////////
void db::config(
		const std::vector<std::string>&& conf)
{ try {
	if(conf.empty()) throw err("conf is empty");
	
	// verifica se somente tem strings vazias;
	for(auto const& e : conf)
		if(e.empty() == false) goto STR_NOT_EMPTY;
	
	throw err("conf contains only empty string. - conf size: %lu", conf.size());
	STR_NOT_EMPTY:
	
	_conf.clear();
	
	for(auto const& e : conf)
		_conf.push_back(std::make_unique<std::string>(e));

 } catch(std::exception const& e) { throw err(e.what()); }
}



std::unique_ptr<std::vector<std::unique_ptr<pqxx::result>>>
db::nontransaction(const std::vector<std::string> v)
{ try {
	pqxx::connection C { get_config_connect() };
	if (C.is_open() == false)
		 throw err("Can't open database: \"%s\"\n", C.dbname());
	
	auto result = std::make_unique<std::vector<std::unique_ptr<pqxx::result>>>();
	pqxx::nontransaction W {C};
	
	for(auto const& e : v)
	{
		switch(resolve(e))
		{
			case 'r':result->push_back(
					std::make_unique<pqxx::result>(W.exec(e))); break;
					
			case 'n': W.exec0(e);
			case 'c': break; // command - nothing just to verify
			default : throw err("invalid answer for checking input.");
		}
	}
	
	W.commit();
	C.disconnect();
	return result;
 } catch(pqxx::sql_error const &e) { 
 		throw err("SQL error: %s\nQuery was: \"%s\"", e.what(), e.query());
 } catch(std::exception const& e) { throw err(e.what()); }
}


std::unique_ptr<std::vector<std::unique_ptr<pqxx::result>>>
db::transaction(const std::vector<std::string> v)
{ try {
	pqxx::connection C { get_config_connect() };
	if (C.is_open() == false)
		 throw err("Can't open database: \"%s\"\n", C.dbname());
	
	auto result = std::make_unique<std::vector<std::unique_ptr<pqxx::result>>>();
	pqxx::work W {C};
	
	for(auto const& e : v)
	{
		switch(resolve(e))
		{
			case 'r':result->push_back(
					std::make_unique<pqxx::result>(W.exec(e))); break;
					
			case 'n': W.exec0(e);
			case 'c': break; // command - nothing just to verify
			default : throw err("invalid answer for checking input.");
		}
	}
	
	W.commit();
	C.disconnect();
	return result;
 } catch(pqxx::sql_error const &e) { 
 		throw err("SQL error: %s\nQuery was: \"%s\"", e.what(), e.query());
 } catch(std::exception const& e) { throw err(e.what()); }
}

pqxx::connection c;
pqxx::nontransaction b(c);

std::string
db::quote_esc(const char *str)
{try{
	if(str == nullptr) return std::string();
	
	return b.quote(b.esc(str));
	
 } catch(std::exception const& e) { throw err(e.what()); }
}

std::string
db::quote_esc(const std::string&& str)
{try{
	return b.quote(b.esc(str));
 } catch(std::exception const& e) { throw err(e.what()); }
}









/**


template<class T>
auto get(std::vector<T> v ) {
    std::cout << __func__ << " : " << v[0] << " : " << typeid(v[0]).name() << '\n';
    return v[0];
}

void boo() {
    for(int i = -1000; i < 1; ++i) {
        auto x = std::make_unique<int>();
        *x = i;
        fprintf(stderr, "%s : %d\n", __func__, *x.get());
    }
}

std::unique_ptr<std::vector<std::unique_ptr<std::string>>>
foo(std::vector<string> va) {
    auto v = std::make_unique<std::vector<std::unique_ptr<std::string>>>();
    for(auto const& e : va) {
        v->push_back(std::make_unique<std::string>(e + " hum..."));
        auto const&& i = &e - &va[0];
        printf("e : \"%s\" | v : \"%s\"\n", e.c_str(), v->at(i)->c_str());
    }
    
    return std::move(v);
}


int main() {
    auto const x = foo({"I", "love"});
    boo();
   printf("\nxxxxxx\n");
    for(auto const&e:*x)
        printf("%ld : \"%s\"\n", &e - &(*x)[0], e->c_str());
}




std::unique_ptr<std::vector<std::unique_ptr<std::string>>> foo() {
    auto x = std::make_unique<std::vector<std::unique_ptr<std::string>>>();
   printf("size: %lu\n",x->size());
    x->push_back(std::make_unique<std::string>("I"));
    x->push_back(std::make_unique<std::string>("love"));
    x->push_back(std::make_unique<std::string>("my"));
    x->push_back(std::make_unique<std::string>("mother"));
    //*u = "ow my God!";
    //a.push_back(std::make_unique<std::string>("love"));
    //a.push_back(std::make_unique<std::string>("my"));
    //a.push_back(std::make_unique<std::string>("family2"));
    
    printf("\nsize: %lu\n", x->size());
    for(auto const& e : *x) {
        printf("%d : %s, ", &e - &(*x)[0], e->c_str());
    }
    printf("\nsize: %lu\n", x->size());
    for(auto const& ee : *x) {
        auto const& e = *ee.get();
        printf("%s ", e.c_str());
    }
    return std::move(x);
}

int main() {
  
    //foo();
    //auto v = foo();
    //std::unique_ptr<std::vector<int>> v (foo());
    auto const&& v = foo();
    printf("\n====\n");
  // int c = 9;
  // printf("%d | %ld\n", c, &c);
  //  auto const && p = c;
  //  printf("%d | %ld\n", p, &p);
   // p = 3;
    //printf("%d | %ld | %d\n", p, &p, c);
    
    printf("size: %lu\n",v->size());
    for(auto const& e : *v) {
        printf("%d : %s\n", &e - &(*v)[0], e->c_str());
    }
}

// check typeid

    std::vector<char*> vc {"x"};
    std::vector<const char*> vk {"x"};
    std::vector<std::string> vs {"x"};
    
    auto i = get<int>({1, 2});
    auto c = get<const char*>({"t"});
    auto a = get<int>({1, 2});
    std::cout << "i : " << typeid(i).name() << '\n';
    std::cout << "c : " << typeid(c).name() << '\n';
    std::cout << "a : " << typeid(a).name() << '\n';
    std::cout << "{1} : " << typeid(char {1}).name() << '\n';
    std::cout << "get : " << typeid(get<int>({1, 2})).name() << '\n';
    
    printf("%s\n%s\n%s\nfas", vc[0], vk[0], vs[0].c_str());
  //bar(foo());

get : St6vectorIiSaIiEE                                                                                             
std::cout << "std::vector<const char*>* : " << typeid(std::vector<const char*>*).name() << '\n';
    std::cout << "std::vector<const char*>  : " << typeid(std::vector<const char*>).name() << '\n';
    std::cout << "std::vector<const char*>& : " << typeid(std::vector<const char*>&).name() << '\n';
    std::cout << "const char* : " << typeid(const char*).name() << '\n';
    std::cout << "char* : " << typeid( char*).name() << '\n';
    
    const auto i = f2();
    const char* c = f2();
    std::cout << "const char* : " << typeid(const char*).name() << '\n';
    std::cout << "i : " << typeid(i).name() << '\n';
    std::cout << "c : " << typeid(c).name() << '\n';
    std::cout << "f2() : " << typeid(f2()).name() << '\n';
*/




