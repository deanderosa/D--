# Define source files
set(SOURCE_FILES "parser.cpp" "lexer.cpp")
set(MAIN_FILES "driver.cpp" ${SOURCE_FILES})

# Add the executable target
add_executable(Driver ${MAIN_FILES})
