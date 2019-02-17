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

#include <mutex>
#include <tuple>

namespace parallel {
namespace raii {
   /**
 * @brief The lock_guards class
 * multi_lock is a mutexes wrapper that provides a convenient RAII-style mechanism for owning a few mutexes for the duration of a scoped block.
 * multi_lock use a deadlock avoidance algorithm to avoid deadlock.
 * Example:

  // Code with multi_lock
   std::mutex m1;
   std::recursive_mutex m2;

   void do_work()
   {
      auto guards = make_locks(m1, m2);
      //do rest of work
   }
/////////////////////////////////////////////////////////////////////////

   // Code without multi_lock
   std::mutex m1;
   std::recursive_mutex m2;

   void do_work()
   {
      std::lock(m1, m2);
      std::lock_guard<std::mutex> lk1(m1, std::adopt_lock);
      std::lock_guard<std::recursive_mutex> lk2(m2, std::adopt_lock);
      //do rest of work
   }
 */

template <typename T>
class unlock_guard
{
   T& _mutex;
public:
   unlock_guard(T& mutex) noexcept
   : _mutex(mutex)
   {   }
   ~unlock_guard()
   {
      _mutex.unlock();
   }
};

template <typename ...Args>
class multi_lock
{
   std::tuple<unlock_guard<Args>...> _guards;
public:
   multi_lock(Args&... args) : _guards(std::tuple<unlock_guard<Args>...>(args...))
   {
      std::lock(args...);
   }
};

template <typename ...Args>
multi_lock<Args...> make_locks(Args&... args)
{
   return multi_lock<Args...>(args...);
}

}
}

