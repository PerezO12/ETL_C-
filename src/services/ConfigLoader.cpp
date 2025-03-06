#include "../../include/services/ConfigLoader.hpp"
#include <fstream>

json leerConfiguracion(const string& configFilePath) {
    ifstream configFile(configFilePath);
    if (!configFile) {
        throw runtime_error("No se pudo abrir el archivo de configuración");
    }
    json config;
    configFile >> config;
    return config;
}