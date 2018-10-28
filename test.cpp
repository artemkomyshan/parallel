#include <iostream>
//#include "utility/sequence.hpp"
#include "utility/lock_guards.hpp"

using namespace std;

/*
void sequenceExemple()
{
   start([]
   {
       cout << "first void job \n";
       return std::make_tuple(6);
   }).next([](int i)
   {
       cout << "first int job " << i << "\n";
       return std::make_tuple(7);
   }).next([](int i)
   {
       cout << "second job " << i << "\n";
   }).next([]
   {
       cout << "second void job\n";
   });
}
*/

class adopt_lock_guard
{
public:
   int& _mutex;
   adopt_lock_guard(int& mutex) noexcept
   : _mutex(mutex)
   {
      std::cout << __FUNCTION__ << " begin" << std::endl; }
   adopt_lock_guard(adopt_lock_guard const& other)
   : _mutex(other._mutex)
   {
      std::cout << __FUNCTION__ << " copy begin" << std::endl; }
   ~adopt_lock_guard()
   {
      std::cout << __FUNCTION__ << " begin" << std::endl;
   }
};


class A
{
public:
   ~A()
   {
      cout << __FUNCTION__ << endl;
   }
};

class B : public A
{
public:
   ~B()
   {
      cout << __FUNCTION__ << endl;
   }
};

int main()
{
   struct s
   {
      void lock(){
         cout << "lock! " << endl;}
      void unlock(){
         cout << "unlock! " << endl;}
   };

   std::mutex m2;
   std::mutex m3;
   s m1;

   std_ext::lock_guards guards(m1, m2, m3);

   //int f = 5; auto tuple_ptr = std::tuple<adopt_lock_guard, adopt_lock_guard>(f, f);


    cout << "Hello World! " << endl;
    return 0;
}
