#include "parse/node_stmt_import.hpp"
#include "utilities.hpp"

namespace parse {

ImportStmtNode::ImportStmtNode(lex::Token kw, lex::Token path) :
    m_kw{std::move(kw)}, m_path{std::move(path)} {}

auto ImportStmtNode::operator==(const Node& rhs) const noexcept -> bool {
  const auto r = dynamic_cast<const ImportStmtNode*>(&rhs);
  return r != nullptr && m_path == r->m_path;
}

auto ImportStmtNode::operator!=(const Node& rhs) const noexcept -> bool {
  return !ImportStmtNode::operator==(rhs);
}

auto ImportStmtNode::operator[](unsigned int /*unused*/) const -> const Node& {
  throw std::out_of_range("No child at given index");
}

auto ImportStmtNode::getChildCount() const -> unsigned int { return 0U; }

auto ImportStmtNode::getSpan() const -> input::Span {
  return input::Span::combine(m_kw.getSpan(), m_path.getSpan());
}

auto ImportStmtNode::getPath() const -> const lex::Token& { return m_path; }

auto ImportStmtNode::getPathString() const -> std::string {
  const auto result = getString(m_path);
  if (!result) {
    throw std::logic_error{"Invalid import statement"};
  }
  return *result;
}

auto ImportStmtNode::accept(NodeVisitor* visitor) const -> void { visitor->visit(*this); }

auto ImportStmtNode::print(std::ostream& out) const -> std::ostream& {
  return out << "import-\"" << ::parse::getString(m_path).value_or("error") << '"';
}

// Factories.
auto importStmtNode(lex::Token kw, lex::Token path) -> NodePtr {
  return std::unique_ptr<ImportStmtNode>{new ImportStmtNode{std::move(kw), std::move(path)}};
}

} // namespace parse
