#ifndef LOCK_GUARDS_HPP
#define LOCK_GUARDS_HPP

#include <mutex>
#include <tuple>

namespace parallel {
namespace raii {
   /**
 * @brief The lock_guards class
 * lock_guards is a mutexes wrapper that provides a convenient RAII-style mechanism for owning a mutexes for the duration of a scoped block.
 * lock_guards use a deadlock avoidance algorithm to avoid deadlock.
 * Example:

   std::mutex m1;
   std::recursive_mutex m2;

   void do_work()
   {
      auto guards = std_ext::make_locks(m1, m2);
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
#endif // LOCK_GUARDS_HPP
