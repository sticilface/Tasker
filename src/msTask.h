
#pragma once


#ifndef MY_TASKER_TASK_H
#define MY_TASKER_TASK_H


#include <Arduino.h>
#include <functional>
#include <list>
#include <memory>


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
		//Serial.printf("  Task deleted @%p (%s)\n", this, (_name) ? _name : "null" );
		if (_onEnd) {
			_onEnd();
		}
	}

	bool run(const uint8_t priority = 0, const bool override = false)
	{
		if (override || ( _enabled && _priority <= priority)) {

			if (override || (_getTime() - _lastcalled > _timeout)) {

				count++;

				if (_cb) {
					//if (count < 3 ) { Serial.printf("Run 0x%p (%s)\n", this, (_name)? _name : "null" ); }
					_cb(*this);

					if (_repeat) {
						_lastcalled = _getTime();
						if (_repeatNo == count) {
							return !_doNotDelete;
						}

					} else {

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

	Task & setTimeout(uint32_t timeout)
	{
		_timeout = timeout;
		return *this;
	}

	Task & setRepeat(bool repeat = true)
	{
		_repeat = repeat;
		return *this;
	}

	Task & setRepeat(int number)
	{
		_repeatNo = number;

		if (number > 0) {
			_repeat = true;
		} else {
			_repeat = false;
		}
		return *this;
	}

	Task & setPriority(uint8_t pr)
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
		_name = name;
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


	uint32_t count{0};
	//bool finished{false};

private:

	inline uint32_t _getTime()
	{
		return (_useMicros) ? micros() : millis();
	}

	taskerCb_t  _cb;
	bool _repeat{false};
	int _repeatNo{ -1};
	uint32_t _timeout{0};
	uint32_t _lastcalled{0};
	bool _enabled{true};
	uint8_t _priority{0};
	const char * _name{nullptr};
	//int8_t _state{ -1};
	std::function<void(void)> _onEnd;
	const bool _useMicros{false};
	bool _doNotDelete{false};

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

	SimpleTask & reset() {
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


#endif
