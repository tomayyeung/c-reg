cmake_minimum_required(VERSION 3.11)
project(c-quickstart LANGUAGES C)
add_executable (quickstart.out quickstart.c)
add_executable(insertCourse.out insertCourse.c)

find_package(libmongoc-1.0 REQUIRED)
find_package(libbson-1.0 REQUIRED)

target_link_libraries(quickstart.out
  mongo::mongoc_shared
  mongo::bson_shared
)

target_link_libraries(insertCourse.out
  mongo::mongoc_shared
  mongo::bson_shared
)