#ifndef SEQUENCE_H
#define SEQUENCE_H

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

#endif // SEQUENCE_H
