#include "../include/repository.hpp"

Repository::Repository(const std::string& host, const std::string& dbname,
                       const std::string& user, const std::string& password, int port)
    : conn("host=" + host + " dbname=" + dbname + " user=" + user + " password=" + password + " port=" + std::to_string(port)) {}

