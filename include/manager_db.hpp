#ifndef MANAGER_DB_HPP
#define MANAGER_DB_HPP


#include <libpq-fe.h>
#include <nlohmann/json.hpp>

using namespace std;
using json = nlohmann::json;
class managerDb 
{
    public:
        managerDb(const string& host, const string& dbname, const string& user, const string& password, const string& port);

        //Insert
        void executeQuery(const string& query);
        string executeQueryReturningId(const string& query);
        //select
        PGresult* excecuteSelectQuery(const string& query); //cambiar
        //crear tablas
        void createTables(const json& config);
        void createRelationships(const json& config);
        //destructor de la conexión a la db
        ~managerDb();
        
    private:
        PGconn* conn;

};

#endif //MANAGER_DB_HPP