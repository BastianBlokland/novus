#include "novasm/serialization.hpp"
#include <istream>
#include <iterator>
#include <ostream>
#include <string>
#include <vector>

namespace novasm {

template <typename ValueType, typename OutputItr>
using Writer = void(const ValueType& val, OutputItr& outItr);

template <typename ValueType, typename InputItr, typename InputEndItr>
using Reader = std::optional<ValueType>(InputItr&, InputEndItr);

template <typename OutputItr>
static auto writeUInt8(uint8_t val, OutputItr& outItr) -> void {
  *outItr = val;
  ++outItr;
}

template <typename OutputItr>
static auto writeUInt16(uint16_t val, OutputItr& outItr) -> void {
  writeUInt8(val, outItr);
  writeUInt8(val >> 8U, outItr); // NOLINT: Magic number
}

template <typename OutputItr>
static auto writeUInt32(uint32_t val, OutputItr& outItr) -> void {
  writeUInt8(val, outItr);
  writeUInt8(val >> 8U, outItr);  // NOLINT: Magic number
  writeUInt8(val >> 16U, outItr); // NOLINT: Magic number
  writeUInt8(val >> 24U, outItr); // NOLINT: Magic number
}

template <typename InputItrBegin, typename InputItrEnd, typename OutputItr>
static auto writeRaw(InputItrBegin beginItr, InputItrEnd endItr, OutputItr& outItr) -> void {
  for (auto itr = beginItr; itr != endItr; ++itr) {
    writeUInt8(*itr, outItr);
  }
}

template <typename OutputItr>
static auto writeString(const std::string& str, OutputItr& outItr) -> void {
  // Write the length.
  writeUInt32(str.length(), outItr);

  // Write the content.
  writeRaw(str.begin(), str.end(), outItr);
}

template <typename InputItrBegin, typename InputItrEnd, typename ElemType, typename OutputItr>
static auto writeSet(
    InputItrBegin beginItr,
    InputItrEnd endItr,
    Writer<ElemType, OutputItr>* writer,
    OutputItr& outItr) -> void {

  // Write the size.
  auto size = endItr - beginItr;
  writeUInt32(size, outItr);

  // Write the elements.
  for (auto itr = beginItr; itr != endItr; ++itr) {
    writer(*itr, outItr);
  }
}

template <typename InputItr, typename InputEndItr>
static auto readUInt8(InputItr& itr, InputEndItr end) -> std::optional<uint8_t> {
  if (itr == end) {
    return std::nullopt;
  }
  return *itr++;
}

template <typename InputItr, typename InputEndItr>
static auto readUInt16(InputItr& itr, InputEndItr end) -> std::optional<uint16_t> {
  if (itr == end) {
    return std::nullopt;
  }
  auto a = static_cast<uint8_t>(*itr++);
  if (itr == end) {
    return std::nullopt;
  }
  auto b = static_cast<uint8_t>(*itr++);
  return static_cast<uint16_t>(a) | (static_cast<uint16_t>(b) << 8U);
}

template <typename InputItr, typename InputEndItr>
static auto readUInt32(InputItr& itr, InputEndItr end) -> std::optional<uint32_t> {
  if (itr == end) {
    return std::nullopt;
  }
  auto a = static_cast<uint8_t>(*itr++);
  if (itr == end) {
    return std::nullopt;
  }
  auto b = static_cast<uint8_t>(*itr++);
  if (itr == end) {
    return std::nullopt;
  }
  auto c = static_cast<uint8_t>(*itr++);
  if (itr == end) {
    return std::nullopt;
  }
  auto d = static_cast<uint8_t>(*itr++);
  return static_cast<uint32_t>(a) | (static_cast<uint32_t>(b) << 8U) |
      (static_cast<uint32_t>(c) << 16U) | (static_cast<uint32_t>(d) << 24U);
}

template <typename InputItr, typename InputEndItr, typename OutputItr>
static auto readRaw(unsigned int size, InputItr& itr, InputEndItr end, OutputItr& outItr) -> bool {

  for (auto i = 0U; i != size; ++i) {
    if (itr == end) {
      return false;
    }
    *outItr = *itr++;
    outItr++;
  }
  return true;
}

template <typename InputItr, typename InputEndItr>
static auto readString(InputItr& itr, InputEndItr end) -> std::optional<std::string> {
  auto size = readUInt32(itr, end);
  if (!size) {
    return std::nullopt;
  }

  auto result = std::string{};
  result.reserve(*size);
  auto outItr = std::back_inserter(result);
  if (!readRaw(*size, itr, end, outItr)) {
    return std::nullopt;
  }
  return result;
}

template <typename ElemType, typename InputItr, typename InputEndItr>
static auto readSet(InputItr& itr, InputEndItr end, Reader<ElemType, InputItr, InputEndItr>* reader)
    -> std::optional<std::vector<ElemType>> {

  auto size = readUInt32(itr, end);
  if (!size) {
    return std::nullopt;
  }

  auto result = std::vector<ElemType>{};
  result.reserve(*size);
  for (auto i = 0U; i != size; ++i) {
    auto elem = reader(itr, end);
    if (!elem) {
      return std::nullopt;
    }
    result.push_back(std::move(*elem));
  }
  return result;
}

template <typename OutputItr>
auto serialize(const Assembly& assembly, OutputItr outItr) noexcept -> OutputItr {

  // Format version number.
  writeUInt16(assemblyFormatVersion, outItr);

  // Program entry point.
  writeUInt32(assembly.getEntrypoint(), outItr);

  // Program string literals.
  writeSet(assembly.beginLitStrings(), assembly.endLitStrings(), &writeString<OutputItr>, outItr);

  // Program instructions.
  const auto& instructions = assembly.getInstructions();
  writeUInt32(instructions.size(), outItr);
  writeRaw(instructions.begin(), instructions.end(), outItr);

  return outItr;
}

template <typename InputItrBegin, typename InputEndItr>
auto deserialize(InputItrBegin itr, InputEndItr end) noexcept -> std::optional<Assembly> {

  // Format version number.
  auto formatVersionNum = readUInt16(itr, end);
  if (!formatVersionNum) {
    return std::nullopt;
  }

  // Check if the version is supported.
  if (formatVersionNum != assemblyFormatVersion) {
    return std::nullopt;
  }

  // Program entry point.
  auto entryPoint = readUInt32(itr, end);
  if (!entryPoint) {
    return std::nullopt;
  }

  // Program string literals.
  auto stringLiterals = readSet(itr, end, &readString<InputItrBegin, InputEndItr>);
  if (!stringLiterals) {
    return std::nullopt;
  }

  // Program instructions.
  auto progInstructionCount = readUInt32(itr, end);
  if (!progInstructionCount) {
    return std::nullopt;
  }
  auto instructions = std::vector<uint8_t>{};
  instructions.reserve(*progInstructionCount);
  auto instructionOutItr = std::back_inserter(instructions);
  if (!readRaw(*progInstructionCount, itr, end, instructionOutItr)) {
    return std::nullopt;
  }

  return Assembly{*entryPoint, std::move(*stringLiterals), std::move(instructions)};
}

// Explicit instantiations.
template auto serialize(const Assembly&, std::back_insert_iterator<std::string>) noexcept
    -> std::back_insert_iterator<std::string>;
template auto serialize(const Assembly&, std::back_insert_iterator<std::vector<char>>) noexcept
    -> std::back_insert_iterator<std::vector<char>>;
template auto serialize(const Assembly&, std::ostream_iterator<char>) noexcept
    -> std::ostream_iterator<char>;
template auto serialize(const Assembly&, std::ostreambuf_iterator<char>) noexcept
    -> std::ostreambuf_iterator<char>;

template auto deserialize(char*, char*) -> std::optional<Assembly>;
template auto deserialize(std::string::iterator, std::string::iterator) -> std::optional<Assembly>;
template auto deserialize(std::string::const_iterator, std::string::const_iterator)
    -> std::optional<Assembly>;
template auto deserialize(std::istream_iterator<char>, std::istream_iterator<char>)
    -> std::optional<Assembly>;
template auto deserialize(std::istreambuf_iterator<char>, std::istreambuf_iterator<char>)
    -> std::optional<Assembly>;

} // namespace novasm
