/*
* Copyright (c) 2018 Artem Komyshan
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

#include <stdexcept>
#include <utility>
#include <assert.h>

template<typename From, typename To>
using allowIfConvertible = std::enable_if_t<std::is_convertible<From, To>::value>;

struct except_on_fail{};

template <typename A>
void action_on_fail();

struct bad_property_access : public std::logic_error
{
    explicit bad_property_access(char const* const message) : std::logic_error(message) {}
};

template <>
inline void action_on_fail<except_on_fail>()
{
   throw bad_property_access("parallel: failure, cause - bad_property_access");
};


struct assert_on_fail{};

template <>
inline void action_on_fail<assert_on_fail>()
{
    assert( false );
};

template <typename T, typename A = except_on_fail>
class property
{
   bool  _valid;

   union storage
   {
      T _value;

      storage () noexcept {}
      ~storage() noexcept {}
   } _storage;

public :
   property() noexcept : _valid( false )
   { }

   ~property()
   {
      clear();
   }

   template <typename OA>
   property( property<T, OA> const& rhs)
   {
      if ( rhs.is_valid() )
      {
          new ( &_storage._value ) T ( rhs.value() );
         _valid = true;
      }
   }

   template <typename OA>
   property( property<T, OA>&& rhs)
   {
      if ( rhs.is_valid() )
      {
         new ( &_storage._value ) T ( std::move( rhs ).value() );
         _valid = true;
      }
   }

   template < typename U, typename = allowIfConvertible<U, T> >
   property( U&& value )
   :  _valid( true ), _storage()
   {
      new ( &_storage._value ) T ( std::forward<U>( value ) );
   }

   template <typename OA>
   property& operator=( property<T, OA> const& rhs )
   {

      if ( is_valid() && !rhs.is_valid() )
         clear();
      else if ( rhs.is_valid() )
      {
         if ( !is_valid() )
         {
            new ( &_storage._value ) T ( rhs.value() );
            _valid = true;
         }
         else
            _storage._value = rhs.value();
      }

      return *this;
   }

   template <typename OA>
   property& operator=( property<T, OA>&& rhs )
   {
      if ( is_valid() && !rhs.is_valid() )
         clear();
      else if ( rhs.is_valid() )
      {
         if ( !is_valid() )
         {
            new ( &_storage._value ) T ( std::forward<property<T, OA>>( rhs ).value() );
            _valid = true;
         }
         else
            _storage._value = std::forward<property<T, OA>>( rhs ).value();
      }

      return *this;
   }

   property& operator=( property const& rhs )
   {
      return operator=<A>(rhs);
   }

   template < typename U, typename = allowIfConvertible<U, T> >
   property& operator=( U&& value )
   {
     new ( &_storage._value ) T ( std::forward<U>( value ) );
      _valid = true;
      return *this;
   }

   bool is_valid() const noexcept
   {
      return _valid;
   }

   explicit operator bool() const noexcept
   {
      return is_valid();
   }

   T const& value() const &
   {
      check();
      return _storage._value;
   }

   T&& value() &&
   {
      check();
      _valid = false;
      return std::move( _storage._value );
   }

   T& get_writable() noexcept
   {
      if ( is_valid() )
      {
         new ( &_storage._value ) T ();
         _valid = true;
      }
      return _storage._value;
   }

   void invalidate() noexcept
   {
      clear();
   }

   T&& release()
   {
      check();
      _valid = false;
      return std::move( _storage._value );
   }

   template < typename U, typename = allowIfConvertible<U, T> >
   T  value_or( U && v ) const &
   {
      if ( is_valid() )
         return _storage._value;
      else
         return std::forward<U>( v );
   }

   template < typename U, typename = allowIfConvertible<U, T> >
   T value_or( U&& v ) &&
   {
      if ( is_valid() )
      {
         _valid = false;
         return std::move( _storage._value );
      }
      else
         return std::forward<U>( v );
   }

private:

   void clear()
   {
      if ( is_valid() )
      {
         _storage._value.T::~T();
         _valid = false;
      }
   }

   void check() const
   {
      if ( !is_valid() )
         action_on_fail<A>();
   }
};

template <typename T, typename A, typename OA>
void swap( property< T, A >& l,  property< T, OA >& r)
{
   const bool hasL = l.is_valid();
   const bool hasR = r.is_valid();

   if ( !hasL && !hasR )
      return;

   std::swap( l.get_writable(), r.get_writable() );

   if( !hasL )
       l.invalidate() ;
   else if( !hasR )
       r.invalidate() ;
}

template <typename T, typename A, typename OA>
bool operator==( property<T, A> const& x, property<T, OA> const& y )
{
  return bool( x ) != bool( y ) ? false : bool ( x ) == false ? true : x.value() == y.value();
}

template <typename T, typename A, typename OA>
bool operator!=( property<T, A> const& x, property<T, OA> const& y )
{
  return !( x == y );
}

template <typename T, typename A, typename OA>
bool operator<( property<T, A> const& x, property<T, OA> const& y )
{
  return ( !y ) ? false : ( !x ) ? true : x.value() < y.value();
}

template <typename T, typename A, typename OA>
bool operator>( property<T, A> const& x, property<T, OA> const& y )
{
  return ( y < x );
}

template <typename T, typename A, typename OA>
bool operator<= ( const property<T, A>& x, property<T, OA> const& y )
{
  return !( y < x );
}

template <typename T, typename A, typename OA>
bool operator>=( property<T, A> const& x, property<T, OA> const& y )
{
  return !( x < y );
}


template <typename T, typename A>
bool operator==( const property<T, A>& x, T const& v )
{
  return bool( x ) ? x.value() == v : false;
}

template <typename T, typename A>
bool operator==( T const& v, property<T, A> const& x )
{
  return bool( x ) ? v == x.value() : false;
}

template <typename T, typename A>
bool operator!=( const property<T, A>& x, T const& v )
{
  return bool( x ) ? x.value() != v : true;
}

template <typename T, typename A>
bool operator!=( T const& v, property<T, A> const& x )
{
  return bool( x ) ? v != x.value() : true;
}

template <typename T, typename A>
bool operator<( const property<T, A>& x, T const& v )
{
  return bool( x ) ? x.value() < v : true;
}

template <typename T, typename A>
bool operator>( T const& v, property<T, A> const& x )
{
  return bool( x ) ? v > x.value() : true;
}

template <typename T, typename A>
bool operator>( const property<T, A>& x, T const& v )
{
  return bool( x ) ? x.value() > v : false;
}

template <typename T, typename A>
bool operator<( T const& v, property<T, A> const& x )
{
  return bool( x ) ? v < x.value() : false;
}

template <typename T, typename A>
bool operator>=( const property<T, A>& x, T const& v )
{
  return bool( x ) ? x.value() >= v : false;
}

template <typename T, typename A>
bool operator<=( T const& v, property<T, A> const& x )
{
  return bool( x ) ? v <= x.value() : false;
}

template <typename T, typename A>
bool operator<=( const property<T, A>& x, T const& v )
{
  return bool( x ) ? x.value() <= v : true;
}

template <typename T, typename A>
bool operator>=( T const& v, property<T, A> const& x )
{
  return bool( x ) ? v >= x.value() : true;
}
