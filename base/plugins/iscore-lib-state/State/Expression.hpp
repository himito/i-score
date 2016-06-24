#pragma once
#include <State/Relation.hpp>
#include <iscore/tools/std/Optional.hpp>
#include <eggs/variant/variant.hpp>

#include <iscore/tools/VariantBasedNode.hpp>
#include <QString>
#include <algorithm>
#include <cstddef>
#include <vector>

#include <iscore/tools/InvisibleRootNode.hpp>

class DataStream;
class JSONObject;
template <typename DataType> class TreeNode;

namespace State
{
enum class BinaryOperator {
    And, Or, Xor, None
};
enum class UnaryOperator {
    Not, None
};

struct ISCORE_LIB_STATE_EXPORT ExprData :
        public iscore::VariantBasedNode<Relation, Pulse, BinaryOperator, UnaryOperator>
{
        ISCORE_SERIALIZE_FRIENDS(ExprData, DataStream)
        ISCORE_SERIALIZE_FRIENDS(ExprData, JSONObject)

        ExprData() = default;
        template<typename T>
        ExprData(T data):
            VariantBasedNode{std::move(data)}
        {

        }

        friend bool operator==(const ExprData& lhs, const ExprData& rhs)
        {
            return lhs.m_data == rhs.m_data;
        }

        QString toString() const;
};

}

/**
 * @brief The TreeNode<State::ExprData> class
 *
 * This class is specialized from TreeNode<T>
 * because we want to have an additional check :
 * a node is a leaf iff a node is a State::Relation
 *
 * TODO enforce the invariant of children.size <= 2 (since it's a binary tree)
 */
template<>
class ISCORE_LIB_STATE_EXPORT TreeNode<State::ExprData> final : public State::ExprData
{
        friend struct TSerializer<DataStream, void, TreeNode<State::ExprData>>;
        friend struct TSerializer<JSONObject, void, TreeNode<State::ExprData>>;

        friend bool operator!=(const TreeNode<State::ExprData>& lhs, const TreeNode<State::ExprData>& rhs)
        {
            return !(lhs == rhs);
        }

        friend bool operator==(const TreeNode<State::ExprData>& lhs, const TreeNode<State::ExprData>& rhs)
        {
            const auto& ltd = static_cast<const State::ExprData&>(lhs);
            const auto& rtd = static_cast<const State::ExprData&>(rhs);

            bool b = (ltd == rtd) && (lhs.m_children.size() == rhs.m_children.size());
            if(!b)
                return false;

            for(std::size_t i = 0; i < lhs.m_children.size(); i++)
            {
                if(lhs.m_children[i] != rhs.m_children[i])
                    return false;
            }

            return true;
        }

    public:
        QString toString() const;

        using iterator = typename std::vector<TreeNode>::iterator;
        using const_iterator = typename std::vector<TreeNode>::const_iterator;

        auto begin() { return m_children.begin(); }
        auto begin() const { return m_children.cbegin(); }
        auto cbegin() const { return m_children.cbegin(); }
        auto& front() { return m_children.front(); }

        auto end() { return m_children.end(); }
        auto end() const { return m_children.cend(); }
        auto cend() const { return m_children.cend(); }
        auto& back() { return m_children.back(); }

        TreeNode() = default;

        // The parent has to be set afterwards.
        TreeNode(const TreeNode& other):
            State::ExprData{static_cast<const State::ExprData&>(other)},
            m_children(other.m_children)
        {
            setParent(other.m_parent);
            for(auto& child : m_children)
                child.setParent(this);
        }

        TreeNode(TreeNode&& other):
            State::ExprData{std::move(static_cast<State::ExprData&&>(std::move(other)))},
            m_children(std::move(other.m_children))
        {
            setParent(other.m_parent);
            for(auto& child : m_children)
                child.setParent(this);
        }

        TreeNode& operator=(const TreeNode& source)
        {
            static_cast<State::ExprData&>(*this) = static_cast<const State::ExprData&>(source);
            setParent(source.m_parent);

            m_children = source.m_children;
            for(auto& child : m_children)
            {
                child.setParent(this);
            }

            return *this;
        }

        TreeNode& operator=(TreeNode&& source)
        {
            static_cast<State::ExprData&>(*this) = static_cast<State::ExprData&&>(source);
            setParent(source.m_parent);

            m_children = std::move(source.m_children);
            for(auto& child : m_children)
            {
                child.setParent(this);
            }

            return *this;
        }

        TreeNode(State::ExprData data, TreeNode* parent):
            State::ExprData(std::move(data))
        {
            setParent(parent);
        }

        // Clone
        explicit TreeNode(
                TreeNode source,
                TreeNode* parent):
            TreeNode{std::move(source)}
        {
            setParent(parent);
        }

        void push_back(const TreeNode& child)
        {
            m_children.push_back(child);

            auto& cld = m_children.back();
            cld.setParent(this);
        }

        void push_back(TreeNode&& child)
        {
            m_children.push_back(std::move(child));

            auto& cld = m_children.back();
            cld.setParent(this);
        }

        // OPTIMIZEME : the last arg will be this. Is it possible to optimize that ?
        template<typename... Args>
        auto& emplace_back(Args&&... args)
        {
            m_children.emplace_back(std::forward<Args>(args)...);

            auto& cld = m_children.back();
            cld.setParent(this);
            return cld;
        }

        template<typename... Args>
        auto& emplace(Args&&... args)
        {
            auto& n = *m_children.emplace(std::forward<Args>(args)...);
            n.setParent(this);
            return n;
        }

        TreeNode* parent() const
        {
            return m_parent;
        }

        bool hasChild(std::size_t index) const
        {
            return m_children.size() > index;
        }

        TreeNode& childAt(int index)
        {
            ISCORE_ASSERT(hasChild(index));
            return m_children.at(index);
        }

        const TreeNode& childAt(int index) const
        {
            ISCORE_ASSERT(hasChild(index));
            return m_children.at(index);
        }

        // returns -1 if not found
        int indexOfChild(const TreeNode* child) const
        {
            for(std::size_t i = 0U; i < m_children.size(); i++)
                if(child == &m_children[i])
                    return i;

            return -1;
        }

        int childCount() const
        { return m_children.size(); }

        bool hasChildren() const
        { return ! m_children.empty(); }

        auto& children()
        { return m_children;  }
        const auto& children() const
        { return m_children;  }

        // Won't delete the child!
        void removeChild(const_iterator it)
        {
            m_children.erase(it);
        }

        void setParent(TreeNode* parent)
        {
            ISCORE_ASSERT(
                        !m_parent ||
                        (m_parent && !m_parent->is<State::Relation>()) ||
                        (m_parent && !m_parent->is<State::Pulse>())
                        );
            m_parent = parent;
        }

    protected:
        TreeNode<State::ExprData>* m_parent {};
        std::vector<TreeNode> m_children;
};

namespace State
{
using Expression = TreeNode<ExprData>;
using Condition = Expression;
using Trigger = Expression;

ISCORE_LIB_STATE_EXPORT optional<State::Expression> parseExpression(const QString& str);
ISCORE_LIB_STATE_EXPORT optional<State::Value> parseValue(const QString& str);
ISCORE_LIB_STATE_EXPORT optional<State::Address> parseAddress(const QString& str);
ISCORE_LIB_STATE_EXPORT optional<State::AddressAccessor> parseAddressAccessor(const QString& str);
}

JSON_METADATA(State::Address, "Address")
JSON_METADATA(State::AddressAccessor, "AddressAccessor")
JSON_METADATA(State::Value, "Value")
JSON_METADATA(State::Relation, "Relation")
JSON_METADATA(State::Pulse, "Pulse")
JSON_METADATA(State::UnaryOperator, "UnOp")
JSON_METADATA(State::BinaryOperator, "BinOp")
