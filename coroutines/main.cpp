#include <iostream>
#include <thread>
#include <chrono>
#include <experimental/coroutine>


//单线程coroutine和多线程coutine、特定任务特定线程，针对这些模式，应该在哪里下手？

class Task {
public:
    class promise_type {
    public:
        //这里可以做什么文章？？
        Task get_return_object()
        {
            std::cout << "promise_type::get_return_object()\n";
            return {};
        }
        //可以做某种意义上的惰性运行，暂时没想到具体能干嘛
        auto initial_suspend() -> std::experimental::suspend_never
        {
            std::cout << "promise_type::initial_suspend()\n";
            return {};
        }
        auto final_suspend() -> std::experimental::suspend_never
        {
            std::cout << "promise_type::final_suspend()\n";
            return {};
        }
        void unhandled_exception() { std::terminate(); }
        //这里可以做什么文章？？
        void return_void()
        {
            std::cout << "promise_type::return_void()\n";
        }
    };
};

class Awaitable {
public:
    Awaitable(){}
    //某些场景可以避免suspend，但是暂时也没想到是什么场景
    bool await_ready() const
    {
        std::cout << "Awaitable::await_ready()\n";
        return false;
    }
    //这里可以做什么文章？？
    int await_resume()
    {
        std::cout << "Awaitable::await_resume()\n";
        return result_;
    }
    //开始post某个异步任务，并且把handle传过去（调度器？）
    void await_suspend(std::experimental::coroutine_handle<> handle)
    {
        std::cout << "Awaitable::await_suspend()\n";
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
    std::cout << "result1:" << result << std::endl;
    result = co_await Awaitable{};
    std::cout << "result2:" << result << std::endl;
}

//clang++ -std=c++20 -stdlib=libc++ main.cpp -lpthread
int main()
{
    the_coroutine();
    std::this_thread::sleep_for(std::chrono::seconds(3));
    std::cout << "done" << std::endl;
    return 0;
}