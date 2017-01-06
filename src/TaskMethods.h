
#pragma once

#ifndef MY_TASKER_METHODS_H
#define MY_TASKER_METHODS_H

#include <Arduino.h>
#include <functional>
#include <list>
#include <memory>



template<typename T>
class ASync
{
public:

	typedef std::unique_ptr<T> task_t;
	typedef std::list<task_t> taskList_t;
	typedef std::function<void(void)> EmptyCb_t;

	void loop()
	{
		uint32_t startTime = micros();

		if (_list.size() == 0) {
			return;
		}

		typename taskList_t::iterator it;

		if (_it != _list.end()) {
			it = _it;
		} else {
			it = _list.begin();
		}

		for ( /*   */ ; it != _list.end(); /*    */ ) {

			bool todelete = false; 

			if (*_it) {
				//Serial.printf("%p\n", it->get());
				todelete = (*it)->run(_currentPriority);
			}

			if (todelete) {
				//Serial.printf(" Function Finished: @%p (%s)\n", &*it, ((*it)->name())? (*it)->name() : "null" );
				it = _list.erase(it);
			} else {
				it++;
			}

			if (_maxWait && micros() - startTime > _maxWait) {
				//Serial.println("Task Exit: Timeout Reached");
				_it = it++;
				return;
			}

		}

		_it = _list.end();


		// while ( _it != _list.end()   ) {

		// 	bool todelete = false;
		// 	todelete = (*_it)->run(_currentPriority);

		// 	if (todelete) {
		// 		Serial.printf(" Function Finished: @%p (%s)\n", &*_it, ((*_it)->name())? (*_it)->name() : "null" );
		// 		_it = _list.erase(_it);



		// 	} else {
		// 		_it++;
		// 	}

		// 	if (_maxWait && micros() - startTime > _maxWait) {
		// 		_bailOut = true;
		// 		//Serial.println("Task Exit: Timeout Reached");
		// 		return;
		// 	}

		// 	if ( _it == _list.end()) {
		// 		_bailOut = false;
		// 	}
		// }

	}

	void sort()
	{
		_list.sort( [](const task_t& lhs, const task_t & rhs) {

			return ( lhs->getPriority() < rhs->getPriority() );

		});
	}

	// void enablePriority(bool value, uint32_t maxWait = 0)
	// {
	// 	_enablePriority = value;
	// 	_maxWait = maxWait;
	// }

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

	// void SetEmptyFn(EmptyCb_t Fn) {
	// 	listEmptyFn = Fn;
	// }

	// void clearList() {

	// 	typename taskList_t::iterator ptr; 

	// 	for (ptr = _list.begin(); ptr != _list.end(); ) {
	// 		ptr = _list.erase(ptr);
	// 	}
	// }



	taskList_t _list;


	typename taskList_t::iterator _it;

	bool listEmptyFnFlag{true};

protected:
	//EmptyCb_t listEmptyFn;

private:

	bool _bailOut{false};
	uint32_t _maxWait{0};
	//bool _enablePriority{false};
	uint8_t _currentPriority{0};


};



/*


	Runs each one in sequence....


*/
template<typename T>
class Sync
{

public:

	typedef std::unique_ptr<T> task_t;
	typedef std::list<task_t> taskList_t;
	typedef std::function<void(void)> EmptyCb_t;

	void loop()
	{

		if (_list.size() == 0) {
			return;
		}


		if (_enable && _it != _list.end()) {

			bool finished = false; 

			if (*_it) {
				finished = (*_it)->run(_currentPriority);
			}

			if (finished) {

				if (!_repeat) {
					_it = _list.erase(_it); //  remove task once done, if not repeating!
				} else {
					_it++;
				}

				(*_it)->reset(); //  this is important as it resets the start time for the next sequential task

				if (_repeat && _list.size() && _it == _list.end()) {
					_it = _list.begin();

					typename taskList_t::iterator it;

					for (it = _list.begin(); it != _list.end(); ++it ) {

						(*it)->reset();

					}
				}
			}
		}

	}

	void enable(bool enable)
	{
		_enable = enable;
	}

	void repeat(bool repeat)
	{
		_repeat = repeat;
	}

	void sort() {} //  empty function...

	// void clearList() {

	// 	typename taskList_t::iterator ptr; 

	// 	for (ptr = _list.begin(); ptr != _list.end(); ) {
	// 		ptr = _list.erase(ptr);
	// 	}
	// }



	//bool listEmptyFnFlag{true};

protected:

	taskList_t _list;
	typename taskList_t::iterator _it;
	//EmptyCb_t listEmptyFn;

private:
	bool _enable{true};
	bool _repeat{false};
	uint8_t _currentPriority{0};


};


#endif
