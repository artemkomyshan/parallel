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

#include <utility>
#include "assert.hpp"

/**
* TODO:
* 1. implementation of check should depend on template parameter;
* 3. implement pointer-like issnterface;
 */

template<typename From, typename To>
using allowIfConvertible = std::enable_if_t<std::is_convertible<From, To>::value>;

template <typename T>
class property
{
   bool  _valid;

   union storage
   {
      T _value;

      storage () noexcept {}
      ~storage() noexcept {}
   } _storage;


   void clear()
   {
      if ( _valid )
      {
         _storage._value.T::~T();
         _valid = false;
      }
   }

public :
   property() noexcept : _valid( false )
   { }

   ~property()
   {
      clear();
   }

   property( property const& rhs)
   {
      if ( rhs.is_valid() )
      {
          new (&_storage._value) T ( rhs.value() );
         _valid = true;
      }
   }

   property( property&& rhs)
   {
      if ( rhs.is_valid() )
      {
         new (&_storage._value) T ( std::move( rhs ).value() );
         _valid = true;
      }
   }

   template < typename U, typename = allowIfConvertible<U, T> >
   property( U&& value )
   :  _valid( true )
   {
      new (&_storage._value) T ( std::forward<U>( value ) );
   }

   property& operator=( property const& rhs )
   {
      if ( is_valid() && !rhs.is_valid() )
         clear();
      else if ( rhs.is_valid() )
      {
         if ( !is_valid() )
            _valid = true;

         _storage._value = rhs.value();
      }

      return *this;
   }
   property& operator=( property&& rhs )
   {
      if ( is_valid() && !rhs.is_valid() )
         clear();
      else if ( rhs.is_valid() )
      {
         if ( !is_valid() )
            _valid = true;

         _storage._value = std::move( rhs ).value();
      }

      return *this;
   }

   template < typename U, typename = allowIfConvertible<U, T> >
   property& operator=( U&& value )
   {
     new (&_storage._value) T ( std::forward<U>( value ) );
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
      if ( _valid )
      {
         new (&_storage._value) T ();
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
      _valid = false;

      if ( is_valid() )
         return std::move( _storage._value );
      else
         return std::forward<U>( v );
   }

private:
   void check() const
   {
      Ensures( _valid );
   }
};

template <typename T>
void swap( property< T >& l,  property< T >& r)
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

template <typename T>
bool operator==( const property<T>& x, property<T> const& y )
{
  return bool( x ) != bool( y ) ? false : bool ( x ) == false ? true : x.value() == y.value();
}

template <typename T>
bool operator!=( property<T> const& x, property<T> const& y )
{
  return !( x == y );
}

template <typename T>
bool operator<( property<T> const& x, property<T> const& y )
{
  return ( !y ) ? false : ( !x ) ? true : x.value() < y.value();
}

template <typename T>
bool operator>( property<T> const& x, property<T> const& y )
{
  return ( y < x );
}

template <typename T>
bool operator<= ( const property<T>& x, property<T> const& y )
{
  return !( y < x );
}

template <typename T>
bool operator>=( property<T> const& x, property<T> const& y )
{
  return !( x < y );
}


template <typename T>
bool operator==( const property<T>& x, T const& v )
{
  return bool( x ) ? x.value() == v : false;
}

template <typename T>
bool operator==( T const& v, property<T> const& x )
{
  return bool( x ) ? v == x.value() : false;
}

template <typename T>
bool operator!=( const property<T>& x, T const& v )
{
  return bool( x ) ? x.value() != v : true;
}

template <typename T>
bool operator!=( T const& v, property<T> const& x )
{
  return bool( x ) ? v != x.value() : true;
}

template <typename T>
bool operator<( const property<T>& x, T const& v )
{
  return bool( x ) ? x.value() < v : true;
}

template <typename T>
bool operator>( T const& v, property<T> const& x )
{
  return bool( x ) ? v > x.value() : true;
}

template <typename T>
bool operator>( const property<T>& x, T const& v )
{
  return bool( x ) ? x.value() > v : false;
}

template <typename T>
bool operator<( T const& v, property<T> const& x )
{
  return bool( x ) ? v < x.value() : false;
}

template <typename T>
bool operator>=( const property<T>& x, T const& v )
{
  return bool( x ) ? x.value() >= v : false;
}

template <typename T>
bool operator<=( T const& v, property<T> const& x )
{
  return bool( x ) ? v <= x.value() : false;
}

template <typename T>
bool operator<=( const property<T>& x, T const& v )
{
  return bool( x ) ? x.value() <= v : true;
}

template <typename T>
bool operator>=( T const& v, property<T> const& x )
{
  return bool( x ) ? v >= x.value() : true;
}
