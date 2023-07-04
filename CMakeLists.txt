project(aion CXX)
cmake_minimum_required(VERSION 3.12)

if(WIN32)
	add_compile_definitions(_WIN32_WINNT=0x0601)
endif()

include_directories("include")
include_directories("3rdparty/asio-1.26.0/include")

if(WIN32)
	include_directories("3rdparty/openssl/include")
endif()

# Shared Library.
file(GLOB_RECURSE shared_SRC CONFIGURE_DEPENDS "include/shared/**.h" "src/shared/**.cpp")
add_library(shared STATIC ${shared_SRC})

# Login Server.
file(GLOB_RECURSE loginserver_SRC CONFIGURE_DEPENDS "src/loginserver/**.h" "src/loginserver/**.cpp")
add_executable(loginserver ${loginserver_SRC})
target_compile_options(loginserver PRIVATE /W4)
add_dependencies(loginserver shared)
target_link_libraries(loginserver shared)
if(WIN32)
	target_link_libraries(loginserver "${CMAKE_SOURCE_DIR}/3rdparty/openssl/lib/libcrypto.lib")
else()
	target_link_libraries(loginserver ssl crypto)
endif()

# World Server.

# Tests.
add_executable(PacketTest "src/test/PacketTest.cpp")
add_dependencies(PacketTest shared)
target_link_libraries(PacketTest shared)