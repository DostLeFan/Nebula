#ifndef DEF_SCHEDULER
#define DEF_SCHEDULER

#include <vector>
#include <queue>
#include <unordered_set>
#include <functional>
#include <cstdint>

namespace nebula
{
	class Scheduler
	{
		public:
			using Callback = std::function<void()>;
			using EventId = uint64_t;
			
			// Conversion helper (1 M-cycle = 4 T-cycles in normal speed).
			static constexpr uint64_t MCycle = 4;
			
			Scheduler() = default;
			
			// Schedule an event that will fire after `delay` T-cycles.
			// Returns an EventId that can be used to cancel it later.
			EventId schedule(uint64_t delay, Callback cb);
			
			// Advance the clock by `cycles` T-cycles and execute all due events.
			void tick(uint64_t cycles);
			
			// Cancel a previously scheduled event.
			// Returns true if the event was still pending and has been cancelled.
			bool cancel(EventId id);
			
			// Current absolute cycle count.
			uint64_t currentCycle() const { return m_currentCycle; }
			
			// Remove all pending events (useful on reset / power-off).
			void clear();
		
		private:
			struct Event
			{
				uint64_t cycle; // Absolute cycle when the event should fire.
				EventId id;
				Callback cb;
				
				// For priority_queue (smallest cycle first).
				bool operator>(Event const& src) const { return cycle > src.cycle; }
			};
			
			uint64_t m_currentCycle = 0;
			EventId m_nextId = 1; // 0 is reserved as "invalid".
			std::priority_queue<Event, std::vector<Event>, std::greater<Event> > m_events;
			std::unordered_set<EventId> m_pending;
	};
}

#endif // DEF_SCHEDULER