function(configure_nov_executable app file)
  message(STATUS "Configuring utility: ${app}")

  set(srcFile ${CMAKE_CURRENT_SOURCE_DIR}/${file})
  set(tgtFile ${app}.nx)

  add_custom_command(
    OUTPUT ${tgtFile}
    COMMAND ${EXECUTABLE_OUTPUT_PATH}/novc ${srcFile} -o ${tgtFile}
    DEPENDS ${srcFile}
    VERBATIM)

  add_custom_target(${app} ALL DEPENDS ${tgtFile} novc)

  add_custom_command(
      TARGET ${app} POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy
      ${CMAKE_CURRENT_BINARY_DIR}/${tgtFile}
      ${EXECUTABLE_OUTPUT_PATH}/${tgtFile})
endfunction(configure_nov_executable)

# Utilities.
configure_nov_executable(cdb        cdb/app.ns)
configure_nov_executable(np4        np4/app.ns)
configure_nov_executable(niob       niob.ns)
configure_nov_executable(nls        nls.ns)
configure_nov_executable(nov-watch  nov-watch.ns)
configure_nov_executable(nove       nove.ns)
