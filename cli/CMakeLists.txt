cmake_minimum_required(VERSION 3.11)
project(cli LANGUAGES C)
add_executable (creg commands.c commands_helper.c handle.c help.c user.c sections.c main.c)

find_package(libmongoc-1.0 REQUIRED)
find_package(libbson-1.0 REQUIRED)

target_link_libraries(creg
  mongo::mongoc_shared
  mongo::bson_shared
)