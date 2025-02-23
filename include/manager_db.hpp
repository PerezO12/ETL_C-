#ifndef MANAGER_DB_HPP
#define MANAGER_DB_HPP

#include <string>
#include <libpq-fe.h>

using namespace std;

class manager_db 
{
    public:
        manager_db(const string& host, const string& dbname, const string& user, const string& password, const string& port);
        //todo: hare 2 metodos de momento
        //Insert
        void execute_query(const string& query);
        //select
        PGresult* execute_select_query(const string& query);
        //crear tablas
        void create_tables(const string& query); //luego hacer apra crear dinamicas
        //void create_tables()

        //destructor de la conexión a la db
        ~manager_db();
        
    private:
        PGconn* conn;

};

#endif //MANAGER_DB_HPP