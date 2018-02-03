project(kv-service)
cmake_minimum_required(VERSION 3.2)
add_compile_options(-std=c++11)
#add_definitions(-DAWS_CUSTOM_MEMORY_MANAGEMENT)

#set(CMAKE_FIND_LIBRARY_SUFFIXES ".a;.so")
set(BUILD_SHARED_LIBS OFF)
set(CMAKE_EXE_LINKER_FLAGS "-static")

#SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/${PROJECT_NAME}/lib")
#SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

# Include cmake packages 
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}")
find_package(AWSSDK REQUIRED)
find_package(GRPC REQUIRED)

set(PROTOS ${CMAKE_CURRENT_SOURCE_DIR}/protos/kv.proto)
set(PROTO_SRC_DIR ${CMAKE_CURRENT_BINARY_DIR}/protos-src)
file(MAKE_DIRECTORY ${PROTO_SRC_DIR})
include_directories(${PROTO_SRC_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

# Generate gRPC classes
grpc_generate_cpp(GRPC_SRCS GRPC_HDRS ${PROTOS})

# Building server
add_executable(kv-server
    ${CMAKE_CURRENT_SOURCE_DIR}/kvServer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/service/SettingsImpl.cpp
    ${GRPC_SRCS}
    ${GRPC_HDRS}
)
target_link_libraries(kv-server
    libprotobuf.a
	grpc++.a grpc.a gpr.a
	z.a	ssl.a crypto.a cares.a
	curl.a
	dl pthread
#    aws-cpp-sdk-core.a aws-cpp-sdk-dynamodb.a aws-cpp-sdk-kms.a
    aws-cpp-sdk-core aws-cpp-sdk-dynamodb aws-cpp-sdk-kms
)

# Building client
add_executable(kv-client
    ${CMAKE_CURRENT_SOURCE_DIR}/kvClient.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/service/SettingsClient.cpp
    ${GRPC_SRCS}
    ${GRPC_HDRS}
)
target_link_libraries(kv-client
    libprotobuf.a
	grpc++.a grpc.a gpr.a
    grpc.a
    gpr.a
	z.a	ssl.a crypto.a cares.a
    dl pthread
)

install(CODE "execute_process(COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/scripts/cpld.sh ${CMAKE_CURRENT_BINARY_DIR}/kv-server ${CMAKE_CURRENT_BINARY_DIR}/lib)")
install(
	TARGETS kv-server 
	PERMISSIONS OWNER_READ GROUP_READ WORLD_READ OWNER_WRITE OWNER_EXECUTE GROUP_EXECUTE WORLD_EXECUTE 
	DESTINATION ${PROJECT_NAME}/bin)
install(
	DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib
	DESTINATION ${PROJECT_NAME})

# Generate RPM
INCLUDE(InstallRequiredSystemLibraries)
set(CPACK_PACKAGE_VERSION "1.0.0")
set(CPACK_PACKAGE_NAME "KV-Server")
set(CPACK_PACKAGE_RELEASE 1)
set(CPACK_PACKAGE_CONTACT "Fabricio Figueiredo")
set(CPACK_PACKAGE_VENDOR "Meketrefe Studios")
set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CPACK_PACKAGE_RELEASE}.${CMAKE_SYSTEM_PROCESSOR}")
set(CPACK_MONOLITHIC_INSTALL 1)
set(CPACK_COMPONENTS_ALL_IN_ONE_PACKAGE 1)
set(CPACK_GENERATOR "RPM")
set(CPACK_RPM_COMPONENT_INSTALL ON)
set(CPACK_RPM_PACKAGE_AUTOREQ 0)
set(CPACK_PACKAGING_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})
include(CPack)
