cmake_minimum_required(VERSION 3.7)
project(Projet_FTP)

set(CMAKE_C_STANDARD 11)

set(CLIENT_SOURCE_FILES ftp_client.c ftp_common.c ftp_common.h)
set(SERVER_SOURCE_FILES ftp_server.c ftp_common.c ftp_common.h)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin)

add_executable(ftp_client ${CLIENT_SOURCE_FILES})
add_executable(ftp_server ${SERVER_SOURCE_FILES})