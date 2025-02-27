#include "../include/transformation.hpp"

string join(const vector<string>& elements, const string& separator) {
    ostringstream result;
    for (size_t i = 0; i < elements.size(); ++i) {
        result << elements[i];
        if (i < elements.size() - 1) result << separator;
    }
    return result.str();
}

//applica las transformaciones defidindas en conf
string applyTransformation(const string& value, const string& transformName, const json& transformationsConfig) {
    if (!transformationsConfig.contains(transformName)) {
        throw runtime_error("Transformación no definida: " + transformName);
    }

    const json& transform = transformationsConfig[transformName];
    string logic = transform["logic"];

    if (logic == "split_by_space") {
        size_t spacePos = value.find(' ');
        if (spacePos == string::npos) {
            throw runtime_error("No se puede dividir el nombre completo: " + value);
        }
        return value.substr(0, spacePos) + "_" + value.substr(spacePos + 1);
    } else if (logic == "trim_uppercase") {
        string trimmed = value;
        trimmed.erase(trimmed.begin(), find_if(trimmed.begin(), trimmed.end(), [](int ch) { return !isspace(ch); }));
        trimmed.erase(find_if(trimmed.rbegin(), trimmed.rend(), [](int ch) { return !isspace(ch); }).base(), trimmed.end());
        std::transform(trimmed.begin(), trimmed.end(), trimmed.begin(), ::toupper);
        return trimmed;
    }
    return value;
}



vector<string> jsonToSqlInsert(const string& tableName, const vector<json>& recordsTable, const json& data, const json& config, map<string, map<string, int>>& idCache) {
    vector<string> queries;
    const json& transformations = config["transformations"];
    const json& tableConfig = config["tables"][tableName];
    for (const auto& recordTable : recordsTable) {
        map<string, string> processedValues;
        vector<string> columns;
        vector<string> values;

        for (const auto& col : tableConfig["columns"]) {
            if (col.contains("lookup")) continue;
            string colName = col["name"];
            string jsonPath = col["jsonPath"];
            string value = getValueFromJson(recordTable, jsonPath);

            if (col.contains("transform")) {
                value = applyTransformation(value, col["transform"], transformations);
            }

            processedValues[colName] = value;
            columns.push_back("\"" + colName + "\"");
            values.push_back("'" + value + "'");
        }
        //lookups: su fnc es reemplazar lso valores naturales creados anteriormente con los id d la base d datos
        //sin tener q hacer la consulta apra obtenerlos
        //todo: vamos a bsucar estos datos en la tabla correspondiente, est asolucion es infeciente revizar luego
        for (const auto& col : tableConfig["columns"]) {
            if (!col.contains("lookup")) continue;

            const json& lookup = col["lookup"];
            string refTable = lookup["table"];
            //esto es para poder buscar en la tabla anterior la concatenacion de llave unica q sera remplazada x el id 
            const string rootPath = config["dataSource"].value("rootPath", "");
            const json& tableConfigLookup = config["tables"][refTable];
            vector<json> recordsTableLookup = getJsonRecords(data, tableConfigLookup["sourcePath"], rootPath);

            vector<string> refKeyComponents = tableConfigLookup["naturalKey"];
            vector<string> keyParts;
        

            //el error esta en que esta buscnado en la misma tabla, tiene q saltar para la de profesores
            for (const string& field : refKeyComponents) {
                string valField;
                for(auto& ref: recordsTableLookup) {
                    valField = getValueFromJson(ref, field);
                }
                if (valField != "") {
                    keyParts.push_back(valField);
                } else {
                    throw runtime_error("Campo requerido para lookup no encontrado: " + field);
                }
            }

            string naturalKey = join(keyParts, "_");
            if (idCache[refTable].find(naturalKey) == idCache[refTable].end()) {
                throw runtime_error("Clave no encontrada en " + refTable + ": " + naturalKey);
            }

            string lookupColumn = col["name"];
            processedValues[lookupColumn] = to_string(idCache[refTable][naturalKey]);
            columns.push_back("\"" + lookupColumn + "\"");
            values.push_back(processedValues[lookupColumn]);
        }
        // consulta
        ostringstream query;
        query << "INSERT INTO \"" << tableName << "\" ("
              << join(columns, ", ") << ") VALUES (" << join(values, ", ") << ")";
        
        if (tableConfig["generatedId"]) {
            query << " RETURNING id";
        }

        queries.push_back(query.str() + ";");
    }
    return queries;
}

/* map<string, vector<string>> jsonToSqlInsertt2(const json& data, const json& config, map<string, map<string, int>>& idCache) {
    map<string, vector<string>> insertQueries;
    const string rootPath = config["dataSource"].value("rootPath", "");
    const json& transformations = config["transformations"];


    for (const string& tableName : config["globalOptions"]["loadOrder"]) {
        const json& tableConfig = config["tables"][tableName];
        vector<json> records = getJsonRecords(data, tableConfig["sourcePath"], rootPath);
        vector<string> queries;

        for (const auto& record : records) {
            map<string, string> processedValues;
            vector<string> columns;
            vector<string> values;

            for (const auto& col : tableConfig["columns"]) {
                string colName = col["name"];
                string jsonPath = col["jsonPath"];
                string value = getValueFromJson(record, jsonPath);

                if (col.contains("transform")) {
                    value = applyTransformation(value, col["transform"], transformations);
                }

                processedValues[colName] = value;
                columns.push_back("\"" + colName + "\"");
                values.push_back("'" + value + "'");
            }
            //lookups: su fnc es reemplazar lso valores naturales creados anteriormente con los id d la base d datos
            //sin tener q hacer la consulta apra obtenerlos
            for (const auto& col : tableConfig["columns"]) {
                if (!col.contains("lookup")) continue;
                continue;//BORRAR LUEGO
                const json& lookup = col["lookup"];
                string refTable = lookup["table"];
                vector<string> refKeyComponents = lookup["naturalKey"];
                vector<string> keyParts;

                for (const string& field : refKeyComponents) {
                    if (processedValues.find(field) != processedValues.end()) {
                        keyParts.push_back(processedValues[field]);
                    } else {
                        throw runtime_error("Campo requerido para lookup no encontrado: " + field);
                    }
                }

                string naturalKey = join(keyParts, "_");
                if (idCache[refTable].find(naturalKey) == idCache[refTable].end()) {
                    throw runtime_error("Clave no encontrada en " + refTable + ": " + naturalKey);
                }


                string lookupColumn = col["name"];
                processedValues[lookupColumn] = to_string(idCache[refTable][naturalKey]);
                columns.push_back("\"" + lookupColumn + "\"");
                values.push_back(processedValues[lookupColumn]);
            }

            // consulta
            ostringstream query;
            query << "INSERT INTO \"" << tableName << "\" ("
                  << join(columns, ", ") << ") VALUES (" << join(values, ", ") << ")";
            
            if (tableConfig["generatedId"]) {
                query << " RETURNING id";
            }

            queries.push_back(query.str() + ";");
        }

        insertQueries[tableName] = queries;
    }

    return insertQueries;
}
 */
