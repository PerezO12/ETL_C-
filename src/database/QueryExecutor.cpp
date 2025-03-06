#include "../../include/database/QueryExecutor.hpp"

QueryExecutor::QueryExecutor(PGconn* connection) : conn(connection) {
    if (!conn) {
        throw std::invalid_argument("La coneccion a la base de datos es null.");
    }
}
//insert
void QueryExecutor::executeQuery(const string& query) {
    PGresult* res = PQexec(conn, query.c_str());

    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        cerr << "Error en la consulta: " << PQerrorMessage(conn) << endl;
    } else {
        cout << "Consulta ejecutada con éxito: " << endl;
        cout << "Quer: " << query << endl;
    }

    PQclear(res);
}

string QueryExecutor::executeQueryReturningId(const string& query) {
    PGresult* res = PQexec(conn, query.c_str());
    
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        string errorMsg = "Error en la consulta (RETURNING id): " + string(PQerrorMessage(conn));
        PQclear(res);
        throw runtime_error(errorMsg);
    }

    if (PQntuples(res) == 0 || PQnfields(res) == 0) {
        PQclear(res);
        throw runtime_error("La consulta " + query + " no retornó ningún ID.");
    }

    char* rawId = PQgetvalue(res, 0, 0);
    if (!rawId) {
        PQclear(res);
        throw runtime_error("El ID retornado es NULL.");
    }

    string id(rawId);
    PQclear(res);

    return id;
}

//get
PGresult* QueryExecutor::executeSelectQuery(const string& query) {
    PGresult* res = PQexec(conn, query.c_str());

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
     cerr << "Error en la consulta SELECT: " << PQerrorMessage(conn) << endl;
        PQclear(res);
        return nullptr;
    }

    return res;
}