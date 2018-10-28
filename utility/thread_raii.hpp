#ifndef THREAD_RAII_HPP
#define THREAD_RAII_HPP

#include <thread>

/**
 * Example
 * tread_raii( std::thread( []{ do work.. }, tread_raii::dtor_action::detach ) );
 * Note: by default tread_raii use join
 */

class thread_raii
{
public:
   enum class dtor_action
   {
      join,
      detach
   };

private:
   dtor_action _action;
   std::thread _t; //should be last member, as thread can start immediately

public:
   thread_raii( std::thread&& t, dtor_action a = dtor_action::join ) :
      _action( a ),
      _t( std::move( t ) )
   {}

   ~thread_raii()
   {
      if ( _t.joinable() )
         if ( dtor_action::join == _action )
            _t.join();
         else
            _t.detach();
   }

   thread_raii( thread_raii&& ) = default;
   thread_raii& operator=( thread_raii&& ) = default;

   std::thread& get() { return _t; }

};


#endif // THREAD_RAII_HPP
