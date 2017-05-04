#pragma once
#include <QMetaType>
#include <iscore/serialization/DataStreamVisitor.hpp>
#include <iscore/serialization/JSONValueVisitor.hpp>

#include <iscore_lib_state_export.h>
#include <memory>

namespace ossia
{
namespace traversal
{
struct path;
}
}

namespace State
{
struct ISCORE_LIB_STATE_EXPORT TraversalPath
{
  Q_GADGET
  ISCORE_SERIALIZE_FRIENDS
public:
  TraversalPath() noexcept;
  TraversalPath(const TraversalPath& other) noexcept;
  TraversalPath(TraversalPath&& other) noexcept;
  TraversalPath& operator=(const TraversalPath& other) noexcept;
  TraversalPath& operator=(TraversalPath&& other) noexcept;
  ~TraversalPath();

  TraversalPath(const QString&, const ossia::traversal::path&) noexcept;
  TraversalPath(QString&&, ossia::traversal::path&&) noexcept;

  static optional<TraversalPath> make_path(QString str);

  operator const ossia::traversal::path&() const noexcept;
  operator ossia::traversal::path&() noexcept;

  bool operator==(const State::TraversalPath& other) const noexcept;
  bool operator!=(const State::TraversalPath& other) const noexcept;

  const ossia::traversal::path& get() const noexcept;
  ossia::traversal::path& get() noexcept;

  const QString& text() const { return m_textual; }
private:
  QString m_textual;
  std::unique_ptr<ossia::traversal::path> m_path;
};
}

Q_DECLARE_METATYPE(State::TraversalPath)
