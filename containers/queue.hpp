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

#include <queue>
#include <memory>
#include <mutex>
#include <condition_variable>

namespace parallel {

template <typename T>
class queue
{
   mutable std::mutex _mut;
   std::queue<T> _q;
   std::condition_variable _cond;

public:
   // TODO:
   // Should be movable?
   // Assigned operator
   
   queue() = default;
   queue( queue const& other );
   
   void push( T&&  new_value );

   void wait_and_pop( T& value );
   //change shared_ptr to boost::optional
   std::shared_ptr<T> wait_and_pop();
   
   bool try_pop( T& value );
   //change shared_ptr to boost::optional
   std::shared_ptr<T> try_pop();
   
   bool empty();	
};	

template <typename T>
queue<T>::queue( queue const& other )
{
   std::lock_guard<std::mutex> lk( _mut );   
   _q = other._q;
}

template <typename T>
void queue<T>::push( T&& new_value )
{
   std::lock_guard<std::mutex> lk( _mut );
   _q.emplace( std::forward<T>( new_value ) );
   _cond.notify_one();   
}

template <typename T>
void queue<T>::wait_and_pop( T& value )
{
   std::unique_lock<std::mutex> lk( _mut );
   _cond.wait( lk, [this]{ return !_q.empty(); });
   value = _q.front();
   _q.pop();
}

template <typename T>
std::shared_ptr<T> queue<T>::wait_and_pop()
{
   std::unique_lock<std::mutex> lk( _mut );
   _cond.wait( lk, [this]{ return !_q.empty(); });
   auto res = std::make_shared<T>( _q.front() );
   _q.pop();
   return res;
}

template <typename T>
bool queue<T>::try_pop( T& value )
{
   std::lock_guard<std::mutex> lk( _mut );
   if ( _q.empty() )
      return false;
   
   value = _q.front();
   _q.pop();
   return true;
}

template <typename T>
std::shared_ptr<T> queue<T>::try_pop()
{
   std::lock_guard<std::mutex> lk( _mut );
   if ( _q.empty() )
      return nullptr;

   auto res = std::make_shared<T>( _q.front() );
   _q.pop();
   return res;
}

template <typename T>
bool queue<T>::empty()
{
   std::lock_guard<std::mutex> lk( _mut );
   return _q.empty();
}

}
