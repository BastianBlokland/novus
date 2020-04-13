#pragma once
#include "gsl.hpp"
#include "internal/fd_utilities.hpp"
#include "internal/ref_stream.hpp"
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

class FileStreamRef final : public StreamRef {
  friend class Allocator;

public:
  FileStreamRef(const FileStreamRef& rhs) = delete;
  FileStreamRef(FileStreamRef&& rhs)      = delete;
  ~FileStreamRef() noexcept override {
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

  [[nodiscard]] auto isValid() noexcept -> bool override { return m_filePtr != nullptr; }

  auto readString(Allocator* alloc, int32_t max) noexcept -> StringRef* override {
    auto strAlloc              = alloc->allocStr(max); // allocStr already does +1 for null-ter.
    auto bytesRead             = std::fread(strAlloc.second, 1U, max, m_filePtr);
    strAlloc.second[bytesRead] = '\0'; // null-terminate.
    strAlloc.first->updateSize(bytesRead);
    return strAlloc.first;
  }

  auto readChar() noexcept -> char override {
    auto res = std::getc(m_filePtr);
    return res > 0 ? static_cast<char>(res) : '\0';
  }

  auto writeString(StringRef* str) noexcept -> bool override {
    return std::fwrite(str->getDataPtr(), str->getSize(), 1, m_filePtr) == 1;
  }

  auto writeChar(uint8_t val) noexcept -> bool override {
    return std::fputc(val, m_filePtr) == val;
  }

  auto flush() noexcept -> void override { std::fflush(m_filePtr); }

  auto setOpts(StreamOpts /*unused*/) noexcept -> bool override { return false; }

  auto unsetOpts(StreamOpts /*unused*/) noexcept -> bool override { return false; }

private:
  gsl::owner<FILE*> m_filePtr;
  gsl::owner<char*> m_filePath;
  FileStreamFlags m_flags;

  inline explicit FileStreamRef(
      gsl::owner<FILE*> filePtr, gsl::owner<char*> filePath, FileStreamFlags flags) noexcept :
      StreamRef{}, m_filePtr{filePtr}, m_filePath{filePath}, m_flags{flags} {}
};

inline auto openFileStream(Allocator* alloc, StringRef* path, FileStreamMode m, FileStreamFlags f)
    -> StreamRef* {

  // Make copy of the file-path string to store in the file-stream.
  gsl::owner<char*> filePath = new char[path->getSize() + 1];     // +1 for null-terminator.
  std::memcpy(filePath, path->getDataPtr(), path->getSize() + 1); // +1 for null-terminator.

  // Open the file.
  char const* modeStr;
  switch (m) {
  case FileStreamMode::Open:
    modeStr = "r+be";
    break;
  case FileStreamMode::Append:
    modeStr = "a+be";
    break;
  case FileStreamMode::Create:
  default:
    modeStr = "w+be";
    break;
  }
  gsl::owner<FILE*> filePtr = std::fopen(filePath, modeStr);

  return alloc->allocPlain<FileStreamRef>(filePtr, filePath, f);
}

inline auto removeFile(StringRef* path) -> bool { return std::remove(path->getCharDataPtr()) == 0; }

} // namespace vm::internal
