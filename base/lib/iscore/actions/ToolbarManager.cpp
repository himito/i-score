#include "ToolbarManager.hpp"
#include <QToolBar>
namespace iscore
{

Toolbar::Toolbar(
    QToolBar* tb, StringKey<Toolbar> key, int defaultRow, int defaultCol)
    : m_impl{tb}
    , m_key{std::move(key)}
    , m_row{defaultRow}
    , m_col{defaultCol}
    , m_defaultRow{defaultRow}
    , m_defaultCol{defaultCol}
{
}

QToolBar* Toolbar::toolbar() const
{
  return m_impl;
}

StringKey<Toolbar> Toolbar::key() const
{
  return m_key;
}

int Toolbar::row() const
{
  return m_row;
}

int Toolbar::column() const
{
  return m_col;
}

void ToolbarManager::insert(Toolbar val)
{
  m_container.insert(std::make_pair(val.key(), std::move(val)));
}

void ToolbarManager::insert(std::vector<Toolbar> vals)
{
  for (auto& val : vals)
  {
    insert(std::move(val));
  }
}

}
