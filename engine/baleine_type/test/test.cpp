#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <thread>

#include "baleine_type/mutex.h"
#include "doctest/doctest.h"

struct Foo {
    int a;
    bool b;
};

TEST_SUITE_BEGIN("Test Mutex");

TEST_CASE("Mutex") {
    using namespace std::chrono_literals; 
    baleine::MutexVal<int> resource_a(1);
    
    auto task1 = [&] {
        auto guard = resource_a.lock();
        *guard += 2;
    };
    
    auto task2 = [&] {
        auto guard = resource_a.lock();
        *guard += 3;
    };
    
    std::thread t1(task1), t2(task2);
    t1.join(); t2.join();

    auto guard = resource_a.lock();
    CHECK(*guard == 6);
}

TEST_SUITE_END();