


#include <Arduino.h>
#include <functional>
#include <list>
#include <memory>


class usTask 
{

public:

	typedef std::function<void(usTask&)> taskerCb_t;


	usTask (taskerCb_t cb)
	{
		//Serial.printf("Task created @%p\n", this);
		_cb = cb;
		//_lastcalled = micros();
	}

	~usTask()
	{
	}

	bool run(const uint8_t priority);

	usTask & setTimeout(uint32_t timeout) 
	{
		_timeout = timeout;
		return *this;
	}

	usTask & setRepeat(bool repeat = true)
	{
		_repeat = repeat;
		return *this;
	}

	usTask & setPriority(uint8_t pr) 
	{
		_priority = pr;
		return *this;
	}

	uint8_t getPriority() 
	{
		return _priority;
	}

	usTask & enable()  
	{
		_enabled = true;
		return *this;
	}

	usTask & disable() 
	{
		_enabled = false;
		return *this;
	}

	usTask & setName(const char * name) 
	{
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

// public:
// 	//  times...
// 	uint32_t _cycles{0};
// 	uint32_t _micros{0};
// 	uint32_t _millis{0};
// 	uint32_t _count2{0};

};