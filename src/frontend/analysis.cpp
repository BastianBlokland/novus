#include "frontend/analysis.hpp"
#include "prog/program.hpp"
#include "visitors/declare_user_funcs.hpp"
#include "visitors/get_parse_diags.hpp"
#include <memory>
#include <vector>

namespace frontend {

auto analyze(const Source& src) -> Output {
  // Check source for any parse errors.
  auto getParseDiags = visitors::GetParseDiags{src};
  src.accept(&getParseDiags);
  if (getParseDiags.hasErrors()) {
    return buildOutput(nullptr, getParseDiags.getDiags());
  }

  auto prog = std::make_unique<prog::Program>();

  // Declare any user functions.
  auto declareUserFuncs = visitors::DeclareUserFuncs{src, prog.get()};
  src.accept(&declareUserFuncs);
  if (declareUserFuncs.hasErrors()) {
    return buildOutput(nullptr, getParseDiags.getDiags());
  }

  return buildOutput(std::move(prog), {});
}

} // namespace frontend
