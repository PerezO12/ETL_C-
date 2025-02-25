#ifndef MANAGER_DB_HPP
#define MANAGER_DB_HPP

#include <string>
#include <map>
#include <vector>
#include <libpq-fe.h>
#include <nlohmann/json.hpp>

using namespace std;
using json = nlohmann::json;
class manager_db 
{
    public:
        manager_db(const string& host, const string& dbname, const string& user, const string& password, const string& port);

        //Insert
        void execute_query(const string& query);
        //select
        PGresult* execute_select_query(const string& query); //cambiar
        //crear tablas
        void createTables(const json& config);
        void createRelationships(const json& config);
        //destructor de la conexión a la db
        ~manager_db();
        
    private:
        PGconn* conn;

};

#endif //MANAGER_DB_HPP