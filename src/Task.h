
#pragma once


#ifndef MY_TASKER_TASK_H
#define MY_TASKER_TASK_H


#include <Arduino.h>
#include <functional>
#include <list>
#include <memory>
#include <time.h>

class Task
{

public:

	typedef std::function<void(Task&)> taskerCb_t;


	Task (taskerCb_t cb, bool useMicros = false) : _useMicros(useMicros)
	{
		//Serial.printf("  Task created @%p\n", this);
		_cb = cb;
		_lastcalled = _getTime();
	}

	~Task()
	{
		//q23[p;oSerial.printf("~Task @%p (%s)\n", this, (_name) ? _name : "null" );
		if (_onEnd) {
			_onEnd();
		}

		if (_name) {
			free( (char *)_name);
		}
	}

	bool run(const uint8_t priority = 0, const bool override = false)
	{
		if (override || ( _enabled && _priority <= priority)) {

			if (override || (_getTime() - _lastcalled > _timeout)) {

				if (_usetime && difftime(_time, time(nullptr)) > 0) {
					return false; //  this returns false if there are remaining seconds left to execute..  otherwise proceed...
				}

				count++;

				if (_cb) {

					_cb(*this);

					if (_repeat) {
						_lastcalled = _getTime();
						if (_repeatNo == (int)count) {
							return !_doNotDelete;
						}

					} else {
						//Serial.printf("Run returned : %s\n", (!_doNotDelete)? "true" : "false" );
						return !_doNotDelete; //  calls delete!
					}
				}

			}

		}

		return false;

	}

	Task & runCb()
	{
		_lastcalled = _getTime();
		if (_cb) {
			_cb(*this);
		}
		return *this;
	}

	// Task & setTimeout(const uint32_t timeout) // timeout in milliseconds/microseconds. 
	// {
	// 	_timeout = timeout;
	// 	_usetime = false;
	// 	return *this;
	// }

	Task & setTimeout(const uint32_t timeout)
	{
		_timeout = timeout;
		_usetime = false;
		return *this;
	}

	Task & setTime(const time_t attime)
	{

		if (difftime(attime, time(nullptr)) > 0) {
			_timeout = 1000; // set to 1 second resolution
			_time = attime;
			_usetime = true;
		} else {
			Serial.printf("Task set to run in the past\n");
			_enabled = false;
		}

		return *this;
	}

	Task & setTime(tm * attime)
	{
		setTime(mktime(attime));
		return *this;
	}

	uint32_t getTimeout()
	{
		return _timeout;
	}

	uint32_t getMsLeft() //  returns time to execution in ms...
	{

		if (_usetime) {
			return  static_cast<uint32_t>( difftime(_time, time(nullptr)) * 1000) ;

		} else {

			return (((_lastcalled + _timeout) -  _getTime() )) ;
		}


	}

	const time_t getNextRunTime() {

		if (_usetime) {
			return _time ;

		} else {
			time_t now = time(nullptr); 
			tm t = *localtime( &now ); 
			t.tm_sec = t.tm_sec +  ( getMsLeft() / 1000 ); 
			return mktime(&t);
		}		
	}

	Task & setRepeat(const bool repeat = true)
	{
		_repeat = repeat;
		return *this;
	}

	Task & setRepeat(const int number)
	{
		_repeatNo = number;

		if (number > 0) {
			_repeat = true;
		} else {
			_repeat = false;
		}
		return *this;
	}

	Task & setPriority(const uint8_t pr)
	{
		_priority = pr;
		return *this;
	}

	uint8_t getPriority()
	{
		return _priority;
	}

	Task & enable()
	{
		_enabled = true;
		return *this;
	}

	Task & disable()
	{
		_enabled = false;
		return *this;
	}

	Task & setName(const char * name)
	{
		if (_name) {
			free( (char*)_name);
		}
		_name = strdup(name);
		return *this;
	}

	const char * name()
	{
		return (_name) ? _name : "null" ;
	}

	void reset()
	{
		_lastcalled = millis();
	}

	Task & onEnd(std::function<void(void)> Cb)
	{
		_onEnd = Cb;
		return *this;
	}

	Task & setPersistent(bool donotdelete)
	{
		_doNotDelete = donotdelete;
		return *this;
	}

	// Task & at(uint32_t time)
	// {
	// 	_timeout = time;
	// 	_at = true;
	// 	return *this;
	// }

	bool running()
	{
		return _running;
	}


	uint32_t count{0};
	//bool finished{false};

protected:

	inline uint32_t _getTime()
	{
		return (_useMicros) ? micros() : millis();
	}

private:

	taskerCb_t  _cb;
	bool _repeat{false};
	int _repeatNo{ -1};
	uint32_t _timeout{0};
	uint32_t _lastcalled{0};
	bool _enabled{true};
	uint8_t _priority{0};
	const char * _name{nullptr};
	//bool _at{false};
	//int8_t _state{ -1};
	std::function<void(void)> _onEnd;
	const bool _useMicros{false};
	bool _doNotDelete{false};

	bool _running{false};
	time_t _time{0};
	bool _usetime{false};

};




class SimpleTask
{

public:

	typedef std::function<void(SimpleTask&)> taskerCb_t;

	SimpleTask (taskerCb_t cb, bool useMicros = false)
	{
		_cb = cb;
		_lastcalled = millis();
	}

	~SimpleTask()
	{

	}

	bool run(const uint8_t priority = 0)
	{

		if (_enabled && millis() - _lastcalled > _timeout) {

			if (_cb) {
				_cb(*this);

				if (_repeat) {
					_lastcalled = millis();

				} else {
					return true; //  calls delete!
				}
			}
		}

		return false;

	}

	SimpleTask & setTimeout(uint32_t timeout)
	{
		_timeout = timeout;
		return *this;
	}

	SimpleTask & setRepeat(bool repeat = true)
	{
		_repeat = repeat;
		return *this;
	}

	SimpleTask & enable(bool val = true)
	{
		_enabled = val;
		return *this;
	}

	SimpleTask & reset()
	{
		_lastcalled = millis();
		return *this;
	}

private:

	taskerCb_t  _cb;
	bool _repeat{false};
	uint32_t _timeout{0};
	uint32_t _lastcalled{0};
	bool _enabled{true};

};




// #ifdef ESP8266

// class ScheduledTask : public Task
// {

// public:

// 	using Task::taskerCb_t;
// 	ScheduledTask (taskerCb_t cb, bool useMicros = false) : Task(cb,useMicros)
// 	{
// 		//Serial.printf("  Task created @%p\n", this);
// 		_cb = cb;
// 		_lastcalled = _getTime();
// 	}


// private:

// 	taskerCb_t  _cb;
// 	bool _repeat{false};
// 	uint32_t _timeout{0};
// 	uint32_t _lastcalled{0};
// 	bool _enabled{true};

// };

// #endif //ESP8266


#endif
