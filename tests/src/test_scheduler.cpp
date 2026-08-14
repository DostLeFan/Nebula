#include <catch2/catch_test_macros.hpp>
#include <nebula/Scheduler/Scheduler.hpp>
#include <vector>

using namespace nebula;

TEST_CASE("Scheduler - basic schedule and tick", "[scheduler]")
{
	Scheduler sched;
	int value = 0;
	
	sched.schedule(10, [&]() { value = 42; });
	
	REQUIRE(value == 0);
	REQUIRE(sched.currentCycle() == 0);
	
	sched.tick(5);
	REQUIRE(value == 0);
	REQUIRE(sched.currentCycle() == 5);
	
	sched.tick(5); // Reaches exactly 10.
	REQUIRE(value == 42);
	REQUIRE(sched.currentCycle() == 10);
}

TEST_CASE("Scheduler - multiple events in order", "[scheduler]")
{
	Scheduler sched;
	std::vector<int> order;
	
	sched.schedule(30, [&]() { order.push_back(3); });
	sched.schedule(10, [&]() { order.push_back(1); });
	sched.schedule(20, [&]() { order.push_back(2); });
	
	sched.tick(100);
	
	REQUIRE(order.size() == 3);
	REQUIRE(order[0] == 1);
	REQUIRE(order[1] == 2);
	REQUIRE(order[2] == 3);
}

TEST_CASE("Scheduler - events at same cycle", "[scheduler]")
{
	Scheduler sched;
	std::vector<int> order;
	
	sched.schedule(10, [&]() { order.push_back(1); });
	sched.schedule(10, [&]() { order.push_back(2); });
	
	sched.tick(10);
	
	REQUIRE(order.size() == 2);
	
	// Order of same-cycle events is not strictly defined, but both must run.
	REQUIRE((order[0] == 1 || order[0] == 2));
}

TEST_CASE("Scheduler - cancel before execution", "[scheduler]")
{
	Scheduler sched;
	int value = 0;
	
	auto id = sched.schedule(20, [&]() { value = 99; });
	
	REQUIRE(sched.cancel(id) == true);
	REQUIRE(sched.cancel(id) == false); // Already cancelled.
	
	sched.tick(50);
	REQUIRE(value == 0); // Never executed.
}

TEST_CASE("Scheduler - cancel after execution does nothing", "[scheduler]")
{
	Scheduler sched;
	int value = 0;
	
	auto id = sched.schedule(5, [&]() { value = 1; });
	
	sched.tick(5);
	REQUIRE(value == 1);
	
	REQUIRE(sched.cancel(id) == false); // Already fired.
}

TEST_CASE("Scheduler - clear removes all pending events", "[scheduler]")
{
	Scheduler sched;
	int a = 0, b = 0;
	
	sched.schedule(10, [&]() { a = 1; });
	sched.schedule(20, [&]() { b = 1; });
	
	sched.clear();
	sched.tick(100);
	
	REQUIRE(a == 0);
	REQUIRE(b == 0);
}

TEST_CASE("Scheduler - currentCycle advances correctly", "[scheduler]")
{
	Scheduler sched;
	
	REQUIRE(sched.currentCycle() == 0);
	sched.tick(7);
	REQUIRE(sched.currentCycle() == 7);
	sched.tick(3);
	REQUIRE(sched.currentCycle() == 10);
}

TEST_CASE("Scheduler - MCycle helper", "[scheduler]")
{
	REQUIRE(Scheduler::MCycle == 4);
	
	Scheduler sched;
	int value = 0;
	
	// Schedule in 2 M-cycles = 8 T-cycles.
	sched.schedule(2 * Scheduler::MCycle, [&]() { value = 1; });
	
	sched.tick(7);
	REQUIRE(value == 0);
	sched.tick(1);
	REQUIRE(value == 1);
}

TEST_CASE("Scheduler - schedule with delay 0 fires on next tick", "[scheduler]")
{
	Scheduler sched;
	int value = 0;
	
	sched.schedule(0, [&]() { value = 1; });
	
	REQUIRE(value == 0);
	sched.tick(1); // Even 0 cycles would work, but tick(0) is a no-op for time.
	REQUIRE(value == 1);
}

TEST_CASE("Scheduler - many events stress", "[scheduler]")
{
	Scheduler sched;
	int count = 0;
	
	for(int i=0;i<1000;++i)
		sched.schedule(i * 3 + 1, [&]() { ++count; });
	
	sched.tick(1000 * 3 + 10);
	REQUIRE(count == 1000);
}