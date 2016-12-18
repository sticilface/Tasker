#include "usTask.h"

bool usTask::run(const uint8_t priority)
{
	bool result = false;

	if (_enabled && _state == -1) {
		_state = 0; 
		_lastcalled = micros();
	}


	if (_state == 0 && _enabled && _priority <= priority) {

		if (micros() - _lastcalled > _timeout) {
			count++;

			// uint32_t pre_micros = micros();
			// uint32_t cycles = ESP.getCycleCount();

			if (_cb) {
				_cb(*this);
			}
			
			// _cycles =   ( ( _cycles * ( count - 1)) + ( ESP.getCycleCount() - cycles ) ) / count; 
			// _micros =   ( ( _micros * ( count - 1)) + ( micros() - pre_micros  ) ) / count;

			if (_repeat) {
				_lastcalled = micros();
			} else {
				return true; //  calls delete!  
			}

		}

	}

	return result;

}