project(sk-compiler)
cmake_minimum_required(VERSION 2.8)

add_definitions(-Wall -Werror)

set(SOURCE ltg-opt.cpp 
           expand-integer.cpp 
           expand-sk.cpp 
           command.cpp 
           compile.cpp
           program.cpp 
           tool.cpp)

file(GLOB PKG_SOURCE "*.cpp")
file(GLOB PKG_HEADER "*.hpp")
file(GLOB PKG_SCRIPT "prog")

set(EXEC_NAME ltg-opt)
add_executable(${EXEC_NAME} ${SOURCE})

set(PKG_DIR ${EXEC_NAME}.d)

add_definitions("-D WITH_SLEEP")

add_custom_target(package 
                  COMMAND rm -rf ${PKG_DIR} 
                  COMMAND mkdir ${PKG_DIR} 
                  COMMAND mkdir ${PKG_DIR}/src
                  COMMAND cp ${PROJECT_BINARY_DIR}/${EXEC_NAME} ${PKG_DIR}/run
                  COMMAND cp ${PROJECT_SOURCE_DIR}/install ${PKG_DIR}/install
                  COMMAND cp ${PKG_SOURCE} ${PKG_DIR}/src/
                  COMMAND cp ${PKG_HEADER} ${PKG_DIR}/src/
                  COMMAND cp -R ${PKG_SCRIPT} ${PKG_DIR}/src/
                  COMMAND tar -cf ${PROJECT_BINARY_DIR}/${EXEC_NAME}.d.tar ${PKG_DIR}
                  COMMAND gzip -f ${PROJECT_BINARY_DIR}/${EXEC_NAME}.d.tar
                  COMMAND rm -rf ${PKG_DIR})
