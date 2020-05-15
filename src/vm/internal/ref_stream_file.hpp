#pragma once
#include "gsl.hpp"
#include "internal/fd_utilities.hpp"
#include "internal/ref.hpp"
#include "internal/ref_allocator.hpp"
#include "internal/ref_string.hpp"
#include <cstdio>
#include <cstring>

namespace vm::internal {

enum class FileStreamMode : uint8_t {
  Create = 0,
  Open   = 1,
  Append = 2,
};

enum FileStreamFlags : uint8_t {
  AutoRemoveFile = 1,
};

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

  auto readString(RefAllocator* alloc, int32_t max) noexcept -> StringRef* {
    auto strAlloc              = alloc->allocStr(max); // allocStr already does +1 for null-ter.
    auto bytesRead             = std::fread(strAlloc.second, 1U, max, m_filePtr);
    strAlloc.second[bytesRead] = '\0'; // null-terminate.
    strAlloc.first->updateSize(bytesRead);
    return strAlloc.first;
  }

  auto readChar() noexcept -> char {
    auto res = std::getc(m_filePtr);
    return res > 0 ? static_cast<char>(res) : '\0';
  }

  auto writeString(StringRef* str) noexcept -> bool {
    return std::fwrite(str->getDataPtr(), str->getSize(), 1, m_filePtr) == 1;
  }

  auto writeChar(uint8_t val) noexcept -> bool { return std::fputc(val, m_filePtr) == val; }

  auto flush() noexcept -> bool { return std::fflush(m_filePtr) == 0; }

  auto setOpts(StreamOpts /*unused*/) noexcept -> bool { return false; }

  auto unsetOpts(StreamOpts /*unused*/) noexcept -> bool { return false; }

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
