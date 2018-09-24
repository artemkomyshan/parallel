#include <queue>
#include <memory>
#include <mutex>
#include <condition_variable>

namespace parallel

template <typename T>
class queue
{
   mutable std::mutex _mut;
   std::queue<T> _q;
   std::conditioan_variable _cond;

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
   std::shared<T> try_pop();
   
   bool empty();	
};	

template <typename T>
inline 
queue<T>::queue( queue const& other )
{
   std::lock_guard<std::mutex> lk( _mut );   
   _q = other._q;
}

template <typename T>
inline 
queue<T>::push( T&& new_value )
{
   std::lock_guard<std::mutex> lk( _mut );
   _q.emplace( std::forward<T>( new_value ) );
   _cond.notify_one();   
}

template <typename T>
inline 
void queue<T>::wait_and_pop( T& value )
{
   std::unique_lock<std::mutex> lk( _mut );
   _cond.wait( lk, []{ return !_q.empty();}
   value = _q.front();
   _q.pop();
}

template <typename T>
inline 
std::shared_ptr<T> queue<T>::wait_and_pop()
{
   std::unique_lock<std::mutex> lk( _mut );
   _cond.wait( lk, []{ return !_q.empty();}
   auto res = std::make_shared<T>( _q.front() );
   _q.pop();
   return res;
}

template <typename T>
inline 
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
inline 
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
inline 
bool queue<T>::empty()
{
   std::lock_guard<std::mutex> lk( _mut );
   return _q.empty();
}

}