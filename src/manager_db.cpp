#include "../include/manager_db.hpp"

#include <iostream>
#include <unordered_set>
#include <stdexcept>

manager_db::manager_db(const string& host, const string& dbname, const string& user, const string& password, const string& port)
{
    string conninfo = "host=" + host + " dbname=" + dbname + " user=" + user + " password=" + password + " port=" + port;

    conn = PQconnectdb(conninfo.c_str());

    if (PQstatus(conn) != CONNECTION_OK) {
        cerr << "Conexión a la base de datos fallida: " << PQerrorMessage(conn) << endl;
        throw runtime_error("Error en la conexión a la base de datos");
    } else {
        cout << "Conexión exitosa a la base de datos " << dbname << endl;
    }
}

//insert
void manager_db::execute_query(const string& query) {
    PGresult* res = PQexec(conn, query.c_str());

    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        cerr << "Error en la consulta: " << PQerrorMessage(conn) << endl;
    } else {
        cout << "Consulta ejecutada con éxito" << endl;
    }

    PQclear(res);
}
//crear tablas
//todo:cambiar luego
void manager_db::create_tables(const string& query) {
    PGresult* res = PQexec(conn, query.c_str());

    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        cerr << "Error en la consulta: " << PQerrorMessage(conn) << endl;
    } else {
        cout << "Consulta ejecutada con éxito" << endl;
    }

    PQclear(res);
}


//get
PGresult* manager_db::execute_select_query(const string& query) {
    PGresult* res = PQexec(conn, query.c_str());

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
     cerr << "Error en la consulta SELECT: " << PQerrorMessage(conn) << endl;
        PQclear(res);
        return nullptr;
    }

    return res;
}




//destructor
manager_db::~manager_db() {
    if (conn) {
        PQfinish(conn);
     cout << "Conexión cerrada" << endl;
    }
}