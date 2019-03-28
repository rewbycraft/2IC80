cmake_minimum_required(VERSION 3.1...3.13)

if(${CMAKE_VERSION} VERSION_LESS 3.12)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()
SET(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
SET(LIBTINS_BUILD_SHARED OFF)
SET(LIBTINS_BUILD_TESTS OFF)
set(LIBTINS_ENABLE_CXX11 ON)

project(attack VERSION 1.0 DESCRIPTION "Disguised LSA attack" LANGUAGES C CXX)

find_package(Git QUIET)
if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
# Update submodules as needed
    option(GIT_SUBMODULE "Check submodules during build" ON)
    if(GIT_SUBMODULE)
        message(STATUS "Submodule update")
        execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
                        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                        RESULT_VARIABLE GIT_SUBMOD_RESULT)
        if(NOT GIT_SUBMOD_RESULT EQUAL "0")
            message(FATAL_ERROR "git submodule update --init failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
        endif()
    endif()
endif()

if(NOT EXISTS "${PROJECT_SOURCE_DIR}/libtins/CMakeLists.txt")
    message(FATAL_ERROR "The submodules were not downloaded! GIT_SUBMODULE was turned off or failed. Please update submodules and try again.")
endif()
if(NOT EXISTS "${PROJECT_SOURCE_DIR}/spdlog/CMakeLists.txt")
    message(FATAL_ERROR "The submodules were not downloaded! GIT_SUBMODULE was turned off or failed. Please update submodules and try again.")
endif()

add_subdirectory(libtins)

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME ON)
find_package(Boost REQUIRED COMPONENTS system program_options)
message(STATUS "Boost version: ${Boost_VERSION}")

add_library(attack_lib STATIC src/main.cpp src/util.cpp src/util.h src/pdus/OSPFv3.cpp src/pdus/OSPFv3.h src/parser/internal.h src/parser/internal.cpp src/parser/Packet.cpp src/parser/Packet.h src/parser/ospf/OSPFv3Packet.cpp src/parser/ospf/OSPFv3Packet.h src/parser/MalformedPacketException.cpp src/parser/MalformedPacketException.h src/parser/ospf/HelloPacket.cpp src/parser/ospf/HelloPacket.h src/parser/ospf/lsa/LSAPacket.cpp src/parser/ospf/lsa/LSAPacket.h src/parser/ospf/DatabaseDescriptionPacket.cpp src/parser/ospf/DatabaseDescriptionPacket.h src/parser/ospf/LinkStateRequestPacket.cpp src/parser/ospf/LinkStateRequestPacket.h src/parser/ospf/LinkStateUpdatePacket.cpp src/parser/ospf/LinkStateUpdatePacket.h src/parser/ospf/LinkStateAcknowledgementPacket.cpp src/parser/ospf/LinkStateAcknowledgementPacket.h src/parser/ospf/lsa/PrefixLSAPacket.cpp src/parser/ospf/lsa/PrefixLSAPacket.h src/parser/ospf/lsa/RouterLSAPacket.cpp src/parser/ospf/lsa/RouterLSAPacket.h src/parser/ospf/lsa/lsa_checksum.cpp src/parser/ospf/lsa/lsa_checksum.h src/parser/ospf/ospf_checksum.cpp src/parser/ospf/ospf_checksum.h src/parser/ospf/lsa/IntraAreaPrefixLSAPacket.cpp src/parser/ospf/lsa/IntraAreaPrefixLSAPacket.h src/parser/ospf/lsa/LinkLSAPacket.cpp src/parser/ospf/lsa/LinkLSAPacket.h src/parser/ospf/lsa/ASExternalLSAPacket.cpp src/parser/ospf/lsa/ASExternalLSAPacket.h src/netns.cpp src/netns.h src/tinshelper.h src/tinshelper.cpp src/parser/ospf/lsa/ChecksumInterface.cpp src/parser/ospf/lsa/ChecksumInterface.h)
target_compile_features(attack_lib PUBLIC cxx_std_17)
set_target_properties(attack_lib PROPERTIES CXX_EXTENSIONS OFF)
target_link_libraries(attack_lib stdc++fs)

target_link_libraries(attack_lib tins Boost::system Boost::program_options)
target_include_directories(attack_lib PRIVATE ${PROJECT_SOURCE_DIR}/libtins/include ${PROJECT_SOURCE_DIR}/spdlog/include)

add_executable(attack src/wrappedmain.cpp)
target_include_directories(attack PRIVATE ${PROJECT_SOURCE_DIR}/libtins/include ${PROJECT_SOURCE_DIR}/spdlog/include)
target_include_directories(attack PUBLIC ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(attack attack_lib)
add_custom_command(TARGET attack POST_BUILD COMMAND sudo setcap CAP_SYS_ADMIN+eip "$<TARGET_FILE:attack>")

add_executable(pcapdump tools/pcapdump.cpp)
target_include_directories(pcapdump PRIVATE ${PROJECT_SOURCE_DIR}/libtins/include ${PROJECT_SOURCE_DIR}/spdlog/include)
target_link_libraries(pcapdump attack_lib)

add_executable(random tools/random.cpp)
target_include_directories(random PRIVATE ${PROJECT_SOURCE_DIR}/src ${PROJECT_SOURCE_DIR}/libtins/include ${PROJECT_SOURCE_DIR}/spdlog/include)
target_link_libraries(random attack_lib)


option(PACKAGE_TESTS "Build the tests" ON)
if(PACKAGE_TESTS)
    enable_testing()
    add_subdirectory(googletest)
    mark_as_advanced(
            BUILD_GMOCK BUILD_GTEST BUILD_SHARED_LIBS
            gmock_build_tests gtest_build_samples gtest_build_tests
            gtest_disable_pthreads gtest_force_shared_crt gtest_hide_internal_symbols
    )

    set_target_properties(gtest PROPERTIES FOLDER googletest)
    set_target_properties(gtest_main PROPERTIES FOLDER googletest)
    set_target_properties(gmock PROPERTIES FOLDER googletest)
    set_target_properties(gmock_main PROPERTIES FOLDER googletest)

    macro(package_add_test TESTNAME)
        add_executable(attack_${TESTNAME} ${ARGN})
        target_link_libraries(attack_${TESTNAME} gtest gmock gtest_main)
        add_test(NAME attack_${TESTNAME} COMMAND attack_${TESTNAME})
        set_target_properties(attack_${TESTNAME} PROPERTIES FOLDER tests)
        target_include_directories(attack_${TESTNAME} PUBLIC ${PROJECT_SOURCE_DIR}/src)
        target_include_directories(attack_${TESTNAME} PRIVATE ${PROJECT_SOURCE_DIR}/libtins/include ${PROJECT_SOURCE_DIR}/spdlog/include)
        target_link_libraries(attack_${TESTNAME} attack_lib)
        add_dependencies(attack attack_${TESTNAME})
    endmacro()
    
    package_add_test(endiantest tests/endiantest.cpp)
    package_add_test(parsertest tests/parsertest.cpp)
    package_add_test(utiltest tests/utiltest.cpp)
    package_add_test(checksumtest tests/checksumtest.cpp)
endif()

