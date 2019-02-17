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

#include <atomic>
#include <vector>
#include <functional>
#include <boost/range/irange.hpp>
#include "queue.hpp"
#include "raii/scoped_thread.hpp"

namespace parallel {

class thread_pool
{
   using tTask = std::function<void(void)>;

   std::atomic_bool _done {false};
   std::vector<raii::join_thread> _threds;
   queue<tTask> _work_q;

   void worker_thred()
   {
      while (!_done)
      {
         tTask task;
         if (_work_q.try_pop(task))
         {
            task();
         }
         else
         {
            std::this_thread::yield();
         }
      }
   }

public:
   thread_pool()
   {
      auto thread_counter = std::thread::hardware_concurrency();

      try
      {
         _threds.reserve(thread_counter);
         for (auto i : boost::irange(thread_counter))
            _threds.emplace_back(&thread_pool::worker_thred, this);
      }
      catch(...)
      {
         _done = true;
         throw;
      }
   }

   ~thread_pool()
   {
      _done = true;
   }

   void submit(tTask task)
   {
      _work_q.push(std::move(task));
   }
};

}
