#include "database.hpp"

////////////////////////////////////////////////////////////////////////////////
// public global variables
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// global functions - namespace: database_obj_str
////////////////////////////////////////////////////////////////////////////////
bool
database_obj_str::is_quote(const std::string& type)
{ try {
	if(type.empty()) throw err("type cannot be an empty string.");
	////////////////////////////////////////////////////////////////////////////////
	// return true
	////////////////////////////////////////////////////////////////////////////////
	if(type == "character varying") return true;
	if(type == "timestamp with time zone") return true;
	if(type == "date") return true;

	////////////////////////////////////////////////////////////////////////////////
	// return false
	////////////////////////////////////////////////////////////////////////////////
	if(type == "integer") return false;
	if(type == "numeric") return false;

	////////////////////////////////////////////////////////////////////////////////
	// error
	////////////////////////////////////////////////////////////////////////////////
	throw err("type is undefined. type: \"%s\"", type.c_str());
 } catch (const std::exception &e) { throw err(e.what()); }
}

std::string 
database_obj_str::to_str(const Constraint constraint)
{ try {
	if(constraint == Constraint::none) return "NONE - NO CONSTRAINT";
	if(constraint == Constraint::primary_key) return "PRIMARY KEY";
	if(constraint == Constraint::unique) return "UNIQUE";
	throw err("database_obj_str::Constraint not defined. Constraint value: %d", static_cast<int>(constraint));
	return "";
 } catch (const std::exception &e) { throw err(e.what()); }
}

database_obj_str::Constraint 
database_obj_str::to_constraint(const std::string& constraint)
{ try {
	if(constraint == "NONE" || constraint == "NO CONSTRAINT") return Constraint::none;
	if(constraint == "PRIMARY KEY") return Constraint::primary_key;
	if(constraint == "UNIQUE") return Constraint::unique;
	throw err("String conversion to database_obj_str::Constraint failure. String constraint: \"%s\"", constraint.c_str());
	return Constraint::none;
 } catch (const std::exception &e) { throw err(e.what()); }
}

std::string 
database_obj_str::map_table_str(const std::string& table_name, const std::string &database_connection)
{ try {
	// a checagem se a table_name é vazio, é feita dentro das funções que geram o sql
    ////////////////////////////////////////////////////////////////////////////////
    // descobre as informações da tabela: nome das colunas, tipos de dados e se pode ser NULL
    ////////////////////////////////////////////////////////////////////////////////
	const auto sql1 = make_sql_information_schema_column(table_name);
    const auto R = database::selectr(sql1, database_connection);

    std::unordered_map<std::string, std::vector<std::string>>  map;
    for(const auto& col : R) { // column_name, data_type, is_nullable
        const auto column_name = !col["column_name"].is_null() ? col["column_name"].as<std::string>() : 
            throw err("Column name is NULL.");
        const auto data_type = !col["data_type"].is_null() ? col["data_type"].as<std::string>() : 
            throw err("Column data type is NULL. - Column name: \"%s\".", column_name.c_str());
        const auto is_nullable = !col["is_nullable"].is_null() ? col["is_nullable"].as<std::string>() : 
            throw err("Column property is_nullable is NULL. - Column name: \"%s\".", column_name.c_str());
        
		////////////////////////////////////////////////////////////////////////////////
		// verifica e insere o valor de not_Null -> o valor é colocado como sendo true or false, o que é uma otimização para a execução
		////////////////////////////////////////////////////////////////////////////////
		std::string not_null;
		if(is_nullable == "YES") not_null = "false";
		else if(is_nullable == "NO") not_null = "true";
		else throw err("is_nullable value is undefined. is_nullbale: \"%s\". Values defined: \"YES\" or \"NO\". Column name: \"%s\"", is_nullable.c_str(), column_name.c_str());

        std::vector<std::string> obj { column_name, data_type, not_null };
        map.emplace(column_name, obj);
    }

	////////////////////////////////////////////////////////////////////////////////
    // recebe as constraints das colunas - UNIQUE and PRIMARY KEY
    ////////////////////////////////////////////////////////////////////////////////
    const auto sql2 = make_sql_information_schema_constraint_type(table_name);
    const auto C = database::selectr(sql2, database_connection);

    for(auto const col : C) {
        const auto column_name = !col["column_name"].is_null() ? col["column_name"].as<std::string>() : 
            throw err("Column name is NULL.");
        const auto constraint_type = !col["constraint_type"].is_null() ? col["constraint_type"].as<std::string>() : 
            throw err("constraint_type is NULL. - Column name: \"%s\".", column_name.c_str());
        
		////////////////////////////////////////////////////////////////////////////////
		// verifica e insere o valor de not_Null -> o valor é colocado como sendo o tipo Constraint::XXX, o que é uma otimização para a execução
		////////////////////////////////////////////////////////////////////////////////
		std::string constraint = "database_obj_str::Constraint::";
		if(constraint_type == "PRIMARY KEY") constraint += "primary_key";
		else if(constraint_type == "UNIQUE") constraint += "unique";
		else throw err("String conversion to database_obj_str::Constraint failure. String constraint: \"%s\" - Column name: \"%s\".", constraint_type.c_str(), column_name.c_str());

        map.at(column_name).push_back(constraint);
    }

	////////////////////////////////////////////////////////////////////////////////
	// tranforma o map em uma string
	////////////////////////////////////////////////////////////////////////////////
	std::string map_str = "{ ";
	for(const auto& [key, field_vector] : map) {
		// get the variables of vector
		const auto& column_name = field_vector[0];
		const auto& data_type = field_vector[1];
		const auto& not_null = field_vector[2];

		if(field_vector.size() == 4) { // caso o field tenha o valor de Constraint::xxx
			const auto& constraint = field_vector[3];

			// verifica consistência dos dados
			if(not_null == "false" && constraint == "database_obj_str::Constraint::primary_key") 
				throw err("Constraint PRIMARY KEY must be an NOT NULL constraint, but NOT NULL is false. Column name: \"%s\".", column_name.c_str());

			map_str += "{\"" + key + "\", "
			"{\"" + table_name + "\", \"" + column_name + "\""
			", \"" + data_type + "\", " +
			not_null + ", " + constraint + "}}, ";
		} else { // // caso o field tenha o valor de Constraint::xxx
			map_str += "{\"" + key + "\", "
			"{\"" + table_name + "\", \"" + column_name + "\""
			", \"" + data_type + "\", " + not_null + "}}, ";
		}
	}
	map_str.pop_back(); map_str.pop_back(); // remove the string ", "
	map_str += " }"; // fecha o map
	return map_str;
 } catch (const std::exception &e) { throw err("%s\nTable name: \"%s\"", e.what(), table_name.c_str()); }
}

////////////////////////////////////////////////////////////////////////////////
// class database_obj_str::Field
////////////////////////////////////////////////////////////////////////////////
database_obj_str::Field::Field(
	const std::string& table_name, const std::string& column_name, const std::string& type,
    const bool not_null, const Constraint constraint)
{ try {
	if(table_name.empty()) throw err("Table name is empty.");
	if(column_name.empty()) throw err("Column name is empty.");
	if(type.empty()) throw err("Type of field is empty.");
	if(!not_null && constraint == Constraint::primary_key) throw err("Constraint PRIMARY KEY must be an NOT NULL constraint, but NOT NULL is false.");

	this->table_name = table_name;
	this->column_name = column_name;
	this->type = type;
	this->constraint = constraint;
	this->not_null = not_null;
	this->quote = is_quote(type);
 } catch (const std::exception &e) { throw err(e.what()); }
}

database_obj_str::Field::Field(
	const std::string& table_name, const std::string& column_name, 
	const std::string& type, const std::string& is_nullable)
{ try {
	if(table_name.empty()) throw err("Table name is empty.");
	if(column_name.empty()) throw err("Column name is empty.");
	if(type.empty()) throw err("Type of field is empty.");
	if(is_nullable != "NO" && is_nullable != "YES") throw err("is_nullable value is undefined. is_nullbale: \"%s\". Values defined: \"YES\" or \"NO\".", is_nullable.c_str());

	this->table_name = table_name;
	this->column_name = column_name;
	this->type = type;
	this->quote = is_quote(type);
	if(is_nullable == "YES") this->not_null = false; // a variável nullable é o oposto do not_null constraint
	else this->not_null = true;
 } catch (const std::exception &e) { throw err(e.what()); }
}

void 
database_obj_str::Field::set_constraint(const std::string& constraint)
{ try {
	if(constraint.empty()) throw err("Column constraint type is empty.");
	this->constraint = to_constraint(constraint);
 } catch (const std::exception &e) { throw err("%s.\nTable name: \"%s\", Column name: \"%s\"", table_name.c_str(), column_name.c_str()); }
}

std::string
database_obj_str::Field::print() const
{ try {
	std::string str = sprint();
	std::cerr << str;
	return str;
 } catch (const std::exception &e) { throw err(e.what()); }
}

std::string
database_obj_str::Field::sprint() const
{ try {
	std::string str = "Printing database_obj_str::Field: " + this->column_name + "\n";
	str += "\tTable name: \'" + this->table_name + "\'\n";
	str += "\tColumn name: \'" + this->column_name + "\'\n";
	str += "\tType: \'" + this->type + "\'\n";
	str += "\tNot null: \'" + u::to_str(this->not_null) + "\'\n";
	str += "\tTo quote: \'" + u::to_str(this->quote) + "\'\n";
	str += "\tConstraint: \'" + to_str(this->constraint) + "\'\n";
	str += "\tData Value: \'" + this->data + "\'\n";

	return str;
 } catch (const std::exception &e) { throw err(e.what()); }
}

void
database_obj_str::Field::set_data(const std::string& data)
{ try {
	if(not_null && data.empty())
		throw err("Data cannot be an empty string. - NOT NULL constraint is true.");
	
	this->data = data;
 } catch (const std::exception &e) { throw err("%s\n%s", e.what(), sprint().c_str()); }
}
////////////////////////////////////////////////////////////////////////////////
// END
////////////////////////////////////////////////////////////////////////////////



/*
// void database_online::walk()
{ try {
 } catch (const std::exception &e) { throw err(e.what()); }
}
*/

////////////////////////////////////////////////////////////////////////////////
// SQL's from namespace database_obj_str
////////////////////////////////////////////////////////////////////////////////
/*

onde: table_name = nome da tabela que se quer buscar as informações

select column_name, data_type, is_nullable  from INFORMATION_SCHEMA.COLUMNS where table_name = 'table_name';

SELECT distinct
    kcu.column_name,
	tc.constraint_type
FROM 
    information_schema.table_constraints AS tc 
    JOIN information_schema.key_column_usage AS kcu
      ON tc.constraint_name = kcu.constraint_name
      AND tc.table_schema = kcu.table_schema

WHERE (tc.constraint_type='UNIQUE' OR tc.constraint_type='PRIMARY KEY') AND tc.table_name='table_name';

*/







