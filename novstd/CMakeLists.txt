# Copy all nov standard library files to the output directory and register ctests to evalute them.
# Also generate the 'std.nov' wrapper header that imports the entire standard library.

set(stdHeader ${EXECUTABLE_OUTPUT_PATH}/std.nov)

function(add_nov_test filePath)
  get_filename_component(testName ${filePath} NAME)
  add_test(test_novstd_${testName} ${EXECUTABLE_OUTPUT_PATH}/nove ${filePath})
endfunction(add_nov_test)

function(configure_novstd_file file)
  set(srcFile ${file}.nov)
  set(tgtFile ${EXECUTABLE_OUTPUT_PATH}/std/${file}.nov)

  message(STATUS "Configuring ${file}.nov")

  # Copy to output dir.
  configure_file(${srcFile} ${tgtFile} COPYONLY)

  # Append an import to the 'std.nov' header.
  file(APPEND ${stdHeader} "import \"std/${file}.nov\"\n")

  # Add a test that evaluates the file.
  add_nov_test(${tgtFile})
endfunction(configure_novstd_file)

# Create an empty 'std.nov' file in the output dir.
file(WRITE ${stdHeader} "")

# Standard nov library.
configure_novstd_file(ascii)
configure_novstd_file(bench)
configure_novstd_file(bits)
configure_novstd_file(console)
configure_novstd_file(either)
configure_novstd_file(env)
configure_novstd_file(error)
configure_novstd_file(file)
configure_novstd_file(func)
configure_novstd_file(future)
configure_novstd_file(image)
configure_novstd_file(json)
configure_novstd_file(list)
configure_novstd_file(math)
configure_novstd_file(noise)
configure_novstd_file(option)
configure_novstd_file(pair)
configure_novstd_file(parallel)
configure_novstd_file(parse)
configure_novstd_file(process)
configure_novstd_file(rng)
configure_novstd_file(stream)
configure_novstd_file(tcp)
configure_novstd_file(terminal)
configure_novstd_file(text)
configure_novstd_file(time)
configure_novstd_file(unicode)
configure_novstd_file(utf8)
configure_novstd_file(version)
configure_novstd_file(writer)

# Register a test that evaluates the 'std.nov' header, usefull for detecting name conflics.
add_nov_test(${stdHeader})
