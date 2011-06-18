project(bot)
cmake_minimum_required(VERSION 2.8)

set(SOURCE bot.cpp)

add_executable(${PROJECT_NAME} ${SOURCE})

add_custom_target(package 
                  COMMAND rm -rf ${PROJECT_NAME}.d
                  COMMAND mkdir ${PROJECT_NAME}.d
                  COMMAND mkdir ${PROJECT_NAME}.d/src
                  COMMAND cp ./${PROJECT_NAME} ${PROJECT_NAME}.d/run
                  COMMAND cp ${PROJECT_SOURCE_DIR}/install ${PROJECT_NAME}.d/install
                  COMMAND cp ${SOURCE} ${PROJECT_NAME}.d/src/
                  COMMAND tar -cf ${PROJECT_NAME}.d.tar ${PROJECT_NAME}.d
                  COMMAND gzip ${PROJECT_NAME}.d.tar)
