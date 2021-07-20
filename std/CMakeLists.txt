# Copy all standard library files to the output directory and register ctests to evalute them.
# Also generate the 'std.ns' wrapper header that imports the entire standard library.

set(stdHeader ${EXECUTABLE_OUTPUT_PATH}/std.ns)

function(add_nov_test filePath)
  get_filename_component(fileName ${filePath} NAME)
  set(testName "[std]\\ ${fileName}")
  add_test("${testName}"
    ${EXECUTABLE_OUTPUT_PATH}/novrt
    ${EXECUTABLE_OUTPUT_PATH}/nove.nx
    --compiler ${EXECUTABLE_OUTPUT_PATH}/novc
    --runtime ${EXECUTABLE_OUTPUT_PATH}/novrt
    ${filePath})
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
configure_std_file(cli)
configure_std_file(cli/api)
configure_std_file(cli/buildin)
configure_std_file(cli/help)
configure_std_file(cli/invoker)
configure_std_file(cli/options)
configure_std_file(cli/parser)
configure_std_file(cli/runner)
configure_std_file(core)
configure_std_file(core/bits)
configure_std_file(core/byte-size)
configure_std_file(core/either)
configure_std_file(core/func)
configure_std_file(core/future)
configure_std_file(core/lazy)
configure_std_file(core/list)
configure_std_file(core/math)
configure_std_file(core/option)
configure_std_file(core/parallel)
configure_std_file(core/reflect)
configure_std_file(core/text)
configure_std_file(core/text-pattern)
configure_std_file(core/time)
configure_std_file(core/tuple)
configure_std_file(diag)
configure_std_file(diag/assert)
configure_std_file(diag/bench)
configure_std_file(diag/error)
configure_std_file(diag/sourceloc)
configure_std_file(format)
configure_std_file(format/ascii)
configure_std_file(format/base64)
configure_std_file(format/image)
configure_std_file(format/json-orm)
configure_std_file(format/json)
configure_std_file(format/parser)
configure_std_file(format/python)
configure_std_file(format/unicode)
configure_std_file(format/utf8)
configure_std_file(format/version)
configure_std_file(format/writer)
configure_std_file(io)
configure_std_file(io/console)
configure_std_file(io/file)
configure_std_file(io/ip)
configure_std_file(io/path)
configure_std_file(io/process)
configure_std_file(io/readline)
configure_std_file(io/stream)
configure_std_file(io/tcp)
configure_std_file(io/tty)
configure_std_file(io/watcher)
configure_std_file(prim)
configure_std_file(prim/bool)
configure_std_file(prim/char)
configure_std_file(prim/float)
configure_std_file(prim/int)
configure_std_file(prim/long)
configure_std_file(prim/meta)
configure_std_file(prim/string)
configure_std_file(prim/type)
configure_std_file(sys)
configure_std_file(sys/env)
configure_std_file(sys/gc)
configure_std_file(sys/rt)
configure_std_file(utilities)
configure_std_file(utilities/color)
configure_std_file(utilities/noise)
configure_std_file(utilities/rng)

# Register a test that evaluates the 'std.ns' header, usefull for detecting name conflics.
add_nov_test(${stdHeader})
