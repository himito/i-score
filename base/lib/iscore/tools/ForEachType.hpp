#pragma once
/**
 * \file ForEachType
 * \brief Algorithms to iterate on a type list.
 *
 * There are two variants :
 *
 * `for_each_type` and `for_each_type_if`.
 *
 * Example:
 *
 * \code
 * struct foo {
 *  void perform(int) { ... }
 *  void perform(float) { ... }
 *  void perform(MyType) { ... }
 * };
 * for_each_type<TL<int, float, MyType>>(foo{});
 * for_each_type<TL<int, float, MyType>>(foo{});
 * \endcode
 *
 * Note : the brigand library seems to have a better implementation of this.
 */

// Courtesy of Daniel J-H
// https://gist.github.com/daniel-j-h
template <typename... Args>
struct TypeList
{
  static const constexpr auto size = sizeof...(Args);
};
namespace iscore
{
namespace detail
{

// Forward declare, in order to pattern match via specialization
template <typename...>
struct TypeVisitor;

// Pattern match to extract the TypeLists types into Args
template <template <typename...> class Sequence, typename... Args>
struct TypeVisitor<Sequence<Args...>>
{

  template <typename F>
  static constexpr void visit(F&& f)
  {
    // Start recursive visitation for each type
    do_visit<F, Args...>(std::forward<F>(f));
  }

  // Allow empty sequence
  template <typename F>
  static constexpr void do_visit(F&& f)
  {
  }

  // Base case: one type, invoke functor
  template <typename F, typename Head>
  static constexpr void do_visit(F&& f)
  {
    std::forward<F>(f).template perform<Head>();
  }

  // At least [Head, Next], Tail... can be empty
  template <typename F, typename Head, typename Next, typename... Tail>
  static constexpr void do_visit(F&& f)
  {
    // visit head and recurse visitation on rest
    do_visit<F, Head>(std::forward<F>(f)),
        do_visit<F, Next, Tail...>(std::forward<F>(f));
  }
};
} // End Detail
}
// Invokes the functor with every type, this code generation is done at compile
// time
template <typename Sequence, typename F>
constexpr void for_each_type(F&& f)
{
  iscore::detail::TypeVisitor<Sequence>::template visit<F>(std::forward<F>(f));
}

namespace iscore
{
namespace detail
{

// Forward declare, in order to pattern match via specialization
template <typename...>
struct TypeVisitorIf;

// Pattern match to extract the TypeLists types into Args
template <template <typename...> class Sequence, typename... Args>
struct TypeVisitorIf<Sequence<Args...>>
{

  template <typename F>
  static constexpr void visit_if(const F& f)
  {
    // Start recursive visitation for each type
    do_visit_if<F, Args...>(f);
  }

  // Allow empty sequence
  template <typename F>
  static constexpr bool do_visit_if(const F& f)
  {
    return false;
  }

  // Base case: one type, invoke functor
  template <typename F, typename Head>
  static constexpr bool do_visit_if(const F& f)
  {
    return f.template visit_if<Head>();
  }

  // At least [Head, Next], Tail... can be empty
  template <typename F, typename Head, typename Next, typename... Tail>
  static constexpr bool do_visit_if(const F& f)
  {
    // visit head and recurse visitation on rest
    return do_visit_if<F, Head>(f) ? true : do_visit_if<F, Next, Tail...>(f);
  }

  template <typename F>
  static constexpr void visit_if(F& f)
  {
    // Start recursive visitation for each type
    do_visit_if<F, Args...>(f);
  }

  // Allow empty sequence
  template <typename F>
  static constexpr bool do_visit_if(F& f)
  {
    return false;
  }

  // Base case: one type, invoke functor
  template <typename F, typename Head>
  static constexpr bool do_visit_if(F& f)
  {
    return f.template visit_if<Head>();
  }

  // At least [Head, Next], Tail... can be empty
  template <typename F, typename Head, typename Next, typename... Tail>
  static constexpr void do_visit_if(F& f)
  {
    // visit head and recurse visitation on rest
    return do_visit_if<F, Head>(f) ? true : do_visit_if<F, Next, Tail...>(f);
  }
};
}
} // End Detail
// Invokes the functor with every type, this code generation is done at compile
// time
template <typename Sequence, typename F>
constexpr void for_each_type_if(const F& f)
{
  iscore::detail::TypeVisitorIf<Sequence>::template visit_if<F>(f);
}
template <typename Sequence, typename F>
constexpr void for_each_type_if(F& f)
{
  iscore::detail::TypeVisitorIf<Sequence>::template visit_if<F>(f);
}
