


/*
SticilFace

General Task Manager....

Priority!  not sure how to do this...

0 = always run every loop




*/

#include <Arduino.h>
#include <functional>
#include <list>
#include <memory>

class Task;


typedef std::function<void(Task&)> taskerCb_t;

class Task {
public:
	virtual Task & setTimeout(uint32_t timeout) { return *this; } 
	virtual Task & setRepeat(bool repeat = true) { return *this; }
	virtual Task & setPriority(uint8_t pr) { return *this; }
	virtual uint8_t getPriority() {} 
	virtual Task & enable() { return *this; }
	virtual Task & disable() { return *this; } 
	virtual Task & setName(const char * name) { return *this; } 
	virtual const char * name() { return nullptr ;}


private:

};

class msTask : public Task
{

public:

	msTask (taskerCb_t cb)
	{
		Serial.printf("Task created @%p\n", this);
		_cb = cb;
		_lastcalled = millis();
	}

	~msTask()
	{
	}

	bool run();

	Task & setTimeout(uint32_t timeout) override
	{
		_timeout = timeout;
		return *this;
	}

	Task & setRepeat(bool repeat = true) override
	{
		_repeat = repeat;
		return *this;
	}

	Task & setPriority(uint8_t pr) override
	{
		_priority = pr;
		return *this;
	}

	uint8_t getPriority() override
	{
		return _priority;
	}

	Task & enable() override 
	{
		_enabled = true;
		return *this;
	}

	Task & disable() override
	{
		_enabled = false;
		return *this;
	}

	Task & setName(const char * name) override
	{
		_name = name;
		return *this;
	}

	const char * name() override
	{
		return _name;
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

public:
	//  times...
	uint32_t _cycles{0};
	uint32_t _micros{0};
	uint32_t _millis{0};
	uint32_t _count2{0};

};


template<typename T>
class ASync
{
public:

	typedef std::unique_ptr<T> task_t;
	typedef std::list<task_t> taskList_t;

	void loop()
	{
		uint32_t startTime = micros();

		//_loopTime = ( (_loopTime * _loopCounter++) + (startTime - _startTime)) / _loopCounter;

		//_startTime = startTime;

		if ( _it == _list.end() || _bailOut == false ) {
			_it = _list.begin();
		}

		//  work on allocation of time
		while ( _it != _list.end()   ) {

			bool todelete = (*_it)->run();

			if (todelete) {
				_it = _list.erase(_it);
			} else {
				_it++;
			}

			// if (_maxWait && micros() - startTime > _maxWait) {
			// 	_bailOut = true;
			// 	Serial.println("Task Exit: Timeout Reached");
			// 	return;
			// }

			// if ( _it == _list.end()) {
			// 	_bailOut = false;
			// }
		}

	}




	 taskList_t _list;
	 typename taskList_t::iterator _it;

	private:

		bool _bailOut{false}; 
};

template<typename T>
class Sync
{

public:

	typedef std::unique_ptr<T> task_t;
	typedef std::list<task_t> taskList_t;

	void loop()
	{
		T a([](Task & t) {

		});
	}

	T & next() {

	}
	 taskList_t _list;
	typename taskList_t::iterator _it;
};

template <class T, class M>
class Tasker: public M
{
public:


	Tasker() {};
	~Tasker() {};

	//T & add(taskerCb_t Cb);

	//bool remove(T & _task);

	//void loop();

	void enablePriority(bool value, uint32_t maxWait = 0)
	{
		_enablePriority = value;
		_maxWait = maxWait;
	}

	//void debugOut(Stream & stream);

	void setMaxWaitus(uint32_t wait)
	{
		_maxWait = wait;
	}

	uint32_t _loopCycles{0};

	T & add(taskerCb_t Cb)
	{
		M::_list.push_back(typename M::task_t(new T(Cb)));
		M::_it = M::_list.begin();
		T & ret = *M::_list.back();
		_sort();
		return ret;
	}

	// void loop()
	// {
	// 	 M::loop();
	// }

	// void loop()
	// {
	// 	uint32_t startTime = micros();

	// 	_loopTime = ( (_loopTime * _loopCounter++) + (startTime - _startTime)) / _loopCounter;

	// 	_startTime = startTime;

	// 	if ( _it == _list.end() || _bailOut == false ) {
	// 		_it = _list.begin();
	// 	}

	// 	//  work on allocation of time
	// 	while ( _it != _list.end()   ) {

	// 		bool todelete = (*_it)->run();

	// 		if (todelete) {
	// 			_it = _list.erase(_it);
	// 		} else {
	// 			_it++;
	// 		}

	// 		if (_maxWait && micros() - startTime > _maxWait) {
	// 			_bailOut = true;
	// 			Serial.println("Task Exit: Timeout Reached");
	// 			return;
	// 		}

	// 		if ( _it == _list.end()) {
	// 			_bailOut = false;
	// 		}
	// 	}

	// }

	bool remove(T & _task)
	{
		typename M::taskList_t::iterator it;

		for (it = M::_list.begin(); it != M::_list.end(); /*  leave empty */ ) {

			if (&_task == it->get()) {
				it = M::_list.erase(it);
			} else {
				it++;
			}
		}
	}

	void debugOut(Stream & stream)
	{

		stream.println("\n*****  Tasker *****  ");
		stream.printf("Heap: %u\n", ESP.getFreeHeap());
		stream.printf("Tasks Running :%u\n", M::_list.size());
		stream.printf("us Per Loop :%uus = %u cycles\n", _loopTime, microsecondsToClockCycles(_loopTime));

		typename M::taskList_t::iterator it;
		uint16_t index = 0;

		for (it = M::_list.begin(); it != M::_list.end(); it++ ) {
			index++;
			T & t = **it;
			stream.printf("[%s][%2u@%p] pr %u\tRun %3u\tAv Cycles [%u] \tAv micros [%u]\n", (t.name()) ? t.name() : "null" , index, &t, t.getPriority(), t.count , t._cycles, t._micros );

		}
		stream.println("\n*****   END   *****\n  ");

	}


	void _sort()
	{
		M::_list.sort( [](const typename M::task_t& lhs, const typename M::task_t & rhs) {

			return ( lhs->getPriority() < rhs->getPriority() );

		});
	}


private:

	//void _sort();


	uint32_t _startTime{0};
	uint32_t _loopTime{0};
	uint32_t _loopCounter{0};
	uint32_t _maxWait{0};
	bool _enablePriority{false};
	
	bool _bailOut{false};

};