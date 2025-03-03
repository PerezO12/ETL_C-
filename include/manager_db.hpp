#ifndef MANAGER_DB_HPP
#define MANAGER_DB_HPP


#include <libpq-fe.h>
#include <nlohmann/json.hpp>
#include "utility.hpp"
#include "extraction.hpp"

using namespace std;
using json = nlohmann::json;
class managerDb 
{
    public:
        managerDb(const string& host, const string& dbname, const string& user, const string& password, const string& port);
        managerDb(const json& dbConfig);
        //Insert
        void executeQuery(const string& query);
        string executeQueryReturningId(const string& query);
        //select
        PGresult* excecuteSelectQuery(const string& query); //cambiar

        //funciones add
        void createTables(const json& tables, const vector<string> loadOrder);
        void createTable(const string tableName, const json& tableData);
        void createRelationships(const json& relationshipsConfig);
        void truncateTables(const vector<string>& tables);
        void batchInsert(const string& table, const vector<json>& records, const json& tableConfig);
        string lookupId(const string& table, const map<string, string>& naturalKey);
        ~managerDb();
        
    private:
        PGconn* conn;

};

#endif //MANAGER_DB_HPP