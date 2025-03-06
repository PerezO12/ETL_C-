#ifndef INSERT_VALUES_HPP
#define BINSERT_VALUES_HPP

#include "../common/common_types.hpp"
#include "../common/Utility.hpp"
#include "QueryExecutor.hpp"
#include "../etl/extraction.hpp"


class InsertValues {
public:
    InsertValues(QueryExecutor& executor);
    void batchInsert(const std::string& table, const std::vector<json>& records, const json& tableConfig);
    void processRelationships(const json& config, const json& relationshipsConfig, const json& data, const std::string& rootPath);

private:
    QueryExecutor& executor;
    std::string lookupId(const std::string& table, const std::map<std::string, std::string>& naturalKey);
};

#endif