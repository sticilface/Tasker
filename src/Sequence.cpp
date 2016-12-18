

#include "Sequence.h"


bool Sequence::run(const uint8_t priority) 
{

	if (finished) {
		return true; 
	}

	if (_enabled && _state == -1) {
		_state = 0; 
		_lastcalled = getTime();
	}

	if (_state == 0 && _enabled) {

		if (getTime() - _lastcalled > _timeout) {

			if (_cb) {
				_cb(*this);
			}

			if (_repeat) {
				_lastcalled = getTime();
			} else {
				return true; //  calls delete!  
			}
			
		}

	}

	return false;

}