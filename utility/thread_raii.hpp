#ifndef THREAD_RAII_H
#define THREAD_RAII_H

#include <thread>

/**
 * Example
 * tread_raii( std::thread( []{ do work.. }, tread_raii::dtor_action::detach ) );
 */

class tread_raii
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
   tread_raii( std::thread&& t, dtor_action a ) :
      _action( a ),
      _t( std::move( t ) )
   {}

   ~thread_raii()
   {
      if ( t.joinable() )
         if ( dtor_action::join == _action )
            _t.join();
         else
            _t.detach();
   }

   tread_raii( tread_raii&& ) = default;
   tread_raii& operator=( tread_raii&& ) = default;

   std::thread& get() { return t; }

};


#endif // THREAD_RAII_H
