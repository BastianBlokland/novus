function(configure_novstd_file file)
  set(srcFile ${file}.nov)
  set(tgtFile ${EXECUTABLE_OUTPUT_PATH}/std/${file}.nov)

  # Copy to output dir.
  configure_file(${srcFile} ${tgtFile} COPYONLY)

  # Add a test that evaluates the file.
  add_test(test_novstd_${file} ${EXECUTABLE_OUTPUT_PATH}/eval ${tgtFile})
endfunction(configure_novstd_file)

# Standard nov library.
configure_novstd_file(list)
configure_novstd_file(math)
configure_novstd_file(option)
configure_novstd_file(pair)
configure_novstd_file(text)

# Add a dependency on the 'eval' executable to be able to test the nov files.
add_dependencies(tests eval)
