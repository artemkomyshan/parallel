#ifndef THREAD_RAII_HPP
#define THREAD_RAII_HPP

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

   template <thread_exec d_action>
   class scoped_thread
   {
      std::thread _t;

   public:
      template <typename Func, typename ...Args>
      scoped_thread( Func&& f, Args... args )
      : _t( std::forward( f ), std::forward<Args>( args)... )
      {   }

      ~scoped_thread()
      {
         if ( _t.joinable() )
            join_or_detach();
      }

      void join_or_detach();

      scoped_thread( scoped_thread&& ) = default;
      scoped_thread& operator=( scoped_thread&& ) = default;

      std::thread& get() { return _t; }
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

   using join_thread = scoped_thread<thread_exec::join>;
   using detach_thread = scoped_thread<thread_exec::detach>;

}
}
#endif // THREAD_RAII_HPP
