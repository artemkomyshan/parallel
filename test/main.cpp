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

#include "tst_parallel.hpp"
#include <gtest/gtest.h>

#include <iostream>

int main(int argc, char *argv[])
{
   ::testing::InitGoogleTest(&argc, argv);
   return RUN_ALL_TESTS();
}


//#include "utility/property.hpp"
//#include <iostream>

//struct no_action_on_fail{};

//template <>
//inline void action_on_fail<no_action_on_fail>()
//{
//};

//int main(int argc, char *argv[])
//{
//struct movable
//{
//   bool is_moved {false};
//   movable(){ std::cout << "construct " << this << std::endl; }

//   movable( movable&& o)
//   {
//      std::cout << "move construct " << this << " o " << &o  << std::endl;
//      o.is_moved = true;
//   }

//   movable& operator=( movable&& o)
//   {
//      std::cout << "move =" << this << " o " << &o  << std::endl;
//      o.is_moved = true;
//      return*this;
//   }
//   movable( movable const& o)
//   {
//      std::cout << "copy construct" << this << " o " << &o  << std::endl;
//   }

//   movable& operator=( movable const& o)
//   {
//      std::cout << "copy =" << this << " o " << &o  << std::endl;
//      return*this;
//   }
//};
///*
//property<movable, no_action_on_fail> p {movable{}};
//movable m = p.release();

//EXPECT_EQ( false, p.is_valid() );
//EXPECT_EQ( true, p.value().is_moved );

//p = movable{};

//EXPECT_EQ( true, p.is_valid() );

//m = std::move( p ).value();
//EXPECT_EQ( false, p.is_valid() );
//EXPECT_EQ( true, p.value().is_moved );

//*/
//property<movable, no_action_on_fail>p { movable()};

//movable m = std::move( p ).value_or( movable() );

//std::cout <<p.value().is_moved << &p.value() << std::endl;

////   property<int> p1 = 5;
////   property<int> p2 ( std::move( p1 ) );

////   EXPECT_EQ( 5, p2.value() );
//return 0;
//}
