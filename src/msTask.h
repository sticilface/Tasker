
#pragma once


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
	}

	~Task()
	{
		//Serial.printf("  Task deleted @%p (%s)\n", this, (_name)? _name : "null" );	
		if (_onEnd) {
			_onEnd();
		}
	}

	bool run(const uint8_t priority = 0);

	void runCb() {
		_lastcalled = _getTime();
		if (_cb) {
			_cb(); 
		}
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
		return (_name)? _name : "null" ;
	}

	void reset() {
		_state = -1;  
	}

	Task & onEnd(std::function<void(void)> Cb) {
		_onEnd = Cb; 
		return *this; 
	}

	Task & setPersistent(bool donotdelete) {
		_doNotDelete = donotdelete; 
		return *this; 
	}


	uint32_t count{0};
	bool finished{false}; 

private:
	
	inline uint32_t _getTime() {
		return (_useMicros)? micros() : millis(); 
	}

	taskerCb_t _cb;
	bool _repeat{false};
	uint32_t _timeout{0};
	uint32_t _lastcalled{0};
	bool _enabled{true};
	uint8_t _priority{0};
	const char * _name{nullptr};
	int8_t _state{-1}; 
	std::function<void(void)> _onEnd;  
	const bool _useMicros{false}; 
	bool _doNotDelete{false}; 

};