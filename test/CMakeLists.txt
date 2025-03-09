

include_directories(include)

add_executable(unittest unit.cpp ../src/mathfunctions.cpp)

add_test(NAME mytest COMMAND unittest)
