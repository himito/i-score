#pragma once
#include <QObject>
#include <QShortcut>
#include <iscore/document/DocumentContext.hpp>
#include <iscore/tools/std/Optional.hpp>
#include <iscore_lib_base_export.h>

#include <boost/optional.hpp>
class Selection;
namespace iscore
{
struct GUIApplicationContext;
struct DocumentContext;
class PanelModel;
class PanelView;

/**
 * @brief The PanelStatus struct
 *
 * Some metadata for the panels.
 *
 */
struct ISCORE_LIB_BASE_EXPORT PanelStatus
{
  PanelStatus(
      bool isShown,
      Qt::DockWidgetArea d,
      int prio,
      QString name,
      const QKeySequence& sc);

  const bool shown;              // Controls if it is shown by default.
  const Qt::DockWidgetArea dock; // Which dock.
  const int priority;            // Higher priority will come up first.
  const QString prettyName;      // Used in the header.
  const QKeySequence shortcut; // Keyboard shortcut to show or hide the panel.
};

/**
 * @brief The PanelDelegate class
 *
 * Base class for the panels on the sides of i-score.
 * A panel is something that may outlive a document.
 * When the document changes, all the panels are updated
 * with the new visible document.
 *
 *
 * \see \ref PanelStatus
 */
class ISCORE_LIB_BASE_EXPORT PanelDelegate
{
public:
  PanelDelegate(const iscore::GUIApplicationContext& ctx);
  virtual ~PanelDelegate();

  const iscore::GUIApplicationContext& context() const;

  void setModel(const iscore::DocumentContext& model);
  void setModel(none_t n);

  /**
   * @brief document The optional current document
   * @return The document if there is a current document in i-score, else
   * nothing.
   */
  MaybeDocument document() const;

  /**
   * @brief widget The widget of the panel.
   * @note The \ref View class takes ownership of it.
   */
  virtual QWidget* widget() = 0;

  /**
   * @brief defaultPanelStatus Metadata of the panel.
   */
  virtual const PanelStatus& defaultPanelStatus() const = 0;

  /**
   * @brief setNewSelection This function will be called if the selected
   * objects
   * change in i-score
   *
   * @param s The new selection.
   */
  virtual void setNewSelection(const Selection& s);

protected:
  /**
   * @brief on_modelChanged This function is called when the visible
   * document changes.
   * @param oldm The previous (actual) document or nothing if there was none.
   * @param newm The new document (or nothing if the user closed everything).
   */
  virtual void on_modelChanged(MaybeDocument oldm, MaybeDocument newm);

private:
  const iscore::GUIApplicationContext& m_context;
  MaybeDocument m_model;
};
}
