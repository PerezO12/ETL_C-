#include "../../include/database/Connection.hpp"

Connection::Connection(const string& host, const string& dbname, const string& user, const string& password, const string& port) {
    string conninfo = "host=" + host + " dbname=" + dbname + " user=" + user + " password=" + password + " port=" + port;

    conn = PQconnectdb(conninfo.c_str());

    if (PQstatus(conn) != CONNECTION_OK) {
        cerr << "Conexión a la base de datos fallida: " << PQerrorMessage(conn) << endl;
        throw runtime_error("Error en la conexión a la base de datos");
    } else {
        
        cout << "Conexión exitosa a la base de datos " << dbname << endl;
    }
}

Connection::Connection(const json& dbConfig) {
    try {
        const string host = dbConfig.at("host").get<string>();
        const string dbname = dbConfig.at("dbname").get<string>();
        const string user = dbConfig.at("user").get<string>();
        const string password = dbConfig.at("password").get<string>();
        const string port = dbConfig.at("port").get<string>();

        string conninfo = 
            "host=" + host + " " +
            "dbname=" + dbname + " " +
            "user=" + user + " " +
            "password=" + password + " " +
            "port=" + port;

        conn = PQconnectdb(conninfo.c_str());

        if (PQstatus(conn) != CONNECTION_OK) {
            string errorMsg = "Conexión a la base de datos fallida: " + string(PQerrorMessage(conn));
            PQfinish(conn);
            throw runtime_error(errorMsg);
        } else {
            cout << "Conexión exitosa a la base de datos " << dbname << endl;
        }
    } catch (const json::exception& e) {
        throw runtime_error("Error en la configuración de la base de datos: " + string(e.what()));
    } catch (const exception& e) {
        throw runtime_error("Error: " + string(e.what()));
    }
}

Connection::~Connection() {
    if (conn) {
        PQfinish(conn);
        cout << "Conexión cerrada" << endl;
    }
}
PGconn* Connection::getRawConnection() const {
    return conn;
}