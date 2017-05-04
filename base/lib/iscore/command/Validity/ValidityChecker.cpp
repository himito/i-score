#include "ValidityChecker.hpp"
#include <core/document/Document.hpp>
#include <iscore/document/DocumentContext.hpp>
namespace iscore
{

ValidityChecker::~ValidityChecker()
{
}

DocumentValidator ValidityCheckerList::make(const Document& ctx)
{
  return DocumentValidator{*this, ctx};
}

DocumentValidator::DocumentValidator(
    const ValidityCheckerList& l, const Document& doc)
    : m_list{l}, m_doc{doc}
{
}

bool DocumentValidator::operator()() const
{
  bool b = true;
  for (auto& e : m_list)
    b &= e.validate(m_doc.context());
  return b;
}
}
