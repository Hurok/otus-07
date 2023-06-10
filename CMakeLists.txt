cmake_minimum_required(VERSION 3.8.0)

set(PATCH_VERSION 1 CACHE INTERNAL "Patch version")
set(PROJECT_VERSION 0.0.${PATCH_VERSION})

project(bulk VERSION ${PROJECT_VERSION})
set_property(TARGET ${PROJECT} PROPERTY CXX_STANDARD 17)

# cli
add_executable(${PROJECT_NAME} Bulk.h Bulk.cpp main.cpp)

# deb
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
set(CPACK_GENERATOR DEB)
set(CPACK_DEBIAN_FILE_NAME bulk.deb)

set(CPACK_PACKAGE_CONTACT example@example.com)
include (CPack)
