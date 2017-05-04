#pragma once

#include <QString>
#include <iscore/plugins/customfactory/FactoryInterface.hpp>
#include <iscore_lib_inspector_export.h>

namespace iscore
{
struct DocumentContext;
}

namespace Inspector
{
class InspectorWidgetBase;
/**
     * @brief The InspectorWidgetFactoryInterface class
     *
     * This class has to be registered in the inspector for each plug-in that
     * provides it.
     *
     * When an object in the Document is selected, the pointer to the object is
 * sent
     * to the inspector using the signal-slot mechanism.
     * The factory can then make a widget from the QObject, which can be
 * displayed
     * in the inspector.
     *
     */
class ISCORE_LIB_INSPECTOR_EXPORT InspectorWidgetFactory
    : public iscore::Interface<InspectorWidgetFactory>
{
  ISCORE_INTERFACE("e7098592-7e2f-400d-9b71-2628d7276f99")
public:
  virtual ~InspectorWidgetFactory();

  /**
  * @brief makeWidget Makes a widget for the inspector from an object
  * @param sourceElement Element from which an inspector widget is to be made
  * @return An inspector widget corresponding to the object.
  */
  virtual InspectorWidgetBase* makeWidget(
      const QList<const QObject*>& sourceElement,
      const iscore::DocumentContext& doc,
      QWidget* parent) const = 0;

  virtual bool matches(const QList<const QObject*>& objects) const = 0;
};
}
