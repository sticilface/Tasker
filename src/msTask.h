


#include <Arduino.h>
#include <functional>
#include <list>
#include <memory>


class Task 
{

public:

	typedef std::function<void(Task&)> taskerCb_t;


	Task (taskerCb_t cb)
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

	bool run(const uint8_t priority);

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
		//Serial.printf("  name = %s\n", name);
		_name = name;
		return *this;
	}

	const char * name() 
	{
		return _name;
	}

	void reset() {
		_state = -1;  
	}

	Task & onEnd(std::function<void(void)> Cb) {
		_onEnd = Cb; 
		return *this; 
	}

	uint32_t count{0};
	bool hasRun{false};
	uint32_t lastRunTime{0};

private:
	taskerCb_t _cb;
	bool _repeat{false};
	uint32_t _timeout{0};
	uint32_t _lastcalled{0};
	bool _enabled{true};
	uint8_t _priority{0};
	const char * _name{nullptr};
	int8_t _state{-1}; 
	std::function<void(void)> _onEnd;  

public:
	//  times...
	uint32_t _cycles{0};
	uint32_t _micros{0};
	uint32_t _millis{0};
	uint32_t _count2{0};

};