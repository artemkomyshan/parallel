/*
* Copyright (c) 2018 Parallel
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*/
/*
* Author: Artem Komyshan
*/

#pragma once

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


   template< class T >
   using remove_extent_t = typename std::remove_extent<T>::type;

   /// make_unique ///
   namespace details
   {
      template<typename T>
      struct MakeUniq
      {
        typedef std::unique_ptr<T> single_object;
      };

      template<typename T>
      struct MakeUniq<T[]>
      {
         typedef std::unique_ptr<T[]> array;
      };

      template<typename T, size_t _Bound>
      struct MakeUniq<T[_Bound]>
      {
         struct invalid_type { };
      };
   }

   /**
    * Constructs an object of type T and wraps it in a std::unique_ptr.
    * The arguments args are passed to the constructor of T.
    * @param args - list of arguments with which an instance of T will be constructed.
    */
   template<typename T, typename... Args>
   inline typename details::MakeUniq<T>::single_object make_unique(Args&&... _Args)
   {
      return std::unique_ptr<T>(new T(std::forward<Args>(_Args)...));
   }

   /**
    * Constructs an array of unknown bound T.
    * This overload only participates in overload resolution if T is an array of unknown bound.
    * @param size	- the size of the array to construct.
    */
   template<typename T>
   inline typename details::MakeUniq<T>::array make_unique(size_t __num)
   {
      return std::unique_ptr<T>(new remove_extent_t<T>[__num]());
   }

   /**
    *  Construction of arrays of known bound is disallowed.
    */
   template<typename T, typename... Args>
   inline typename details::MakeUniq<T>::invalid_type make_unique(Args&&...) = delete;

   }

