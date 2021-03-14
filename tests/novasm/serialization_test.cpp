#include "catch2/catch.hpp"
#include "helpers.hpp"
#include "novasm/serialization.hpp"

namespace novasm {

static auto testSerializeAndDeserializeAsm(Executable a) {
  // Serialize.
  auto outputString = std::string{};
  serialize(a, std::back_inserter(outputString));
  REQUIRE(outputString.size() > 0);

  // Deserialize.
  auto deserializedAssembly = deserialize(outputString.begin(), outputString.end());
  REQUIRE(deserializedAssembly);

  CHECK(*deserializedAssembly == a);
}

TEST_CASE("[novasm] Assembly serialization", "novasm") {

  SECTION("Basic program") {
    testSerializeAndDeserializeAsm(
        GEN_ASM("fun implicit float(int i) intrinsic{int_to_float}(i) "
                "act main(int i, float f) -> float"
                "  intrinsic{float_add_float}(intrinsic{float_pow}(i, f), intrinsic{float_sin}(f)) "
                "main(42, 1.337)"));
  }

  SECTION("Empty program") { testSerializeAndDeserializeAsm(GEN_ASM("")); }

  SECTION("Basic program with string literals") {
    testSerializeAndDeserializeAsm(GEN_ASM("act main(string strA, string strB) -> string "
                                           "  intrinsic{string_add_string}(strA, strB) "
                                           "main(\"hello\", \"world\")"));
  }
}

} // namespace novasm
