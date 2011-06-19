project(sk-compiler)
cmake_minimum_required(VERSION 2.8)

#add_definitions(-Wall
#  -Werror
#  -Wno-sign-compare
#  )
#add_definitions("-D WITH_SLEEP")

set(SOURCE ltg-opt.cpp 
           expand-integer.cpp 
           expand-sk.cpp 
           command.cpp 
           compile.cpp
           program.cpp 
           tool.cpp
           solve.cpp
           eval.cpp
           zombie-help.cpp
           demos.cpp
           nakamura.cpp
		   lambda-buster.cpp
           virtual-slot.cpp)
           #mempool.cpp)

file(GLOB PKG_SOURCE "*.cpp")
file(GLOB PKG_HEADER "*.hpp")
file(GLOB PKG_SCRIPT "prog")

add_executable(
  run-simulator run-simulator.cpp solve.cpp command.cpp
  expand-integer.cpp
  expand-sk.cpp
  virtual-slot.cpp
  program.cpp
  compile.cpp
  eval.cpp
  tool.cpp)

# DUEL_WITH_IDIOT_PROGRAM : 何もしない相手と対戦する
#set_target_properties(run-simulator PROPERTIES COMPILE_FLAGS "-D DUEL_WITH_IDIOT_PROGRAM")

add_executable(
  duel-simulator duel-simulator.cpp solve.cpp command.cpp
  expand-integer.cpp
  expand-sk.cpp
  virtual-slot.cpp
  program.cpp
  compile.cpp
  eval.cpp
  tool.cpp
  )

target_link_libraries(duel-simulator pthread)
set_target_properties(duel-simulator PROPERTIES COMPILE_FLAGS "-D DUEL_IN_LOCAL")

set(EXEC_NAME ltg-opt)
add_executable(${EXEC_NAME} ${SOURCE})

add_executable(attack0 ${SOURCE})
set_target_properties(attack0 PROPERTIES COMPILE_FLAGS "-D ATTACK0")

add_executable(robust-zombie ${SOURCE})
set_target_properties(robust-zombie PROPERTIES COMPILE_FLAGS "-D ROBUST_ZOMBIE")

add_executable(zombie-help ${SOURCE})
set_target_properties(zombie-help PROPERTIES COMPILE_FLAGS "-D ZOMBIE_HELP")

add_executable(selfcare ${SOURCE})
set_target_properties(selfcare PROPERTIES COMPILE_FLAGS "-D SELFCARE")

add_executable(nakamura ${SOURCE})
set_target_properties(nakamura PROPERTIES COMPILE_FLAGS "-D NAKAMURA")

add_executable(lambda-buster ${SOURCE})
set_target_properties(lambda-buster PROPERTIES COMPILE_FLAGS "-D LAMBDA_BUSTER")

#add_executable(pro ${SOURCE})
#add_executable(opp ${SOURCE})
#set_target_properties(pro PROPERTIES COMPILE_FLAGS "-D PRO_SCRIPT")
#set_target_properties(opp PROPERTIES COMPILE_FLAGS "-D OPP_SCRIPT")

set(PKG_DIR ${EXEC_NAME}.d)

enable_testing()
add_test(match ./ltg.linux64 match pro opp 2> log)

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

