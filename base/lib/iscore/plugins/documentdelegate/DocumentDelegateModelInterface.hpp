#pragma once
#include <iscore/tools/IdentifiedObject.hpp>

#include <iscore/selection/Selection.hpp>

struct VisitorVariant;

namespace iscore
{
    class ISCORE_LIB_BASE_EXPORT DocumentDelegateModelInterface :
        public IdentifiedObject<DocumentDelegateModelInterface>
    {
            Q_OBJECT
        public:
            using IdentifiedObject<DocumentDelegateModelInterface>::IdentifiedObject;
            virtual ~DocumentDelegateModelInterface();

            virtual void serialize(const VisitorVariant&) const = 0;

        public slots:
            virtual void setNewSelection(const Selection& s) = 0;
    };
}
