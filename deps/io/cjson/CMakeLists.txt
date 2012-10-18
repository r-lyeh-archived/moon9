project(cjson C)

include_directories(./)
add_library(cjson STATIC cJSON.c)

#add_dependencies(cjson boost)
