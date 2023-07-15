project(aion CXX)
cmake_minimum_required(VERSION 3.12)

if(WIN32)
	add_compile_definitions(_WIN32_WINNT=0x0601)
endif()

include_directories("include")
include_directories("3rdparty/asio-1.26.0/include")
include_directories("3rdparty/nlohmann-json-3.11.2/include")

if(WIN32)
	include_directories("3rdparty/openssl-3.1.1/include")
	include_directories("3rdparty/libpqxx-7.7.5/include")
endif()

# Shared Library.
file(GLOB_RECURSE shared_SRC CONFIGURE_DEPENDS "include/shared/**.h" "src/shared/**.cpp")
add_library(shared STATIC ${shared_SRC})
set_property(TARGET shared PROPERTY CXX_STANDARD 17)

# Login Server.
file(GLOB_RECURSE loginserver_SRC CONFIGURE_DEPENDS "src/loginserver/**.h" "src/loginserver/**.cpp")
add_executable(loginserver ${loginserver_SRC})
set_property(TARGET loginserver PROPERTY CXX_STANDARD 17)
target_compile_options(loginserver PRIVATE /W4)
add_dependencies(loginserver shared)
target_link_libraries(loginserver shared)
if(WIN32)
	target_link_libraries(loginserver "${CMAKE_SOURCE_DIR}/3rdparty/openssl-3.1.1/lib/VC/static/libcrypto64MTd.lib" crypt32)
	target_link_libraries(loginserver "${CMAKE_SOURCE_DIR}/3rdparty/libpqxx-7.7.5/lib/pqxx.lib")
	target_link_libraries(loginserver "${CMAKE_SOURCE_DIR}/3rdparty/libpqxx-7.7.5/lib/libpq.lib")
else()
	target_link_libraries(loginserver ssl crypto)
endif()

# World Server.
file(GLOB_RECURSE worldserver_SRC CONFIGURE_DEPENDS "src/worldserver/**.h" "src/worldserver/**.cpp")
add_executable(worldserver ${worldserver_SRC})
set_property(TARGET worldserver PROPERTY CXX_STANDARD 17)
target_compile_options(worldserver PRIVATE /W4)
add_dependencies(worldserver shared)
target_link_libraries(worldserver shared)
if(WIN32)
	target_link_libraries(worldserver "${CMAKE_SOURCE_DIR}/3rdparty/openssl-3.1.1/lib/VC/static/libcrypto64MTd.lib" crypt32)
else()
	target_link_libraries(worldserver ssl crypto)
endif()

# Tests.
add_executable(PacketTest "src/test/PacketTest.cpp")
add_dependencies(PacketTest shared)
target_link_libraries(PacketTest shared)