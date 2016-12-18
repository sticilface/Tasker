#include "msTask.h"

bool Task::run(const uint8_t priority)
{
	if (finished) {
		return !_doNotDelete;
	}

	if (_enabled && _state == -1) {
		_state = 0;
		_lastcalled = _getTime();
	}

	if (_state == 0 && _enabled && _priority <= priority) {

		if (_getTime() - _lastcalled > _timeout) {

			count++;

			if (_cb) {
				_cb(*this);

				if (_repeat) {
					_lastcalled = _getTime();
				} else {

					return !_doNotDelete; //  calls delete!
				}
			}

		}

	}

	return false;

}