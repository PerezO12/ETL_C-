#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include <libpq-fe.h>
#include "../common/common_types.hpp"

class Connection {
public:
    Connection(const string& host, const string& dbname, const string& user, const string& password, const string& port);
    Connection(const json& dbConfig);
    Connection(const string& conninfo); //todo: falta implementar
    ~Connection();
    PGconn* getRawConnection() const;

private:
    PGconn* conn;
};

#endif