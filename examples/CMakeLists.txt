

add_executable(example main.cpp)

target_link_libraries(example PRIVATE imufungen)

add_custom_command(TARGET example POST_BUILD COMMAND "./example")
