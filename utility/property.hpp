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
* 2. default initialization shouldn't initialize _value;
* 3. implement pointer-like interface;
 */

template <typename T>
class property
{
   T     _value {};
   bool  _valid {false};

public :
   property() noexcept( noexcept( T() ) ) = default;

   template < typename U >
   property( U&& value )
   : _value( std::forward<U>( value ) ), _valid( true )
   {   }

   template < typename U >
   property& operator=( U&& value )
   {
      _value = std::forward<U>( value );
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
      return _value;
   }

   T&& value() &&
   {
      check();
      _valid = false;
      return std::move( _value );
   }

   T& get_writable() noexcept
   {
      _valid = true;
      return _value;
   }

   void invalidate() noexcept
   {
      _valid = false;
   }

   T&& release()
   {
      check();
      _valid = false;
      return std::move( _value );
   }

   template < typename U >
   T  value_or( U && v ) const &
   {
      if ( is_valid() )
         return _value;
      else
         return std::forward<U>( v );
   }

   template < typename U >
   T value_or( U&& v ) &&
   {
      _valid = false;

      if ( is_valid() )
         return std::move( _value );
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
void swap( property< T >& l,   property< T >& r)
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
