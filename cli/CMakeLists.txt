cmake_minimum_required(VERSION 3.11)
project(cli LANGUAGES C)
add_executable (creg main.c commands.c user.c sections.c)

find_package(libmongoc-1.0 REQUIRED)
find_package(libbson-1.0 REQUIRED)

target_link_libraries(creg
  mongo::mongoc_shared
  mongo::bson_shared
)