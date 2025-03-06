#ifndef SCHEMA_MANAGER_HPP
#define SCHEMA_MANAGER_HPP

#include "../common/common_types.hpp"
#include "QueryExecutor.hpp"
#include "../common/Utility.hpp"

class SchemaManager {
public:
    SchemaManager(QueryExecutor& executor);
    void createTable(const string tableName, const json& tableData);
    void createTables(const json& tablesConfig, const vector<string> loadOrder);
    void createRelationships(const json& relationshipsConfig);
    void truncateTables(const vector<string>& tables);

private:
    QueryExecutor& executor;
};

#endif