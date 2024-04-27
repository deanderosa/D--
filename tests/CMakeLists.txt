# Turn on GoogleTest
enable_testing()
find_package(GTest REQUIRED)

# Specify test targets and fils
set(TestTargets "LexerTests" "ParserTests")
set(TestFiles "lexer_tests.cpp" "parser_tests.cpp")
list(LENGTH TestTargets list_length)

# Register a GoogleTest target for a given file
macro(register_gtest TestTarget TestFile)
  #add_executable(${TestTarget} ${TestFile} ${SOURCE_FILES})
  add_executable(${TestTarget} ${TestFile} "${SRC_DIR}/parser.cpp" "${SRC_DIR}/lexer.cpp")
  target_link_libraries(
    ${TestTarget}
    GTest::gtest_main
  )
  add_dependencies(${TestTarget} Driver)
endmacro()

math(EXPR real_idx "${list_length} - 1" OUTPUT_FORMAT DECIMAL)
foreach(index RANGE ${real_idx})
  list(GET TestTargets ${index} target)
  list(GET TestFiles ${index} file)
  register_gtest(${target} ${file})
endforeach()

gtest_discover_tests(${TestTargets})

# Create Test target that runs all generated test targets
set(TestCommands)
foreach(Test ${TestTargets})
  list(APPEND TestCommands
    COMMAND ./${Test}  )
endforeach()

add_custom_target(Test
  DEPENDS ${TestTargets}
  ${TestCommands}
)
