
#pragma once

#ifndef MY_TASKER_METHODS_H
#define MY_TASKER_METHODS_H

#include <Arduino.h>
#include <functional>
#include <list>
#include <memory>


class baseMethods
{
public:
	virtual ~baseMethods() {}
	virtual void printprechar(Stream & stream){}

};

class TaskerBase;



template<typename T>
class ASync : public baseMethods
{
public:

	typedef std::unique_ptr<T> taskStorage_t;

	typedef TaskerBase * subTaskStorage_t;

	typedef std::pair<taskStorage_t, subTaskStorage_t > taskPairStorage_t;
	typedef std::list<taskPairStorage_t> taskList_t;  //  list now stores std:pair...
	typedef std::function<void(void)> EmptyCb_t;
	typedef std::function<void(T&)> taskerCb_t;


	void loop()
	{

		uint32_t startTime = micros();
		
		// Serial.print("List Size = ");
		// Serial.println(_list.size());

		if (_list.size() == 0) {// return if there is nothing to run
			return;
		}

		//  if its the first run then reset all the tasks to start from the loop time. not the time they were created! 
		if (!_hasRun) {
			typename taskList_t::iterator it;
			for (it = _list.begin(); it != _list.end(); ++it) {
				it->first->reset(); 
			}
			_hasRun = true; 
		}

		typename taskList_t::iterator it;

		if (_it != _list.end()) {
			it = _it;
		} else {
			it = _list.begin();
		}

		for ( /*   */ ; it != _list.end(); /*    */ ) {

			bool todelete = false;

			if (it->first) {
				//Serial.printf("%p\n", it->get());
				todelete = (it->first)->run(_currentPriority);
			}

			if (todelete) {
				//Serial.printf("ASYNC Finished: @%p (%s)  SubTaskptr(%p)\n", it->first.get(), (it->first->name())? it->first->name() : "null", it->second );
				
				if (it->second) {
					//Serial.printf("ASYNC SubTasker Loop Found: Calling Delete %p \n", it->second);
					delete it->second;
					it->second = nullptr;
				}

				it = _list.erase(it);

				if (_list.size() == 0 ) { return; }

			} else {
				it++;
			}

			if (_maxWait && micros() - startTime > _maxWait) {
				//Serial.println("Task Exit: Timeout Reached");
				_it = ++it;
				return;
			}

		}

		_it = _list.end();

	}

	void setPriority(uint8_t priority)
	{
		_currentPriority = priority;
	}

	uint8_t getPriority()
	{
		return _currentPriority;
	}

	//void debugOut(Stream & stream);

	void setMaxWaitus(uint32_t wait)
	{
		_maxWait = wait;
	}


	void dumpTask( T & t , Stream & stream, uint & position , uint index)
	{

		for (uint8_t i = 0; i < position + 2 ; ++i) {  stream.printf(" "); }

		stream.printf("%u. %p [%s][%s] priority %u, timeout = %u, runcount = %u\n", index , &t, (t.running()) ? "Y" : "N" , (t.name()) ? t.name() : "null" , t.getPriority(), t.getTimeout() ,t.count  );

	}

	void printprechar(Stream & stream) {
		stream.print(""); 
	}

	//void setRepeat(bool a) {};


	taskList_t _list;
	typename taskList_t::iterator _it;
	bool listEmptyFnFlag{true};

protected:

private:

	//bool _bailOut{false};
	uint32_t _maxWait{0};
	uint8_t _currentPriority{0};
	bool _hasRun{false}; 


};



/*


	Runs each one in sequence....


*/
template<typename T>
class Sync : public baseMethods
{

public:

	typedef std::unique_ptr<T> taskStorage_t;

	typedef TaskerBase * subTaskStorage_t;

	typedef std::pair<taskStorage_t, subTaskStorage_t > taskPairStorage_t;
	typedef std::list<taskPairStorage_t> taskList_t;  //  list now stores std:pair...
	typedef std::function<void(void)> EmptyCb_t;
	typedef std::function<void(T&)> taskerCb_t;


	void loop()
	{

		if (_list.size() == 0) {
			return;
		}


		if (_enable && _it != _list.end()) {

			bool finished = false;

			if (_it->first) {
				finished = _it->first->run(_currentPriority);
			}

			if (finished) {

				if (!_repeat) {
					//Serial.printf("SYNC Task Finished: @%p (%s) [%p]\n", _it->first.get(), (_it->first->name())? _it->first->name() : "null", _it->second  );
					
					if (_it->second) {

						delete _it->second;
					}

					_it = _list.erase(_it); //  remove task once done, if not repeating!

					if (_list.size() == 0 ) { return; }

				} else {
					++_it;
				}

				if (_it == _list.end()) {
					_it = _list.begin(); 
				}


				_it->first->reset(); //  this is important as it resets the start time for the next sequential task

				if (_repeat && _list.size() && _it == _list.end()) {
					_it = _list.begin();

					typename taskList_t::iterator it;

					for (it = _list.begin(); it != _list.end(); ++it ) {

						it->first->reset();

					}
				}
			}
		}

	}

	void enable(bool enable)
	{
		_enable = enable;
	}

	void setRepeat(bool repeat)
	{
		_repeat = repeat;
	}

	void sort() {} //  empty function...


	void dumpTask( T & t , Stream & stream, uint & position, uint index)
	{

		position++; 
		for (uint8_t i = 0; i < position; ++i) {  stream.printf(" "); }
		stream.printf("└ %u. %p [%s][%s] timeout = %u, runcount = %u\n", index , &t, (t.running()) ? "Y" : "N" , (t.name()) ? t.name() : "null", t.getTimeout(), t.count );

	}

	void printprechar(Stream & stream) {
		stream.print("└ "); 
	}


protected:

	taskList_t _list;
	typename taskList_t::iterator _it;

private:
	bool _enable{true};
	bool _repeat{false};
	uint8_t _currentPriority{0};


};


#endif
