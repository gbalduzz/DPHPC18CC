add_executable(main main.cpp)
target_link_libraries(main ${LIBS})
configure_file("${PROJECT_SOURCE_DIR}/applications/input.json"  COPYONLY)
