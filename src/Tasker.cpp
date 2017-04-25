#include "Tasker.h"


Task::Task(cb_t cb)
{
	_cb = cb;
}

Task::~Task()
{
	if (_endCb) {
		_endCb();
	}
}

Task & Task::add(cb_t cb)
{
	_storage.emplace_back(new Task(cb));
	return *_storage.back().get();
}

bool Task::remove(Task & task) 
{
	
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
	return *this;
}

Task & Task::setRepeat(int repeat)
{
	if (repeat >= 0) {
		_repeat = true;
		_remaining = repeat;
	}
	return *this;
}

Task & Task::setEndFn(endFn_t Fn)
{
	_endCb = Fn;
	return *this;
}

Task & Task::setName(String  name)
{
	_name = name;
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

bool Task::run()
{

	if (_state == INIT) {
		_state = WAITING;
		_lastrun = (_useMicros)?micros():millis();
	}

	if ( (_useMicros)?micros():millis() - _lastrun > _timeout) {

		/* Set Lastrun to current time */
		_lastrun = (_useMicros)?micros():millis();

		/* Run Callback if one is attached and it has remaining runs*/
		if (_remaining && _cb) {
			_cb(*this);
		}
		/* Decrement repeat counter if set */
		if (_remaining > 0) {
			_remaining--;
		}

		/*  run through attached tasks */ 
		if (_storage.size()) {
			for (st_t::iterator it = _storage.begin(); it != _storage.end() ; ) {
				Task & t = **it;
				if (t.run()) {
					it = _storage.erase(it);  /*  if task is finished bump to next, regardless of type  */
				} else if (_type == ASYNC) {
					++it;					  /*  if task is ASYNC run the next one */
				} else {
					break;                    /*  otherwise break loop  */
				}
			}
		}

		if ( !_storage.size() && ( !_repeat || !_remaining)) {
			_state = FINISHED;
			return true;
		}

	}
	return false;
}


void Task::dump(Stream & stream, int indent) {

	if (indent == 0) {
		stream.println("             ******       Tasker Tasks      ******           ");
	}

	for (int i = 0; i < indent; i++) {
		stream.print("  ");
	}

	stream.printf("[%p] %s, repeat = %s, remaining = %u, timeout = %ums, children =%u\n", this, _name.c_str(), (_repeat)?"true":"false", _remaining, _timeout, _storage.size()); 

	if (_type == ASYNC) {
		indent++;
		for (st_t::iterator it = _storage.begin(); it != _storage.end() ; ++it) {
			Task & t = **it;
			t.dump(stream,indent); 
		}
	} else if (_type == SYNC) {
		for (st_t::iterator it = _storage.begin(); it != _storage.end() ; ++it) {
			indent++;
			Task & t = **it;
			t.dump(stream,indent); 
		}
	}

}







