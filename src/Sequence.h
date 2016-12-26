#pragma once


#ifndef MY_TASKER_SEQUENCE_H
#define MY_TASKER_SEQUENCE_H


#include <Arduino.h>
#include <functional>
#include <list>
#include <memory>


class Sequence
{

public:
	
	typedef std::function<void(Sequence&)> taskerCb_t;


	Sequence (taskerCb_t cb, bool useMicros = false) : _useMicros(useMicros)
	{
		//Serial.printf("  Task created @%p\n", this);
		_cb = cb;	
	}

	~Sequence()
	{
		//Serial.printf("  Task deleted @%p (%s)\n", this, (_name)? _name : "null" );	
		if (_onEnd) {
			_onEnd();
		}
	}

	Sequence & setTimeout(uint32_t timeout) 
	{
		_timeout = timeout;
		return *this;
	}

	Sequence & setRepeat(bool repeat = true)
	{
		_repeat = repeat;
		return *this;
	}

	bool run(const uint8_t priority )
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
	
	Sequence & onEnd(std::function<void(void)> Cb) {
		_onEnd = Cb; 
		return *this; 
	}

	bool finished{false}; 

	inline uint32_t getTime() {
		if (_useMicros) {
			return micros();
		} else {
			return millis(); 
		}
	}


private:

	std::function<void(void)> _onEnd; 
	taskerCb_t _cb; 
	uint32_t _lastcalled{0};
	uint32_t _timeout{0};
	bool _enabled{true};
	int8_t _state{-1}; 
	bool _repeat{false}; 
	const bool _useMicros{false}; 

};

#endif

