cmake_minimum_required(VERSION 3.10)

project(WebServer)

# Указываем путь к системе сборки vcpkg
set(CMAKE_TOOLCHAIN_FILE "/workspaces/web-server/main-code/1/include/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")

# Указываем, что проект использует C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Указываем путь к nlohmann_json
set(nlohmann_json_DIR "/workspaces/web-server/main-code/1/include/vcpkg/installed/x64-linux/share/nlohmann_json")

# Поиск nlohmann-json
find_package(nlohmann_json CONFIG REQUIRED)

# Поиск OpenSSL
find_package(OpenSSL REQUIRED)

# Поиск jwt-cpp
find_package(jwt-cpp CONFIG REQUIRED)

# Поиск PostgreSQL
find_package(PostgreSQL REQUIRED)

# Добавляем cpp-httplib (если используется как заголовочная библиотека)
include_directories(${CMAKE_SOURCE_DIR}/cpp-httplib)

# Указываем директории для заголовочных файлов
include_directories(${PostgreSQL_INCLUDE_DIRS})

# Создаем исполняемый файл
add_executable(web-server 
    reg/main.cpp 
    reg/reg.cpp 
    discipline/discipline.cpp
    test/test.cpp
    test/try.cpp
)

# Линкуем библиотеки
target_link_libraries(web-server 
    PRIVATE 
    OpenSSL::SSL 
    OpenSSL::Crypto 
    jwt-cpp::jwt-cpp 
    nlohmann_json::nlohmann_json 
    ${PostgreSQL_LIBRARIES}  # Добавляем PostgreSQL
)

# Указываем директории для заголовочных файлов
include_directories(web-server PRIVATE
    ${CMAKE_SOURCE_DIR}/cpp-httplib
    ${CMAKE_SOURCE_DIR}/include  # Добавляем путь к include
    ${CMAKE_SOURCE_DIR}/reg
    ${CMAKE_SOURCE_DIR}/discipline
    ${CMAKE_SOURCE_DIR}/test
)