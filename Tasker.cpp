
#include "Tasker.h"



bool msTask::run()
{
	bool result = false;

	if (_enabled) {

		if (millis() - _lastcalled > _timeout) {
			count++;

			uint32_t pre_micros = micros();
			uint32_t cycles = ESP.getCycleCount();

			_cb(*this);

			_cycles =   ( ( _cycles * ( count - 1)) + ( ESP.getCycleCount() - cycles ) ) / count; 
			_micros =   ( ( _micros * ( count - 1)) + ( micros() - pre_micros  ) ) / count;

			if (_repeat) {
				_lastcalled = millis();
			} else {
				return true; //  calls delete!  
			}

		}

	}

	return result;

}

//  need to create re-order



	// void Tasker<class T> _sort()
	// {
	// 	_list.sort( [](const task_t& lhs, const task_t & rhs) {

	// 		return ( lhs->getPriority() < rhs->getPriority() );

	// 	});
	// }