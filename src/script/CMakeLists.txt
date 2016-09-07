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

target_link_libraries(${MODULE_NAME} anyoption)

add_library(opentxs-client SHARED IMPORTED)
add_library(opentxs-core SHARED IMPORTED)

set_property(TARGET opentxs-client PROPERTY IMPORTED_LOCATION ${OPENTXS-CLIENT})
set_property(TARGET opentxs-core PROPERTY IMPORTED_LOCATION ${OPENTXS-CORE})

target_link_libraries(${MODULE_NAME} PRIVATE opentxs-client)
target_link_libraries(${MODULE_NAME} PRIVATE opentxs-core)

install(TARGETS ${MODULE_NAME}
        DESTINATION bin
        COMPONENT main
)
