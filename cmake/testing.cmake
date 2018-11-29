################################################################################
# Enables testing.
# References: - https://github.com/ALPSCore/ALPSCore

set(TEST_RUNNER "mpirun" CACHE STRING "Command to launch test")

set(MPIEXEC_NUMPROC_FLAG "--np" CACHE STRING "Flag to set number of processes.")

include(CMakeParseArguments)

# Adds a test written with Google Test.
#
# dca_add_gtest(name
#               [MPI_NUMPROC procs]
#               [INCLUDE_DIRS dir1 [dir2 ...]]
#               [SOURCES src1 [src2 ...]]
#               [LIBS lib1 [lib2 ...]])
#
# Adds a test called 'name', the source is assumed to be 'name.cpp'.
# MPI_NUMPROC is the number of MPI processes to use for a test with MPI, the default value is 1.
function(add_gtest name)
  set(oneValueArgs MPI_NUMPROC)
  set(multiValueArgs INCLUDE_DIRS SOURCES LIBS)
  cmake_parse_arguments(ADD_GTEST "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

  add_executable(${name} ${name}.cpp ${ADD_GTEST_SOURCES})

  target_include_directories(${name} PRIVATE ${INCLUDE_DIRS}
                             ${PROJECT_SOURCE_DIR}/libs/googletest-1.8.0/include)

  # Create a macro with the project source dir. We use this as the root path for reading files in
  # tests.
  target_compile_definitions(${name} PRIVATE SOURCE_DIR=\"${PROJECT_SOURCE_DIR}\")

  # Use gtest main.
  target_link_libraries(${name} ${ADD_GTEST_LIBS} gtest gtest_main)

  target_include_directories(${name} PRIVATE
    ${gtest_SOURCE_DIR}/include
    ${ADD_GTEST_INCLUDE_DIRS})

    if (NOT DEFINED ADD_GTEST_MPI_NUMPROC)
      set(ADD_GTEST_MPI_NUMPROC 1)
    endif()

    add_test(NAME ${name}
             COMMAND ${TEST_RUNNER} ${MPIEXEC_NUMPROC_FLAG} ${ADD_GTEST_MPI_NUMPROC}
                     ${MPIEXEC_PREFLAGS} "$<TARGET_FILE:${name}>")
                 target_link_libraries(${name} ${MPI_C_LIBRARIES})
endfunction()
