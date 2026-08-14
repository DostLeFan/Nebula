#include "../../../include/nebula/Scheduler/Scheduler.hpp"

namespace nebula
{
	// Schedule an event that will fire after `delay` T-cycles.
	// Returns an EventId that can be used to cancel it later.
	Scheduler::EventId Scheduler::schedule(uint64_t delay, Callback cb)
	{
		EventId id = m_nextId++;
		
		Event event;
		event.cycle = m_currentCycle + delay;
		event.id = id;
		event.cb = std::move(cb);
		
		m_events.push(std::move(event));
		m_pending.insert(id);
		
		return id;
	}
	
	
	// Advance the clock by `cycles` T-cycles and execute all due events.
	void Scheduler::tick(uint64_t cycles)
	{
		m_currentCycle += cycles;
		
		while(!m_events.empty())
		{
			Event const& top = m_events.top();
			
			if(top.cycle > m_currentCycle)
				break; // Next event is still in the future.
			
			Event event = m_events.top();
			m_events.pop();
			
			// If event is no more in m_pending, it was cancelled.
			
			if(m_pending.erase(event.id) == 0)
				continue;
			
			if(event.cb)
				event.cb();
		}
	}
	
	
	// Cancel a previously scheduled event.
	// Returns true if the event was still pending and has been cancelled.
	bool Scheduler::cancel(EventId id)
	{
		return m_pending.erase(id) > 0; // Erase return number of erased elements (0 or 1).
	}
	
	// Remove all pending events (useful on reset / power-off).
	void Scheduler::clear()
	{
		while(!m_events.empty())
			m_events.pop();
		
		m_pending.clear();
		// Note: we deliberately keep m_currentCycle and m_nextId.
	}
}