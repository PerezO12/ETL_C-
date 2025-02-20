#ifndef REPOSITORY_HPP
#define REPOSITORY_HPP

#include <pqxx/pqxx>
#include <string>
#include <stdexcept>

class Repository {
public:
    Repository(const std::string& host, const std::string& dbname,
               const std::string& user, const std::string& password, int port);
    pqxx::connection& getConnection() { return conn; }

private:
    pqxx::connection conn; 
};

#endif
