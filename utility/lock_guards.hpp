#ifndef LOCK_GUARDS_HPP
#define LOCK_GUARDS_HPP

#include <mutex>
#include <tuple>
#include <memory>
#include "std14.hpp"

namespace std_ext
{
   /**
 * @brief The lock_guards class
 * lock_guards is a mutexes wrapper that provides a convenient RAII-style mechanism for owning a mutexes for the duration of a scoped block.
 * lock_guards use a deadlock avoidance algorithm to avoid deadlock.
 * Example:

   std::mutex m1;
   std::recursive_mutex m2;

   void do_work()
   {
      lock_guards guards {m1, m2};
      //do rest of work
   }
 */

   namespace details
   {
      template <typename T>
      class unlock_guard
      {
         T& _mutex;
      public:
         unlock_guard(T& mutex) noexcept
         : _mutex(mutex)
         {   }
         unlock_guard(unlock_guard const& other)
         : _mutex(other._mutex)
         {   }
         ~unlock_guard()
         {
            _mutex.unlock();
         }
      };

      class _lock_guards
      {
      public:
         virtual ~_lock_guards() = default;
      };

      template <typename ...Args>
      class lock_guards : public _lock_guards
      {
         std::tuple<unlock_guard<Args>...> _guards;
      public:
         lock_guards(Args&... args) : _guards(std::tuple<unlock_guard<Args>...>(args...))
         {
            std::lock(args...);
         }
      };

      template <>
      class lock_guards<void>
      {
         std::unique_ptr<_lock_guards> _guards;
      public:
         template <typename ...Args>
         lock_guards(Args&... args)
         {
            _guards = std14::make_unique<lock_guards<Args...>>(args...);
         }
      };
   }

   using lock_guards = details::lock_guards<void>;
}

#endif // LOCK_GUARDS_HPP
