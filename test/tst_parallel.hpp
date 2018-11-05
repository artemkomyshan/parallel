/*
* Copyright (c) 2018 Parallel
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

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>
#include "raii/multi_lock.hpp"

using namespace testing;

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
   }

   EXPECT_EQ(1, m1.unloc_num);
}
