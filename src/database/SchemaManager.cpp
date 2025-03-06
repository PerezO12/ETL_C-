#include "../../include/database/SchemaManager.hpp"

SchemaManager::SchemaManager(QueryExecutor& executor) 
    : executor(executor) 
{

}

void SchemaManager::createTable(const string tableName, const json& tableData) {
    try {
        vector<string> elements;
            //si en la conf d la tabla se pide que se autogenere la primary key
            bool generatedPK = tableData["generatedPK"].get<bool>();
            if (generatedPK) {
                elements.push_back("id SERIAL PRIMARY KEY");
            }

            for (const auto& column : tableData["columns"]) {
                string colDef = column["name"].get<string>() + " " + column["type"].get<string>();

                //info: add al readme q el json en al tabla puede tener campos de unique
                if (column.contains("unique") && column["unique"].get<bool>()) {
                    colDef += " UNIQUE";
                }

                // FK
                if (column.contains("lookup")) {
                    const string refTable = column["lookup"]["table"].get<string>();
                    colDef += " REFERENCES " + refTable + "(id)";
                }

                elements.push_back(colDef);
            }

            string query = "CREATE TABLE IF NOT EXISTS " + tableName + " (\n    " + Utility::join(elements, ",\n    ") + "\n);";
            
            executor.executeQuery(query);
            cout << "[OK] Tabla creada: " << tableName << endl;
    } catch (const exception& e) {
         throw runtime_error("Error creando tablas: " + string(e.what()));
    }
}

void SchemaManager::createTables(const json& tablesConfig, const vector<string> loadOrder) {
    try {
        for (const auto& table : loadOrder) {
            createTable(table, tablesConfig[table]);
        }
    } catch (const exception& e) {
        throw runtime_error("Error creando tablas: " + string(e.what()));
    }
}

void SchemaManager::createRelationships(const json& relationshipsConfig) {
    try {
        for (const auto& rel : relationshipsConfig) {
            if (rel["type"] == "MANY_TO_MANY") {
                vector<string> columns;
                
                for (const auto& col : rel["columns"]) {
                    string colDef = col["name"].get<string>() + " " + col["type"].get<string>();
                    
                    if (col.contains("source")) {
                        const string refTable = col["source"]["table"].get<string>();
                        colDef += " REFERENCES " + refTable + "(id)";
                    }
                    
                    columns.push_back(colDef);
                }
                
                string query = "CREATE TABLE IF NOT EXISTS " 
                                  + rel["junctionTable"].get<string>() + " (\n    " 
                                  + Utility::join(columns, ",\n    ") + "\n);";
                
                executor.executeQuery(query);
                cout << "[OK] Tabla de relación creada: " << rel["junctionTable"] << endl;
            }
        }
    } catch (const exception& e) {
        throw runtime_error("Error creando relaciones: " + string(e.what()));
    }
}

void SchemaManager::truncateTables(const vector<string>& tables) {
    try {
        if (tables.empty()) {
            throw runtime_error("Lista de tablas vacía. No hay nada que truncar.");
        }

        string query = "TRUNCATE ";
        for (size_t i = 0; i < tables.size(); ++i) {
            query += tables[i];
            if (i != tables.size() - 1) {
                query += ", ";
            }
        }
        query += " RESTART IDENTITY CASCADE;";

        executor.executeQuery(query);

        cout << "Tablas truncadas exitosamente: ";
        for (const auto& table : tables) {
            cout << table << " ";
        }
        cout << endl;

    } catch (const exception& e) {
        throw runtime_error("Error al truncar tablas: " + string(e.what()));
    }
}
