#include "SetCommentText.hpp"

#include <Scenario/Document/CommentBlock/CommentBlockModel.hpp>
#include <iscore/serialization/DataStreamVisitor.hpp>
#include <iscore/model/path/PathSerialization.hpp>

namespace Scenario
{
namespace Command
{
SetCommentText::SetCommentText(
    Path<CommentBlockModel>&& path, QString newComment)
    : m_path{std::move(path)}, m_newComment{std::move(newComment)}
{
  auto& cmt = m_path.find();
  m_oldComment = cmt.content();
}

void SetCommentText::undo() const
{
  auto& cmt = m_path.find();
  cmt.setContent(m_oldComment);
}

void SetCommentText::redo() const
{
  auto& cmt = m_path.find();
  cmt.setContent(m_newComment);
}

void SetCommentText::serializeImpl(DataStreamInput& s) const
{
  s << m_path << m_newComment << m_oldComment;
}

void SetCommentText::deserializeImpl(DataStreamOutput& s)
{
  s >> m_path >> m_newComment >> m_oldComment;
}
}
}
