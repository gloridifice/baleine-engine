#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <thread>

#include "baleine_type/mutex.h"
#include "baleine_type/primitive.h"
#include "baleine_type/result.h"
#include "doctest/doctest.h"

struct Foo {
    int a;
    bool b;
};

TEST_SUITE_BEGIN("Test mutex.h");

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
    t1.join();
    t2.join();

    auto guard = resource_a.lock();
    CHECK(*guard == 6);
}

TEST_SUITE_END();

TEST_SUITE_BEGIN("Test result.h");

TEST_CASE("Result") {
    using namespace baleine;
    std::vector a {1, 2, 3};

    auto ok = Ok(std::move(a));
    CHECK(ok.is_ok());
    // content moved here!
    CHECK(ok.unwrap()[0] == 1);
    // so 'ok' is not valid now.
    CHECK(!ok.is_valid());

    auto err = Err<int>(std::make_unique<LogicError>("This is a Logic Error"));
    CHECK(err.is_err());
}

TEST_SUITE_END();