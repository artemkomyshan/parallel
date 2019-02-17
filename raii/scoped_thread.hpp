/*
* Artem Komyshan
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

#include <thread>

/**
 * Example
 * raii:join_thread ( []{ do work.. } );
 */

namespace parallel {
namespace raii {

enum class thread_exec
{
   join,
   detach
};

template <thread_exec d_action> class scoped_thread;

using join_thread = scoped_thread<thread_exec::join>;
using detach_thread = scoped_thread<thread_exec::detach>;

template <thread_exec>
class scoped_thread
{
   std::thread _t;

public:
   template <typename Func, typename ...Args>
   scoped_thread( Func&& f, Args... args )
   : _t( std::forward<Func>( f ), std::forward<Args>( args)... )
   {   }

   ~scoped_thread()
   {
      if ( _t.joinable() )
         join_or_detach();
   }

   scoped_thread( scoped_thread&& ) = default;
   scoped_thread& operator=( scoped_thread&& ) = default;

   std::thread& get() { return _t; }

private:
   void join_or_detach();
};

template<>
void scoped_thread<thread_exec::join>::join_or_detach()
{
   _t.join();
}

template<>
void scoped_thread<thread_exec::detach>::join_or_detach()
{
   _t.detach();
}

}
}
