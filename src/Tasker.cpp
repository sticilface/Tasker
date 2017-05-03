#include "Tasker.h"


Task::Task(cb_t cb)
{
	_cb = cb;
}

Task::~Task()
{
	//Serial.printf("%p ~Task\n", this);
	if (_endCb) {
		_endCb();
	}
}

Task & Task::add(cb_t cb)
{
	_storage.emplace_back(new Task(cb));
	return *_storage.back().get();
}

Task & Task::setType(Task_t type)
{
	_type = type;
	return *this;
}

Task & Task::setTimeout(uint32_t timeout)
{
	_timeout = timeout;
	return *this;
}

Task & Task::setRepeat(bool repeat)
{
	_repeat = repeat;
	_remaining = 0;
	return *this;
}

Task & Task::setRepeat(int repeat)
{
	if (repeat >= 0) {
		_remaining = repeat;
		_repeat = false;
	}
	return *this;
}

Task & Task::onEnd(endFn_t Fn)
{
	_endCb = Fn;
	return *this;
}

Task & Task::setName(String  name)
{
	_name = name;
	return *this;
}

Task & Task::setDelete(bool deletable)
{
	_canDelete = deletable;
	return *this;
}

Task & Task::setMicros(bool useMicros)
{
	_useMicros = useMicros;
	return *this;
}

Task::Task_state Task::getState()
{
	return _state;
}

uint32_t Task::getTimeout()
{
	return _timeout;
}

String Task::getName()
{
	if (_name) {
		return _name;
	} else {
		return "null";
	}
}

void Task::runNow()
{
	if (_cb) {
		_lastrun = (_useMicros) ? micros() : millis();
		_cb(*this);
		if (_remaining > 0) {
			_remaining--;
		}

	}
}

/**
 *
 *	Need to allow countdown to be reset.  ie... run 5 times then done, but if repeat set..
 *	dont delete, but allow it to be reset to run another 5 times.
 *
 */
bool Task::run(bool override)
{

	if (_state == INIT) {
		_state = WAITING;
		_lastrun = (_useMicros) ? micros() : millis();
		_it = _storage.begin();
	}

	if ( ( (_useMicros) ? micros() : millis() ) - _lastrun > _timeout) {

		bool hasRun = false;

		/* Set Lastrun to current time */
		_lastrun = (_useMicros) ? micros() : millis();

		/* Run Callback if one is attached and it has remaining runs, is repeat, or overridden*/
		if ( (_remaining || _repeat || override) && _cb) {
			_cb(*this);
			hasRun = true;
		}
		/* Decrement repeat counter if being used */
		if (_remaining > 0) {
			_remaining--;
		}

		/*  run through attached tasks */
		if (_storage.size()) {
			for ( _it  ; _it != _storage.end() ; ) {
				Task & t = **_it;

				if (t.canDelete()) {
					_it = _storage.erase(_it);  //  Clean up
				}

				if (t.run(false)) {
					t.reset();    /*  This resets the task after its run. required for SYNC tasks to reset timer */
					if (t.canDelete()) {
						_it = _storage.erase(_it);  /*  if task is finished bump to next, regardless of type  */
					} else if (!t.finished()) {
						//Serial.printf("[%p] Waiting.... \n", this);
						break;
					} else {
						//Serial.printf("[%p] Advancing to NEXT \n", this);
						++_it;  /*  if its finished, bump to the next , but not if it has remaining runs or is repeat */
					}
				} else if (_type == ASYNC) {
					++_it;					  /*  if task is ASYNC move onto the next one */
				} else {
					break;                    /*  otherwise break loop  */
				}


			}

			if ( (override || _repeat) && _it == _storage.end()) {
				//Serial.println("Setting _it = begin()");
				_it = _storage.begin();
			}
		}

		if (hasRun) {
			//Serial.printf("   [%p] hasRun = true\n", this);
			return true;
		}

	}

	return false;
}


void Task::dump(Stream & stream, int indent)
{

	if (indent == 0) {
		stream.println("             ******       Tasker Tasks      ******           ");
	}

	for (int i = 0; i < indent; i++) {
		stream.print("  ");
	}

	stream.printf("[%p] %s, type = %s, repeat = %s, remaining = %u, timeout = %ums, children =%u\n", this, _name.c_str(), (_type == ASYNC) ? "ASYNC" : "SYNC" , (_repeat) ? "true" : "false", _remaining, _timeout, _storage.size());

	if (_type == ASYNC) {
		indent++;
		for (st_t::iterator it = _storage.begin(); it != _storage.end() ; ++it) {
			Task & t = **it;
			t.dump(stream, indent);
		}
	} else if (_type == SYNC) {
		for (st_t::iterator it = _storage.begin(); it != _storage.end() ; ++it) {
			indent++;
			Task & t = **it;
			t.dump(stream, indent);
		}
	}

}

bool Task::canDelete()
{
	if (_canDelete && finished() ) {
		//Serial.printf("   [%p] canDelete() = true\n", this);
		return true;
	} else {
		return false;
	}
}

void Task::reset()
{
	_state = INIT;
	//  need to add resest to init count.. etc....
}

bool Task::remove(const Task * task)
{
	if (!task) return false;  
	for (auto it = _storage.begin() ; it != _storage.end() ; ++it) {
		Task & t = **it;
		if (&t == task) {
			if (it == _it) {
				++_it; 
			}
			_storage.erase(it);
			return true;
		} else {
			return t.remove(task); //  remove nested tasks... if found there... 
		}
	}
	return false;
}

bool Task::finished()
{
	if (!_storage.size() && !_repeat && !_remaining) {
		return true;
	} else {
		return false;
	}
}

