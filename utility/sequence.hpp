#ifndef SEQUENCE_H
#define SEQUENCE_H

#include <memory>
#include <functional>
#include <type_traits>
#include <utility>

template <typename T>
class sequence
{
public:
    template <typename Func, typename U >
    sequence(Func&& work, U i) : _result{}
    {
        if (work)
           _result = work(i);
    }

    template <typename Func >
    sequence(Func&& work) : _result{}
    {
        if (work)
           _result = work();
    }

    template <typename Func, typename R = typename std::result_of<Func(T)>::type>
    sequence<R> next(Func&& next)
    {
        return sequence<R> (std::forward<Func>(next), _result);
    }

private:
    T _result; //should be optional
};

template <>
class sequence<void>
{
public:
    template <typename Func, typename U >
    sequence(Func&& work, U i)
    {
        if (work)
           work(i);
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
        return sequence<R> (std::move(next));
    }
};

template <typename Func, typename U,  typename R = typename std::result_of<Func(U)>::type>
static sequence<R> start(Func&& work, U i)
{
    return sequence<R>(std::move(work), i);
}

template <typename Func,  typename R = typename std::result_of<Func(void)>::type>
static sequence<R> start(Func&& work)
{
    return sequence<R>(std::move(work));
}

#endif // SEQUENCE_H
