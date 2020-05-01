#include <iostream>
#include <thread>
#include <chrono>
#include <experimental/coroutine>


class Task {
public:
    class promise_type {
    public:
        Task get_return_object() { return {}; }
        auto initial_suspend() -> std::experimental::suspend_never { return {}; }
        auto final_suspend() -> std::experimental::suspend_never { return {}; }
        void unhandled_exception() { std::terminate(); }
        void return_void() {}
    };
};

class Awaitable {
public:
    Awaitable(){}
    bool await_ready() const { return false; }
    int await_resume() { return result_; }
    void await_suspend(std::experimental::coroutine_handle<> handle)
    {
        auto t = std::thread{[handle, this]() mutable {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            result_ = 123;
            handle.resume();
        }};
        t.detach();
    }
private:
    int result_ = 0;
};

Task the_coroutine()
{
    int result = co_await Awaitable{};
    std::cout << "result:" << result << std::endl;
}

//clang++ -std=c++20 -stdlib=libc++ main.cpp -lpthread
int main()
{
    the_coroutine();
    std::this_thread::sleep_for(std::chrono::seconds(2));
    std::cout << "done" << std::endl;
    return 0;
}