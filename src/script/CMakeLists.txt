set(cxx-sources
  main.cpp
)

include_directories(SYSTEM ${CZMQ_INCLUDE_DIR})

set(MODULE_NAME ot)
if (WIN32)
  configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/winexe.rc.in
    ${CMAKE_CURRENT_BINARY_DIR}/module.rc
    @ONLY
  )

  add_executable(
    ${MODULE_NAME}
    ${cxx-sources}
    ${CMAKE_CURRENT_BINARY_DIR}/module.rc
  )
else()
  add_executable(${MODULE_NAME} ${cxx-sources})
endif()

target_link_libraries(${MODULE_NAME} ${OPENTXS_LIBRARIES} anyoption)

install(TARGETS ${MODULE_NAME}
        DESTINATION bin
        COMPONENT main
)
