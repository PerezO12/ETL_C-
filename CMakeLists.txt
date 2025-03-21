cmake_minimum_required(VERSION 3.15)
project(ETL_Project)
project(
    ETL_Project
    VERSION 1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_TOOLCHAIN_FILE "C:/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")

# Especificar manualmente la ubicación de simdjson
set(simdjson_DIR "C:/simdjson-1.1.0/lib/cmake/simdjson")

find_package(nlohmann_json CONFIG REQUIRED)
find_package(PostgreSQL REQUIRED)
find_package(simdjson REQUIRED NO_CMAKE_PACKAGE_REGISTRY)  # Ignorar registros globales


add_executable(etl_processor 
    src/main.cpp 
    src/common/Utility.cpp 
    src/database/Connection.cpp 
    src/database/InsertValues.cpp 
    src/database/QueryExecutor.cpp 
    src/database/SchemaManager.cpp 
    src/etl/Extraction.cpp 
    src/etl/Transformation.cpp 
    src/services/ConfigLoader.cpp 
    )

# Vincular la librería nlohmann_json y PostgreSQL al ejecutable
target_link_libraries(etl_processor PRIVATE
    nlohmann_json::nlohmann_json
    PostgreSQL::PostgreSQL
    simdjson::simdjson
)



# Añadir rutas de inclusión
target_include_directories(etl_processor PRIVATE
    "C:/simdjson-1.1.0/include"
)

# Añadir ruta de la biblioteca (solo para MinGW)
target_link_directories(etl_processor PRIVATE
    "C:/simdjson-1.1.0/lib"
)


