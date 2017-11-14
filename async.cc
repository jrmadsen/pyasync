
//----------------------------------------------------------------------------//

#include <future>
#include <chrono>
#include <functional>
#include <mutex>
#include <thread>
#include <atomic>
#include <iostream>
#include <cstdint>
#include <sstream>
#include <map>

typedef std::mutex mutex_t;
using auto_lock_t = std::lock_guard<mutex_t>;

//----------------------------------------------------------------------------//
// simple function wrapper for outputting to stdout
template <typename _Func>
void locked_function(_Func f)
{
    static mutex_t _mutex;
    auto_lock_t l(_mutex);
    f();
}

//----------------------------------------------------------------------------//
// simple function wrapper for locked function that returns
template <typename _Ret, typename _Func>
_Ret locked_function(_Func f)
{
    static mutex_t _mutex;
    auto_lock_t l(_mutex);
    return f();
}

//----------------------------------------------------------------------------//
// fibonacci calculation
int64_t fibonacci(int64_t n)
{
    if (n < 2) return n;
    return fibonacci(n-1) + fibonacci(n-2);
}

//----------------------------------------------------------------------------//
// calculate fibonacci to do various amounts of work
int64_t async_work()
{
    static std::atomic<int64_t> ncall;
    static std::atomic<int64_t> tid;
    static std::map<std::thread::id, int64_t> tids;

    int64_t _ncall = ncall++;

    auto get_tid = [&]()
    {
        auto insert = [&]()
        {
            if(tids.find(std::this_thread::get_id()) == tids.end())
                tids[std::this_thread::get_id()] = ++tid;
            return tids[std::this_thread::get_id()];
        };
        return locked_function<int64_t>(insert);
    };

    // this shows (via get_tid == "get thread id") that this implementation
    // is not using a thread pool... we can fix that
    std::stringstream ss;
    ss << "(C++) Function call #" << _ncall << " on thread # "
       << get_tid() << "(tid: " << std::this_thread::get_id()
       << ")..." << std::endl;

    locked_function([&]() { std::cout << ss.str(); });

    return fibonacci(_ncall % 44);
}

//----------------------------------------------------------------------------//
// called from Python in a loop. In a standard MT situation, this would normally
// block before returning to Python Interp or you would have to move the
// loop down into the C++ layer for performance
std::future<int64_t> async_run()
{
    // async will use a thread pool and defer executation until a later
    // time or when the std::future it returns calls its member function
    // "get()"
    return std::async(std::launch::async | std::launch::deferred, 
                      async_work);
}

//----------------------------------------------------------------------------//

#include <pybind11/pybind11.h>

//----------------------------------------------------------------------------//

namespace py = pybind11;

PYBIND11_MODULE(async, t)
{
    // we have to wrap each return type
    py::class_<std::future<int64_t>> fint (t, "int64_future_t");
    fint.def("get", &std::future<int64_t>::get, "Return the result")
        .def("wait", &std::future<int64_t>::wait,
             "Wait for the result to become avail");

    // the function called from Python loop
    t.def("run", &async_run, "Run asynchron");
}

//----------------------------------------------------------------------------//
