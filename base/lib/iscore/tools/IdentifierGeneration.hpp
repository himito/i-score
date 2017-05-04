#pragma once
#include <ossia/detail/algorithms.hpp>
#include <algorithm>
#include <cstddef>
#include <iscore/model/Identifier.hpp>
#include <iscore/tools/std/ArrayView.hpp>
#include <iscore/tools/std/Optional.hpp>
#include <iterator>
#include <sys/types.h>
#include <type_traits>
#include <vector>

namespace iscore
{
/**
 * @brief Generates random identifiers for new objects.
 */
struct ISCORE_LIB_BASE_EXPORT random_id_generator
{
  /**
   * @brief getNextId
   * @return a random int32
   */
  static int32_t getRandomId();
  static int32_t getFirstId()
  {
    return getRandomId();
  }

  /**
   * @brief getNextId
   * @param ids A vector of ids
   *
   * @return A new id not in the vector.
   */
  template <typename Vector>
  static auto getNextId(const Vector& ids)
  {
    typename Vector::value_type id{};

    auto end = std::end(ids);
    do
    {
      id = typename Vector::value_type{getRandomId()};
    } while (ossia::find(ids, id) != end);

    return id;
  }
};

/**
 * @brief Generates identifiers for new objects, starting from 1.
 */
struct ISCORE_LIB_BASE_EXPORT linear_id_generator
{
  static int32_t getFirstId()
  {
    return 1;
  }

  template <typename Vector>
  static auto getNextId(const Vector& ids)
  {
    auto it = std::max_element(ids.begin(), ids.end());
    if (it != ids.end())
      return typename Vector::value_type{getId(*it) + 1};
    else
      return typename Vector::value_type{getFirstId()};
  }

private:
  template <typename T>
  static int32_t getId(const Id<T>& other)
  {
    return other.val();
  }
  static int32_t getId(const optional<int32_t>& i)
  {
    return *i;
  }
  static int32_t getId(int32_t i)
  {
    return i;
  }
};

using id_generator = iscore::linear_id_generator;
}
template <typename T>
auto getStrongId(const std::vector<Id<T>>& v)
{
  return Id<T>{iscore::id_generator::getNextId(v)};
}

template <typename T>
auto getStrongId(const iscore::dynvector_impl<Id<T>>& v)
{
  return Id<T>{iscore::id_generator::getNextId(v)};
}

template <
    typename Container, std::enable_if_t<std::is_pointer<
                            typename Container::value_type>::value>* = nullptr>
auto getStrongId(const Container& v)
    -> Id<typename std::remove_pointer<typename Container::value_type>::type>
{
  using namespace std;
  using local_id_t
      = Id<typename std::remove_pointer<typename Container::value_type>::type>;
  vector<int32_t> ids(v.size()); // Map reduce

  transform(
      v.begin(),
      v.end(),
      ids.begin(),
      [](const typename Container::value_type& elt) {
        return elt->id().val();
      });

  return local_id_t{iscore::id_generator::getNextId(ids)};
}

template <
    typename Container, std::enable_if_t<!std::is_pointer<
                            typename Container::value_type>::value>* = nullptr>
auto getStrongId(const Container& v) -> Id<typename Container::value_type>
{
  using namespace std;
  auto ids = make_dynarray(int32_t, v.size());

  transform(v.begin(), v.end(), ids.begin(), [](const auto& elt) {
    return elt.id().val();
  });

  return Id<typename Container::value_type>{
      iscore::id_generator::getNextId(ids)};
}

template <typename T>
auto getStrongIdRange(std::size_t s)
{
  std::vector<Id<T>> vec;
  vec.reserve(s);
  vec.emplace_back(iscore::id_generator::getFirstId());

  s--;
  for (; s-- > 0;)
  {
    vec.push_back(getStrongId(vec));
  }

  return vec;
}
template <typename T, typename Vector>
auto getStrongIdRange(std::size_t s, const Vector& existing)
{
  auto existing_size = existing.size();
  auto total_size = existing_size + s;
  std::vector<Id<T>> vec;
  vec.reserve(total_size);

  // Copy the existing ids
  std::transform(
      existing.begin(), existing.end(), std::back_inserter(vec),
      [](const auto& elt) { return elt.id(); });

  // Then generate the new ones
  for (; s-- > 0;)
  {
    vec.push_back(getStrongId(vec));
  }

  return std::vector<Id<T>>(vec.begin() + existing.size(), vec.end());
}
