#include "lex/token_itr.hpp"
#include "parse/parser.hpp"
#include <array>

namespace {

const std::array<lex::Token, 66> fuzzTokens = {
    lex::basicToken(lex::TokenKind::OpPlus),
    lex::basicToken(lex::TokenKind::OpPlusPlus),
    lex::basicToken(lex::TokenKind::OpMinus),
    lex::basicToken(lex::TokenKind::OpMinusMinus),
    lex::basicToken(lex::TokenKind::OpStar),
    lex::basicToken(lex::TokenKind::OpSlash),
    lex::basicToken(lex::TokenKind::OpRem),
    lex::basicToken(lex::TokenKind::OpAmp),
    lex::basicToken(lex::TokenKind::OpAmpAmp),
    lex::basicToken(lex::TokenKind::OpPipe),
    lex::basicToken(lex::TokenKind::OpPipePipe),
    lex::basicToken(lex::TokenKind::OpHat),
    lex::basicToken(lex::TokenKind::OpTilde),
    lex::basicToken(lex::TokenKind::OpShiftL),
    lex::basicToken(lex::TokenKind::OpShiftR),
    lex::basicToken(lex::TokenKind::OpEq),
    lex::basicToken(lex::TokenKind::OpEqEq),
    lex::basicToken(lex::TokenKind::OpBang),
    lex::basicToken(lex::TokenKind::OpBangBang),
    lex::basicToken(lex::TokenKind::OpBangEq),
    lex::basicToken(lex::TokenKind::OpLe),
    lex::basicToken(lex::TokenKind::OpLeEq),
    lex::basicToken(lex::TokenKind::OpGt),
    lex::basicToken(lex::TokenKind::OpGtEq),
    lex::basicToken(lex::TokenKind::OpSemi),
    lex::basicToken(lex::TokenKind::OpQMark),
    lex::basicToken(lex::TokenKind::OpQMarkQMark),
    lex::basicToken(lex::TokenKind::OpDot),
    lex::basicToken(lex::TokenKind::OpColonColon),
    lex::basicToken(lex::TokenKind::OpSquareSquare),
    lex::basicToken(lex::TokenKind::OpParenParen),
    lex::basicToken(lex::TokenKind::SepOpenParen),
    lex::basicToken(lex::TokenKind::SepCloseParen),
    lex::basicToken(lex::TokenKind::SepOpenCurly),
    lex::basicToken(lex::TokenKind::SepCloseCurly),
    lex::basicToken(lex::TokenKind::SepOpenSquare),
    lex::basicToken(lex::TokenKind::SepCloseSquare),
    lex::basicToken(lex::TokenKind::SepComma),
    lex::basicToken(lex::TokenKind::SepColon),
    lex::basicToken(lex::TokenKind::SepArrow),
    lex::litIntToken(42),
    lex::litLongToken(42L),
    lex::litFloatToken(42.0),
    lex::litBoolToken(false),
    lex::litStrToken("Hello World"),
    lex::litCharToken('!'),
    lex::keywordToken(lex::Keyword::Intrinsic),
    lex::keywordToken(lex::Keyword::Import),
    lex::keywordToken(lex::Keyword::Fun),
    lex::keywordToken(lex::Keyword::Act),
    lex::keywordToken(lex::Keyword::Self),
    lex::keywordToken(lex::Keyword::Lambda),
    lex::keywordToken(lex::Keyword::Impure),
    lex::keywordToken(lex::Keyword::Fork),
    lex::keywordToken(lex::Keyword::Lazy),
    lex::keywordToken(lex::Keyword::Struct),
    lex::keywordToken(lex::Keyword::Union),
    lex::keywordToken(lex::Keyword::Enum),
    lex::keywordToken(lex::Keyword::If),
    lex::keywordToken(lex::Keyword::Else),
    lex::keywordToken(lex::Keyword::Is),
    lex::keywordToken(lex::Keyword::As),
    lex::identiferToken("Fuzz identifier"),
    lex::lineCommentToken("Hello World"),
    lex::basicToken(lex::TokenKind::Discard),
    lex::errorToken("Fuzz error token"),
};

class TokenItr final : public lex::TokenItrTraits {
public:
  TokenItr(const uint8_t* fuzzPtr) : m_fuzzPtr{fuzzPtr} {}

  auto operator*() -> lex::Token { return fuzzTokens[*m_fuzzPtr % fuzzTokens.size()]; }

  auto operator==(const TokenItr& rhs) noexcept { return m_fuzzPtr == rhs.m_fuzzPtr; }

  auto operator!=(const TokenItr& rhs) noexcept { return m_fuzzPtr != rhs.m_fuzzPtr; }

  auto operator++() { ++m_fuzzPtr; }

private:
  const uint8_t* m_fuzzPtr;
};

} // namespace

extern "C" auto LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) -> int {
  parse::parseAll(TokenItr{data}, TokenItr{data + size});
  return 0;
}
