#ifndef STD14_HPP
#define STD14_HPP

#include <cstddef>
#include <memory>

namespace std14
   {
   template<typename T, T... Ints>
   struct integer_sequence
   {
      using value_type = T;
      static constexpr std::size_t size() { return sizeof...(Ints); }
   };

   template<std::size_t... Ints>
   using index_sequence = integer_sequence<std::size_t, Ints...>;

   template<typename T, std::size_t N, T... Is>
   struct make_integer_sequence : make_integer_sequence<T, N-1, N-1, Is...> {};

   template<typename T, T... Is>
   struct make_integer_sequence<T, 0, Is...> : integer_sequence<T, Is...> {};

   template<std::size_t N>
   using make_index_sequence = make_integer_sequence<std::size_t, N>;

   template<typename... T>
   using index_sequence_for = make_index_sequence<sizeof...(T)>;


   template <typename T, typename ...Args>
   std::unique_ptr<T> make_unique(Args&&... args)
   {
      return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
   }

   }

#endif // STD14_HPP
