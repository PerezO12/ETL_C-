cmake_minimum_required(VERSION 3.15)
project(ETL_Project)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# Asegúrate de que esta ruta es la correcta
set(CMAKE_TOOLCHAIN_FILE "C:/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")


find_package(nlohmann_json CONFIG REQUIRED)
find_package(PostgreSQL REQUIRED)
#find_package(libpqxx CONFIG REQUIRED)

# Crear el ejecutable con los archivos fuente
add_executable(etl_processor 
    src/main.cpp 
    src/extraction.cpp 
    src/transformation.cpp
    src/manager_db.cpp
    src/utility.cpp
    )

# Vincular la librería nlohmann_json y PostgreSQL al ejecutable
target_link_libraries(etl_processor PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(etl_processor PRIVATE PostgreSQL::PostgreSQL)

