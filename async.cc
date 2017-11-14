
//----------------------------------------------------------------------------//

#include <future>
#include <chrono>
#include <functional>
#include <mutex>
#include <thread>
#include <iostream>
#include <cstdint>

//----------------------------------------------------------------------------//
// fibonacci calculation
int64_t fibonacci(int64_t n)
{
    if (n < 2) return n;
    return fibonacci(n-1) + fibonacci(n-2);
}

//----------------------------------------------------------------------------//

int64_t async_work()
{
    static std::mutex _mutex;
    static int64_t ncall = 0;
    
    int64_t _ncall;
    //std::this_thread::sleep_for(std::chrono::milliseconds(20));
    {
        std::lock_guard<std::mutex> l(_mutex);
        _ncall = ncall++;
        std::cout << "(C++) Function call #" << _ncall << "..." << std::endl;
    }
    
    return fibonacci(_ncall % 44);
}

//----------------------------------------------------------------------------//

std::future<int64_t> async_run()
{
    return std::async(std::launch::async | std::launch::deferred, 
                      async_work);
}

//----------------------------------------------------------------------------//

#include <pybind11/pybind11.h>
#include <pybind11/embed.h>

//----------------------------------------------------------------------------//

namespace py = pybind11;

PYBIND11_MODULE(async, t)
{
    py::class_<std::future<int64_t>> fint (t, "int64_future_t");
    fint.def("get", &std::future<int64_t>::get, "Return the result")
        .def("wait", &std::future<int64_t>::wait,
             "Wait for the result to become avail");

    t.def("run", &async_run, "Run asynchron");
}

//----------------------------------------------------------------------------//
