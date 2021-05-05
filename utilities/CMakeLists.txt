function(configure_nov_executable file)
  message(STATUS "Configuring utility: ${file}")

  set(srcFile ${CMAKE_CURRENT_SOURCE_DIR}/${file}.ns)
  set(tgtFile ${file}.nx)

  add_custom_command(
    OUTPUT ${tgtFile}
    COMMAND ${EXECUTABLE_OUTPUT_PATH}/novc ${srcFile} -o ${tgtFile}
    DEPENDS ${srcFile}
    VERBATIM)

  add_custom_target(${file} ALL DEPENDS ${tgtFile} novc)

  add_custom_command(
      TARGET ${file} POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy
      ${CMAKE_CURRENT_BINARY_DIR}/${tgtFile}
      ${EXECUTABLE_OUTPUT_PATH}/${tgtFile})
endfunction(configure_nov_executable)

# Utilities.
configure_nov_executable(niob)
configure_nov_executable(nls)
configure_nov_executable(nov-watch)
configure_nov_executable(nove)
