#include "msTask.h"

bool Task::run(const uint8_t priority)
{

	if (_enabled && _state == -1) {
		_state = 0; 
		_lastcalled = millis();
	}

	if (_state == 0 && _enabled && _priority <= priority) {

		if (millis() - _lastcalled > _timeout) {


			// if (_timeout > 1000) {
			// 	Serial.printf("RUNNING.. @%p (%s) millis = %u, _lastcalled = %u, _timeout = %u, %s\n", this, (name()) ? name() : "null", millis(), _lastcalled, _timeout, (_repeat)? " --> repeat" : " --> delete"); 
			// }


			count++;

			uint32_t pre_micros = micros();
			uint32_t cycles = ESP.getCycleCount();

			if (_cb) {
				_cb(*this);
			}
			
			_cycles =   ( ( _cycles * ( count - 1)) + ( ESP.getCycleCount() - cycles ) ) / count; 
			_micros =   ( ( _micros * ( count - 1)) + ( micros() - pre_micros  ) ) / count;

			if (_repeat) {
				
				_lastcalled = millis();
			} else {
				_lastcalled = millis();
				return true; //  calls delete!  
			}

		}

	}

	return false;

}