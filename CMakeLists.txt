cmake_minimum_required(VERSION 3.5.1)
project(playchain-client)

option(TEST_PLATFORM_ANDROID "Forcibly set PLAYCHAIN_LIB_FOR_ANDROID to test compiling (ON OR OFF)" OFF)
option(TEST_PLATFORM_IOS "Forcibly set PLATFORM_IOS to test compiling (ON OR OFF)" OFF)
option(TEST_PLATFORM_WINDOWS "Forcibly set PLAYCHAIN_LIB_FOR_WINDOWS to test compiling (ON OR OFF)" OFF)

file(GLOB COMMON_CPP "src/*.cpp")
file(GLOB PRIVATE_HEADERS "src/*.h")
file(GLOB HEADERS "include/*.h")

set(CMAKE_CXX_STANDARD 11)

# This is the code to change default values for vendors/rapidjson options
option(RAPIDJSON_BUILD_DOC "Build rapidjson documentation." ON)
option(RAPIDJSON_BUILD_EXAMPLES "Build rapidjson examples." ON)
option(RAPIDJSON_BUILD_TESTS "Build rapidjson perftests and unittests." ON)
set(RAPIDJSON_BUILD_DOC OFF CACHE BOOL "" FORCE)
set(RAPIDJSON_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(RAPIDJSON_BUILD_TESTS OFF CACHE BOOL "" FORCE)

get_property(RAPIDJSON_INCLUDE GLOBAL PROPERTY RAPIDJSON_INCLUDE)
if (NOT RAPIDJSON_INCLUDE)
    set_property(GLOBAL PROPERTY RAPIDJSON_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/vendors/rapidjson/include")
    get_property(RAPIDJSON_INCLUDE GLOBAL PROPERTY RAPIDJSON_INCLUDE)
    add_subdirectory(vendors/rapidjson)
endif()


option(SECP256K1_EXT "Use interface from extended version of SECP256K1-ZKP (ON OR OFF)" OFF)

get_property(SECP256K1_INCLUDE GLOBAL PROPERTY SECP256K1_INCLUDE)
if (NOT SECP256K1_INCLUDE)
    set_property(GLOBAL PROPERTY SECP256K1_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/vendors/secp256k1-zkp/include")
    get_property(SECP256K1_INCLUDE GLOBAL PROPERTY SECP256K1_INCLUDE)

    # Configure secp256k1-zkp
    if ( MSVC )
        # autoconf won't work here, hard code the defines
        set( SECP256K1_DIR "${CMAKE_CURRENT_SOURCE_DIR}/vendors/secp256k1-zkp" )

        file( GLOB SECP256K1_SOURCES "${SECP256K1_DIR}/src/secp256k1.c" )
        add_library( secp256k1 ${SECP256K1_SOURCES} )

        target_include_directories( secp256k1 PRIVATE "${SECP256K1_DIR}" PUBLIC "${SECP256K1_DIR}/include" )

        set( SECP256K1_BUILD_DEFINES
            USE_FIELD_10X26
            USE_FIELD_INV_BUILTIN
            USE_NUM_NONE
            USE_SCALAR_8X32
            USE_SCALAR_INV_BUILTIN )
        set_target_properties( secp256k1 PROPERTIES COMPILE_DEFINITIONS "${SECP256K1_BUILD_DEFINES}" LINKER_LANGUAGE C )
    else ( MSVC )
        include(ExternalProject)
        if ( MINGW )
            ExternalProject_Add( project_secp256k1
                PREFIX ${CMAKE_CURRENT_BINARY_DIR}/vendors/secp256k1-zkp
                SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/vendors/secp256k1-zkp
                CONFIGURE_COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/vendors/secp256k1-zkp/configure --prefix=${CMAKE_CURRENT_BINARY_DIR}/vendors/secp256k1-zkp --with-bignum=no  --host=x86_64-w64-mingw32
                BUILD_COMMAND make
                INSTALL_COMMAND true
                BUILD_BYPRODUCTS ${CMAKE_CURRENT_BINARY_DIR}/vendors/secp256k1-zkp/src/project_secp256k1-build/.libs/libsecp256k1.a)
        else ( MINGW )
            ExternalProject_Add( project_secp256k1
                PREFIX ${CMAKE_CURRENT_BINARY_DIR}/vendors/secp256k1-zkp
                SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/vendors/secp256k1-zkp
                CONFIGURE_COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/vendors/secp256k1-zkp/configure --prefix=${CMAKE_CURRENT_BINARY_DIR}/vendors/secp256k1-zkp --with-bignum=no
                BUILD_COMMAND make
                INSTALL_COMMAND true
                BUILD_BYPRODUCTS ${CMAKE_CURRENT_BINARY_DIR}/vendors/secp256k1-zkp/src/project_secp256k1-build/.libs/libsecp256k1.a)
        endif ( MINGW )

        ExternalProject_Add_Step(project_secp256k1 autogen
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/vendors/secp256k1-zkp
            COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/vendors/secp256k1-zkp/autogen.sh
            DEPENDERS configure)

        ExternalProject_Get_Property(project_secp256k1 binary_dir)

        add_library(secp256k1 STATIC IMPORTED)
        set_property(TARGET secp256k1 PROPERTY IMPORTED_LOCATION ${binary_dir}/.libs/libsecp256k1${CMAKE_STATIC_LIBRARY_SUFFIX})
        set_property(TARGET secp256k1 PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/vendors/secp256k1-zkp/include)
        add_dependencies(secp256k1 project_secp256k1)
        install( FILES ${binary_dir}/.libs/libsecp256k1${CMAKE_STATIC_LIBRARY_SUFFIX} DESTINATION lib/cryptonomex )
    endif ( MSVC )
    # End configure secp256k1-zkp
endif()

include_directories(${RAPIDJSON_INCLUDE})

if (SECP256K1_INCLUDE)
    #require Secp256k1 version >= 2015-09-28
    # with secp256k1_ec_pubkey_serialize
    add_definitions(-DSECP256K1)
    include_directories(${SECP256K1_INCLUDE})
    link_directories(${SECP256K1_LIBRARY_DIR})

    if (SECP256K1_EXT)
        add_definitions(-DSECP256K1_EXT)
    endif()
endif()

find_package(OpenSSL QUIET)

if (OpenSSL_FOUND)
    include_directories(${OPENSSL_INCLUDE_DIR})
endif()

if (TEST_PLATFORM_ANDROID)
    add_definitions(-DANDROID=1)
else()
    if (TEST_PLATFORM_IOS)
        add_definitions(-DPLATFORM_IOS=1)
    else()
        if (TEST_PLATFORM_WINDOWS)
            add_definitions(-DPLATFORM_WINDOWS=1)
        endif()
    endif()
endif()

message(">> RapidJSON include: ${RAPIDJSON_INCLUDE}")
message(">> SECP256K1 include: ${SECP256K1_INCLUDE}")
message(">> OpenSSL include: ${OPENSSL_INCLUDE_DIR}")

add_library( playchain_client ${COMMON_CPP} ${PRIVATE_HEADERS} ${HEADERS})
target_include_directories( playchain_client
                            PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include"
                            PRIVATE "${RAPIDJSON_INCLUDE}")

find_package(Boost 1.58 QUIET COMPONENTS program_options)

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    link_directories(${Boost_LIBRARY_DIRS})
    include_directories(${RAPIDJSON_INCLUDE})

    set(PLAYCHAIN_LIBRARIES_LIST)
    list(APPEND PLAYCHAIN_LIBRARIES_LIST playchain_client ${Boost_LIBRARIES})

    if (SECP256K1_INCLUDE)
        list(APPEND PLAYCHAIN_LIBRARIES_LIST secp256k1)
    endif()

    if (OpenSSL_FOUND)
        LIST(APPEND PLAYCHAIN_LIBRARIES_LIST ${OPENSSL_LIBRARIES})
    endif()

    if (UNIX)
        file(GLOB TEST_SOURCES "tests/*.cpp")
        file(GLOB TEST_HEADERS "tests/*.h")
        add_executable( playchain_client_test ${TEST_SOURCES} ${TEST_HEADERS})
        target_link_libraries( playchain_client_test ${PLAYCHAIN_LIBRARIES_LIST})

        #utils
        add_executable( keys_from_login utils/keys_from_login.cpp)
        target_link_libraries( keys_from_login ${PLAYCHAIN_LIBRARIES_LIST})

        add_executable( pub_from_wif utils/pub_from_wif.cpp)
        target_link_libraries( pub_from_wif ${PLAYCHAIN_LIBRARIES_LIST})

        install( TARGETS
           keys_from_login

           RUNTIME DESTINATION bin
           LIBRARY DESTINATION lib
           ARCHIVE DESTINATION lib
        )
    endif()
endif()

install( TARGETS
   playchain_client

   RUNTIME DESTINATION bin
   LIBRARY DESTINATION lib
   ARCHIVE DESTINATION lib
)
