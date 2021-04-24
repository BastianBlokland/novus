#pragma once
#include <cstddef>

namespace vm {

#if defined(_WIN32)
using FileHandle = void*;
#else  // !_WIN32
using FileHandle = int;
#endif // !_WIN32

auto fileInvalid() noexcept -> FileHandle;
auto fileIsValid(FileHandle handle) noexcept -> bool;

auto fileStdIn() noexcept -> FileHandle;
auto fileStdOut() noexcept -> FileHandle;
auto fileStdErr() noexcept -> FileHandle;

auto fileTemp() noexcept -> FileHandle;

auto fileRead(FileHandle file, char* buffer, size_t bufferSize) noexcept -> int;
auto fileWrite(FileHandle file, const char* buffer, size_t bufferSize) noexcept -> int;

auto fileSeek(FileHandle file, size_t position) noexcept -> bool;

auto fileClose(FileHandle file) noexcept -> void;

template <typename... FileHandles>
auto fileClose(FileHandles... files) {
  (fileClose(files), ...);
}

} // namespace vm
