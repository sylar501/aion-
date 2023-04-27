project(aion CXX)
cmake_minimum_required(VERSION 3.12)

if(WIN32)
	add_compile_definitions(_WIN32_WINNT=0x0601)
endif()

include_directories("include")
include_directories("3rdparty/asio-1.26.0/include")

file(GLOB_RECURSE shared_SRC CONFIGURE_DEPENDS "include/shared/**.h" "src/shared/**.cpp")
add_library(shared STATIC ${shared_SRC})

file(GLOB_RECURSE loginserver_SRC CONFIGURE_DEPENDS "src/loginserver/**.h" "src/loginserver/**.cpp")
add_executable(loginserver ${loginserver_SRC})
add_dependencies(loginserver shared)
target_link_libraries(loginserver shared)