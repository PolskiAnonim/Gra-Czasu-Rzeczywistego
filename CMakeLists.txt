cmake_minimum_required (VERSION 3.20)
project ("sieci")
if(WIN32)
   include(C:/vcpkg/scripts/buildsystems/vcpkg.cmake)
endif()
if(NOT DEFINED CMAKE_TOOLCHAIN_FILE AND DEFINED ENV{CMAKE_TOOLCHAIN_FILE})
    set(CMAKE_TOOLCHAIN_FILE $ENV{CMAKE_TOOLCHAIN_FILE})
endif()

if(UNIX)
    file(GLOB SOURCES_KLIENT CONFIGURE_DEPENDS
      "${CMAKE_CURRENT_SOURCE_DIR}/klient/src/*/*/*.cpp"
      "${CMAKE_CURRENT_SOURCE_DIR}/klient/src/*/*/*.h"
      "${CMAKE_CURRENT_SOURCE_DIR}/klient/src/*/*.cpp"
      "${CMAKE_CURRENT_SOURCE_DIR}/klient/src/*/*.h"
      "${CMAKE_CURRENT_SOURCE_DIR}/klient/src/*.cpp"
      "${CMAKE_CURRENT_SOURCE_DIR}/klient/src/*.h"


      "${CMAKE_CURRENT_SOURCE_DIR}/networking/src/*/*.h"
      "${CMAKE_CURRENT_SOURCE_DIR}/networking/src/*/*.cpp"
      "${CMAKE_CURRENT_SOURCE_DIR}/networking/src/*.h"
      "${CMAKE_CURRENT_SOURCE_DIR}/networking/src/*.cpp"
    )
endif()
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS OFF)


if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2")
endif()

if(UNIX)
    file(GLOB SOURCES_SERWER CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/serwer/src/*/*/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/serwer/src/*/*/*.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/serwer/src/*/*.cpp" 
        "${CMAKE_CURRENT_SOURCE_DIR}/serwer/src/*/*.hpp"     
        "${CMAKE_CURRENT_SOURCE_DIR}/serwer/src/*.cpp" 
        "${CMAKE_CURRENT_SOURCE_DIR}/serwer/src/*.hpp"     
        "${CMAKE_CURRENT_SOURCE_DIR}/networking/src/*.h"  
        "${CMAKE_CURRENT_SOURCE_DIR}/networking/src/*.cpp"  
    )
    set(SOURCE_RESOURCES_DIR "./resources")
    file(COPY ${SOURCE_RESOURCES_DIR} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
    include_directories(${CMAKE_SOURCE_DIR}/serwer/include)
    add_executable (Serwer "${SOURCES_SERWER}")
    set_target_properties(Serwer PROPERTIES COMPILE_OPTIONS "-lsctp")
    target_link_libraries(Serwer PRIVATE sctp pthread)

endif()

if(UNIX)
    set(SOURCE_RESOURCES_DIR "./resources")
    include_directories(${CMAKE_SOURCE_DIR}/klient/include)


    # Dependencies
    set(RAYLIB_VERSION 5.0)
    find_package(raylib ${RAYLIB_VERSION} QUIET) # QUIET or REQUIRED
    if (NOT raylib_FOUND) # If there's none, fetch and build raylib
        include(FetchContent)
        FetchContent_Declare(
                raylib
                DOWNLOAD_EXTRACT_TIMESTAMP OFF
                URL https://github.com/raysan5/raylib/archive/refs/tags/${RAYLIB_VERSION}.tar.gz
        )
        FetchContent_GetProperties(raylib)
        if (NOT raylib_POPULATED) # Have we downloaded raylib yet?
            set(FETCHCONTENT_QUIET NO)
            FetchContent_Populate(raylib)
            set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) # don't build the supplied examples
            add_subdirectory(${raylib_SOURCE_DIR} ${raylib_BINARY_DIR})
        endif()
    endif()
    add_executable (Klient "${SOURCES_KLIENT}")
    set_target_properties(Klient PROPERTIES COMPILE_OPTIONS "-lsctp -O2")
    target_compile_options(Klient PRIVATE
            $<$<OR:$<CONFIG:Release>,$<CONFIG:RelWithDebInfo>>:-O2>
            $<$<CONFIG:Release>:-DNDEBUG>
    )
    target_link_options(Klient PRIVATE $<$<CONFIG:Release>:-O2>)

    target_link_libraries(Klient PRIVATE raylib sctp pthread)
endif()

# To uruchamia serwer w nowym oknie w wsl
if(UNIX)
    add_custom_target(RunServer
            # Tak... otwiera to wsl który otwiera basha który otwiera wsl który odpala serwer
           ${WSL_EXECUTABLE} bash -c "cmd.exe /c start wsl --cd '${CMAKE_CURRENT_BINARY_DIR}' ./Serwer"
           DEPENDS Serwer
           WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )

else()
    add_custom_target(RunServer
        COMMAND ${WSL_EXECUTABLE} start ${CMAKE_CURRENT_BINARY_DIR}/serwer
        DEPENDS Serwer
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )
endif()


# To do kompilowania obydwu jednocześnie
if(UNIX)
add_custom_target(RunBoth
        ${WSL_EXECUTABLE} bash -c "cmd.exe /c start wsl --cd '${CMAKE_CURRENT_BINARY_DIR}' ./Serwer &"
        ${WSL_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Klient
        DEPENDS Serwer Klient
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)
else()
    add_custom_target(RunBoth
        COMMAND start ${CMAKE_CURRENT_BINARY_DIR}/klient
        COMMAND ${WSL_EXECUTABLE} bash -c "cmd.exe /c start wsl --cd '/mnt/c/Users/pc/Desktop/sieci/sieci/cmake-build-wsl' ./Serwer"
        DEPENDS Klient Serwer
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )
endif()

