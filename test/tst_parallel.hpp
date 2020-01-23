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

#include "gtest/gtest.h"
#include "gmock/gmock-matchers.h"

#include <memory>

#include "raii/multi_lock.hpp"
#include "raii/scoped_thread.hpp"
#include "containers/thread_pool.hpp"
#include "utility/sequence.hpp"
#include "utility/property.hpp"
#include "utility/not_null.hpp"

#include <iostream>
using namespace testing;

TEST(paralel, property)
{
   property<int> p;
   EXPECT_EQ( false, p.is_valid() );

   p = 5;
   EXPECT_EQ( true, p.is_valid() );
   EXPECT_EQ( 5, p.value() );

   p.invalidate();
   EXPECT_EQ( false, p.is_valid() );

   int s = 8;
   p = s;
   EXPECT_EQ( true, static_cast<bool>( p ) );
   EXPECT_EQ( 8, p.value() );

   p.invalidate();

   EXPECT_EQ( 10, p.value_or( 10 ) );
}

struct no_action_on_fail{};

template <>
inline void action_on_fail<no_action_on_fail>()
{
};

TEST(paralel, property_action_on_fail)
{
    property<int> pex{1};
    property<int, no_action_on_fail> pnon{2};
    property<int, assert_on_fail> pas{3};

    pex = pnon;
    pnon = std::move( pas );

    EXPECT_EQ( true, pex.is_valid() );
    EXPECT_EQ( 2, pex.value() );

    EXPECT_EQ( true, pnon.is_valid() );
    EXPECT_EQ( 3, pnon.value() );

    EXPECT_EQ( false, pas.is_valid() );
}

TEST(paralel, property_move)
{
   struct movable
   {
      bool is_moved {false};
      movable(){ /*std::cout << "construct" << std::endl;*/ }

      movable( movable&& o)
      {
         //std::cout << "move construct" << std::endl;
         o.is_moved = true;
      }

      movable& operator=( movable&& o)
      {
         //std::cout << "move =" << std::endl;
         o.is_moved = true;
         return*this;
      }
      movable( movable const&)
      {
         //std::cout << "copy construct" << std::endl;
      }

      movable& operator=( movable const&)
      {
         //std::cout << "copy =" << std::endl;
         return*this;
      }
   };

   property<movable, no_action_on_fail> p {movable{}};
   movable m = p.release();

   EXPECT_EQ( false, p.is_valid() );
   EXPECT_EQ( true, p.value().is_moved );

   p = movable{};

   EXPECT_EQ( true, p.is_valid() );

   m = std::move( p ).value();
   EXPECT_EQ( false, p.is_valid() );
   EXPECT_EQ( true, p.value().is_moved );


   p = movable();
   EXPECT_EQ( true, p.is_valid() );

   m = std::move( p ).value_or( movable{} );
   EXPECT_EQ( false, p.is_valid() );
   EXPECT_EQ( true, p.value().is_moved );


   property<int> p1 = 5;
   property<int> p2 ( std::move( p1 ) );

   EXPECT_EQ( 5, p2.value() );
}


TEST(paralel, paralel)
{

   struct s
   {
      void lock(){
         loc_num++;

      }
      void unlock(){
         unloc_num++;
      }

      int unloc_num {0};
      int loc_num {0};
   };

   std::mutex m2;
   std::mutex m3;
   s m1;
   {
      auto guards = parallel::raii::make_locks(m1, m2, m3);

      EXPECT_EQ(1, m1.loc_num);
      EXPECT_EQ(0, m1.unloc_num);
   }

   EXPECT_EQ(1, m1.unloc_num);

   {
      auto guards = parallel::raii::make_locks(m1, m2, m3);

      EXPECT_EQ(2, m1.loc_num);
   }

   EXPECT_EQ(2, m1.unloc_num);
}

TEST(paralel, not_null)
{
   not_null<std::shared_ptr<int>> nnsp = std::make_shared<int>( 6 );
   nnsp.get();
   EXPECT_EQ( true, static_cast<bool>( nnsp.get() ) );

   not_null<std::unique_ptr<int>> nnup = std::make_unique<int>( 6 );
   nnup.get();
   EXPECT_EQ( true, static_cast<bool>( nnup.get() ) );
}

TEST(paralel, scoped_thread)
{
   {
      parallel::raii::detach_thread t_d([](int i)
      {
            EXPECT_EQ(1, i);
      }, 1);

      parallel::raii::join_thread t_j([](int i)
      {
            EXPECT_EQ(1, i);
      }, 1);
   }
}


TEST(paralel, thread_pool)
{
   std::atomic_int count = {0};
   {
      std::condition_variable cv;
      std::mutex m;

      parallel::thread_pool pool;
      for (auto i : boost::irange(std::thread::hardware_concurrency()))
      {
         pool.submit([&count, &cv, &m]{
            static thread_local int t = [&count, &cv, &m]{
               ++count;
               std::cout << count << "c ";

               if (count == std::thread::hardware_concurrency())
               {
                  std::cout << count << "v ";
                  cv.notify_one();
               }
               else
               {
                  std::unique_lock<std::mutex> l(m);
                  cv.wait(l);
                  cv.notify_one();
               }
               return 0;
            }();

         });
      }

      std::unique_lock<std::mutex> l(m);
      cv.wait(l);
   }
   std::cout << std::endl;
   EXPECT_EQ(std::thread::hardware_concurrency(), count);
}

/**
TEST(paralel, sequence)
{
   start([]
   {
       std::cout << "first void job \n";
       return std::make_tuple(6);
   }).next([](int i)
   {
      std:: cout << "first int job " << i << "\n";
       return std::make_tuple(7);
   }).next([](int i)
   {
       std::cout << "second job " << i << "\n";
   }).next([]
   {
       std::cout << "second void job\n";
   });
}
*/
