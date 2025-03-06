#ifndef QUERY_EXECUTOR_HPP
#define QUERY_EXECUTOR_HPP

#include <libpq-fe.h>
#include "../common/common_types.hpp"

class QueryExecutor {
public:
    explicit QueryExecutor(PGconn* connection);
    void executeQuery(const string& query);
    string executeQueryReturningId(const string& query);
    PGresult* executeSelectQuery(const string& query);//cambiar

private:
    PGconn* conn;
};

#endif