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

#include <memory>
#include <functional>
#include <type_traits>
#include <utility>
#include <tuple>
#include "utility/std14.hpp"

template <typename ...Ts>
class sequence
{
public:
    template <typename Func, typename ...Us >
    sequence(Func&& work, Us&&... args) : _result{}
    {
        if (work)
           _result = work(std::forward<Us>(args)...);
    }

    template <typename Func >
    sequence(Func&& work) : _result{}
    {
        if (work)
           _result = work();
    }

    template <typename Func,
              typename R = typename std::result_of<Func(Ts...)>::type,
              typename Indices = std14::make_index_sequence<sizeof...(Ts)>>
    sequence<R> next(Func&& next)
    {
        return make_sequence (std::forward<Func>(next), _result, Indices{});
    }

    template <typename Func,
              typename ...Args,
              typename R = typename std::result_of<Func(Args...)>::type,
              typename std::size_t... I>
    static sequence<R> make_sequence(Func&& next, std::tuple<Args...>&& result, std14::index_sequence<I...>)
    {
        return sequence<R> (std::forward<Func>(next), std::get<I>(std::forward<std::tuple<Args...>>(result))... );
    }

private:
    std::tuple<Ts...> _result;
};

template <>
class sequence<void>
{
public:
    template <typename Func, typename ...Us >
    sequence(Func&& work, Us&&... args)
    {
        if (work)
           work(std::forward<Us>(args)...);
    }

    template <typename Func >
    sequence(Func&& work)
    {
        if (work)
           work();
    }

    template <typename Func, typename R = typename std::result_of<Func(void)>::type>
    sequence<R> next(Func&& next)
    {
        return sequence<R> (std::forward<Func>(next));
    }
};

template <typename Func, typename ...Us,  typename R = typename std::result_of<Func(Us...)>::type>
static sequence<R> start(Func&& work, Us&&... args)
{
    return sequence<R>(std::forward<Func>(work), std::forward<Us>(args)...);
}

template <typename Func,  typename R = typename std::result_of<Func(void)>::type>
static sequence<R> start(Func&& work)
{
    return sequence<R>(std::forward<Func>(work));
}

