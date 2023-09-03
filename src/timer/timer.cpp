#include "timer.h"

void Timer::reset() 
{ 
	m_beg = Clock::now(); 
}

double Timer::elapsed() const 
{ 
	return std::chrono::duration_cast<Second>(Clock::now() - m_beg).count(); 
}
