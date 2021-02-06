#pragma once
#include "gsl.hpp"
#include "internal/fd_utilities.hpp"
#include "internal/platform_utilities.hpp"
#include "internal/ref.hpp"
#include "internal/ref_allocator.hpp"
#include "internal/ref_string.hpp"
#include "intrinsics.hpp"
#include <cstdio>
#include <cstring>

namespace vm::internal {

enum class FileStreamMode : uint8_t {
  Create = 0,
  Open   = 1,
  Append = 2,
};

enum FileStreamFlags : uint8_t {
  AutoRemoveFile = 1, // File is deleted when the reference to it is freed.
};

// File implementation of the 'stream' interface.
// Note: To avoid needing a vtable there is no abstract 'Stream' class but instead there are wrapper
// functions that dispatch based on the 'RefKind' (see stream_utilities.hpp).
class FileStreamRef final : public Ref {
  friend class RefAllocator;

public:
  FileStreamRef(const FileStreamRef& rhs) = delete;
  FileStreamRef(FileStreamRef&& rhs)      = delete;
  ~FileStreamRef() noexcept {
    if (m_filePtr) {
      std::fclose(m_filePtr);
      if ((m_flags & AutoRemoveFile) == AutoRemoveFile) {
        std::remove(m_filePath);
      }
    }
    delete[] m_filePath;
  }

  auto operator=(const FileStreamRef& rhs) -> FileStreamRef& = delete;
  auto operator=(FileStreamRef&& rhs) -> FileStreamRef& = delete;

  [[nodiscard]] constexpr static auto getKind() { return RefKind::StreamFile; }

  [[nodiscard]] auto isValid() noexcept -> bool { return m_filePtr != nullptr; }

  auto readString(ExecutorHandle* /*unused*/, PlatformError* /*unused*/, StringRef* str) noexcept
      -> bool {
    if (unlikely(str->getSize() == 0)) {
      return false;
    }

    auto bytesRead = std::fread(str->getCharDataPtr(), 1U, str->getSize(), m_filePtr);
    str->updateSize(bytesRead);
    return bytesRead > 0;
  }

  auto readChar(ExecutorHandle* /*unused*/, PlatformError* /*unused*/) noexcept -> char {
    auto res = std::getc(m_filePtr);
    return res > 0 ? static_cast<char>(res) : '\0';
  }

  auto writeString(ExecutorHandle* /*unused*/, PlatformError* /*unused*/, StringRef* str) noexcept
      -> bool {
    return std::fwrite(str->getDataPtr(), str->getSize(), 1, m_filePtr) == 1;
  }

  auto writeChar(ExecutorHandle* /*unused*/, PlatformError* /*unused*/, uint8_t val) noexcept
      -> bool {
    return std::fputc(val, m_filePtr) == val;
  }

  auto flush() noexcept -> bool { return std::fflush(m_filePtr) == 0; }

  auto setOpts(PlatformError* /*unused*/, StreamOpts /*unused*/) noexcept -> bool { return false; }

  auto unsetOpts(PlatformError* /*unused*/, StreamOpts /*unused*/) noexcept -> bool {
    return false;
  }

private:
  FileStreamFlags m_flags;
  gsl::owner<FILE*> m_filePtr;
  gsl::owner<char*> m_filePath;

  inline explicit FileStreamRef(
      gsl::owner<FILE*> filePtr, gsl::owner<char*> filePath, FileStreamFlags flags) noexcept :
      Ref{getKind()}, m_flags{flags}, m_filePtr{filePtr}, m_filePath{filePath} {}
};

inline auto
openFileStream(RefAllocator* alloc, StringRef* path, FileStreamMode m, FileStreamFlags f)
    -> FileStreamRef* {

  // Make copy of the file-path string to store in the file-stream.
  auto filePath = new gsl::owner<char>[path->getSize() + 1];      // +1 for null-terminator.
  std::memcpy(filePath, path->getDataPtr(), path->getSize() + 1); // +1 for null-terminator.

  // Open the file.
  char const* modeStr;
  switch (m) {
  case FileStreamMode::Open:
    modeStr = "r+b";
    break;
  case FileStreamMode::Append:
    modeStr = "a+b";
    break;
  case FileStreamMode::Create:
  default:
    modeStr = "w+b";
    break;
  }
  gsl::owner<FILE*> filePtr = std::fopen(filePath, modeStr);

  return alloc->allocPlain<FileStreamRef>(filePtr, filePath, f);
}

inline auto removeFile(StringRef* path) -> bool { return std::remove(path->getCharDataPtr()) == 0; }

} // namespace vm::internal
