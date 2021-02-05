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
#include <termios.h>
#include <unistd.h>
#include <csignal>
#include <climits>
#include <pthread.h>

#endif // !_WIN32

#if defined(__APPLE__)

#include <mach-o/dyld.h>

#endif // __APPLE__

// clang-format on
