#include "prog/operator.hpp"
#include <stdexcept>

namespace prog {

auto getFuncName(Operator op) -> std::string {
  switch (op) {
  case Operator::Plus:
    return "__op_plus";
  case Operator::Minus:
    return "__op_minus";
  case Operator::Star:
    return "__op_star";
  case Operator::Slash:
    return "__op_slash";
  case Operator::Rem:
    return "__op_rem";
  case Operator::Amp:
    return "__op_amp";
  case Operator::Pipe:
    return "__op_pipe";
  case Operator::EqEq:
    return "__op_eqeq";
  case Operator::Bang:
    return "__op_bang";
  case Operator::BangEq:
    return "__op_bangeq";
  case Operator::Le:
    return "__op_less";
  case Operator::LeEq:
    return "__op_lesseq";
  case Operator::Gt:
    return "__op_gt";
  case Operator::GtEq:
    return "__op_gteq";
  case Operator::ColonColon:
    return "__op_coloncolon";
  }
  throw std::invalid_argument("Unknown operator");
}

} // namespace prog
