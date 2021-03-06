# Copy all standard library files to the output directory and register ctests to evalute them.
# Also generate the 'std.ns' wrapper header that imports the entire standard library.

set(stdHeader ${EXECUTABLE_OUTPUT_PATH}/std.ns)

function(add_nov_test filePath)
  get_filename_component(fileName ${filePath} NAME)
  set(testName "[std]\\ ${fileName}")
  add_test("${testName}" ${EXECUTABLE_OUTPUT_PATH}/nove ${filePath})
endfunction(add_nov_test)

function(configure_std_file file)
  set(srcFile ${file}.ns)
  set(tgtFile ${EXECUTABLE_OUTPUT_PATH}/std/${file}.ns)

  message(STATUS "Configuring ${file}.ns")

  # Copy to output dir.
  configure_file(${srcFile} ${tgtFile} COPYONLY)

  # Append an import to the 'std.ns' header.
  file(APPEND ${stdHeader} "import \"std/${file}.ns\"\n")

  # Add a test that evaluates the file.
  add_nov_test(${tgtFile})
endfunction(configure_std_file)

# Create an empty 'std.ns' file in the output dir.
file(WRITE ${stdHeader} "")

# Standard nov library.
configure_std_file(ascii)
configure_std_file(bench)
configure_std_file(bits)
configure_std_file(char)
configure_std_file(color)
configure_std_file(console)
configure_std_file(either)
configure_std_file(env)
configure_std_file(error)
configure_std_file(file)
configure_std_file(func)
configure_std_file(future)
configure_std_file(image)
configure_std_file(ip)
configure_std_file(json)
configure_std_file(lazy)
configure_std_file(list)
configure_std_file(math)
configure_std_file(noise)
configure_std_file(option)
configure_std_file(pair)
configure_std_file(parallel)
configure_std_file(parser)
configure_std_file(path)
configure_std_file(process)
configure_std_file(rng)
configure_std_file(rt)
configure_std_file(sourceloc)
configure_std_file(stream)
configure_std_file(tcp)
configure_std_file(tty)
configure_std_file(type)
configure_std_file(text)
configure_std_file(time)
configure_std_file(unicode)
configure_std_file(utf8)
configure_std_file(version)
configure_std_file(writer)

# Register a test that evaluates the 'std.ns' header, usefull for detecting name conflics.
add_nov_test(${stdHeader})
