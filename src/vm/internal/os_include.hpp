#pragma once

// Explicitly define the order to include the os-specfic headers in.

// clang-format off
#if defined(_WIN32)

#include <conio.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <fcntl.h>
#include <io.h>
#include <direct.h>
#include <windows.h>

#else // !_WIN32

#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <dirent.h>
#include <termios.h>
#include <pthread.h>
#include <unistd.h>
#include <csignal>
#include <climits>
#include <cerrno>
#include <cstdlib>

#endif // !_WIN32

#if defined(__APPLE__)

#include <mach-o/dyld.h>

#endif // __APPLE__

#if defined(_MSC_VER)

#define PATH_MAX MAX_PATH

#endif // _MSC_VER

// clang-format on
