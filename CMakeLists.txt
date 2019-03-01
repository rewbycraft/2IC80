cmake_minimum_required(VERSION 3.1...3.13)

if(${CMAKE_VERSION} VERSION_LESS 3.12)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

SET(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
SET(LIBTINS_BUILD_SHARED OFF)

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
find_package(Boost REQUIRED COMPONENTS system)
message(STATUS "Boost version: ${Boost_VERSION}")

add_executable(attack src/main.cpp src/pdus/OSPFv3.cpp src/pdus/OSPFv3.h src/parser/internal.h src/parser/internal.cpp src/parser/Packet.cpp src/parser/Packet.h src/parser/ospf/OSPFv3Packet.cpp src/parser/ospf/OSPFv3Packet.h src/parser/MalformedPacketException.cpp src/parser/MalformedPacketException.h src/parser/ospf/HelloPacket.cpp src/parser/ospf/HelloPacket.h src/parser/ospf/lsa/LSAPacket.cpp src/parser/ospf/lsa/LSAPacket.h src/parser/ospf/DatabaseDescriptionPacket.cpp src/parser/ospf/DatabaseDescriptionPacket.h src/parser/ospf/LinkStateRequestPacket.cpp src/parser/ospf/LinkStateRequestPacket.h src/parser/ospf/LinkStateUpdatePacket.cpp src/parser/ospf/LinkStateUpdatePacket.h src/parser/ospf/LinkStateAcknowledgementPacket.cpp src/parser/ospf/LinkStateAcknowledgementPacket.h src/parser/ospf/lsa/PrefixLSAPacket.cpp src/parser/ospf/lsa/PrefixLSAPacket.h src/parser/ospf/lsa/RouterLSAPacket.cpp src/parser/ospf/lsa/RouterLSAPacket.h src/parser/ospf/lsa/lsa_checksum.cpp src/parser/ospf/lsa/lsa_checksum.h src/parser/ospf/ospf_checksum.cpp src/parser/ospf/ospf_checksum.h)
target_compile_features(attack PUBLIC cxx_std_11)
set_target_properties(attack PROPERTIES CXX_EXTENSIONS OFF)

target_link_libraries(attack tins Boost::system)
target_include_directories(attack PRIVATE ${PROJECT_SOURCE_DIR}/libtins/include ${PROJECT_SOURCE_DIR}/spdlog/include)

