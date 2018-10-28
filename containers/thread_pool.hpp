#ifndef THREAD_POOL_HPP
#define THREAD_POOL_HPP

#include <atomic>
#include <vector>
#include <functional>
#include "queue.hpp"
#include "utility/thread_raii.hpp"

namespace parallel {

class thread_pool
{
   using tTask = std::function<void(void)>;

   std::atomic_bool _done {false};
   std::vector<thread_raii> _threds;
   queue<tTask> _work_q;

   void worker_thred()
   {
      while (!_done)
      {
         tTask task;
         _work_q.wait_and_pop(task);
         task();
      }
   }

public:
   thread_pool()
   {
      auto thread_counter = std::thread::hardware_concurrency();

      try
      {
         _threds.reserve(thread_counter);
         for (size_t i = 0; i < thread_counter; ++i)
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
#endif // THREAD_POOL_HPP
