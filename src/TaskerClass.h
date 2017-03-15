
#pragma once

#ifndef MY_TASKER_CLASS_H
#define MY_TASKER_CLASS_H

#include <Arduino.h>
#include <functional>
#include <list>
#include <memory>

class TaskerBase
{
public:
	virtual ~TaskerBase() {}
	virtual void debugOut(Stream & stream, uint indexNum = 0 , uint indentlevel = 0) {};
	virtual void test() {}
};

class TaskBase;


template <class TaskType, class LoopMethod>
class Tasker: public LoopMethod, public TaskerBase
{
public:
	typedef LoopMethod loop_m;
	typedef TaskType task_t;

	using  LoopMethod::_it;
	using  LoopMethod::_list;

	using   LoopMethod::dumpTask;
	using   LoopMethod::baseMethods::printprechar;

	using typename LoopMethod::taskerCb_t;
	using typename LoopMethod::subTaskStorage_t;
	using typename LoopMethod::taskList_t;
	using typename LoopMethod::taskStorage_t;
	using typename LoopMethod::taskPairStorage_t;

	//static uint8_t tasker_id;

	Tasker()
	{
		_it = _list.begin();
	};

	~Tasker()
	{
		//Serial.printf("Deleting Tasker: %p\n", this);
		typename LoopMethod::taskList_t::iterator it;
		for (it = _list.begin(); it != _list.end(); ++it ) {
			if (it->second) {
				delete it->second;
				it->second = nullptr;
			}
		}
	};

	void clear(TaskType * task = nullptr) //  clears all except this one... 
	{
		typename LoopMethod::taskList_t::iterator it;
		for (it = _list.begin(); it != _list.end();  ) {

			if (task && it->first.get() == task) {
				++it;
				continue; //
			} else {
				if (it->second) {
					delete it->second;
					it->second = nullptr;
				}

				it = _list.erase(it);
			}
		}
	}

	TaskType & add(taskerCb_t Cb, bool useMicros = false )
	{
		return _add(Cb, nullptr , useMicros);
	}


	//  not in use yet
	TaskType & addBefore(taskerCb_t Cb, bool useMicros = false)
	{



	}

	// not in use yet....
	TaskType & addAfter(taskerCb_t Cb, bool useMicros = false)
	{



	}


	bool goToTask(TaskType & task, bool runImmediately = false)
	{

		// typename LoopMethod::taskList_t::iterator it;
		// 	for (it = LoopMethod::_list.begin(); it != LoopMethod::_list.end(); ++it ) {
		// 		if (&task == it->get()) {
		// 			LoopMethod::_it == it  ;
		// 			if (runImmediately) {
		// 				it->run(0, true);
		// 			}
		// 			return true;
		// 		}
		// 	}

		return false;
	}


	bool remove(TaskType & task)
	{
		Serial.println("REMOVE TASK CALLED");
		// debugOut(Serial);

		// typename LoopMethod::taskList_t::iterator it;

		// for (it = LoopMethod::_list.begin(); it != LoopMethod::_list.end(); ++it ) {

		// 	/*
		// 			Might need to add check here to destroy if it is a sub loop...  maybe...
		// 	*/
		// 	if (&task == it->first.get() ) {

		// 		LoopMethod::_list.erase(it);

		// 		if (LoopMethod::_it == it && LoopMethod::_it != LoopMethod::_list.end()) {
		// 			LoopMethod::_it++;
		// 		}

		// 		return true;
		// 	}
		// }

		// Serial.printf("task not found\n");
		return false;
	}

	void debugOut(Stream & stream, uint indexNum = 0, uint position = 0)
	{
		yield(); //  for long outputs...

		const uint indexNum_orig = indexNum;
		const uint position_orig = position + 2;

		if (position == 0) {
			stream.printf("\n*****  Tasker  *****  \n");
			//stream.printf("Heap: %u\n", ESP.getFreeHeap());
		}

		position = position + 2;

		if (position_orig) {

			for (uint8_t i = 0 ; i < position ; i++) { stream.print(" "); }

			if (_parent) { _parent->test(); }

			stream.printf("%u. Tasker: %p - %u Tasks Running" , indexNum_orig  , static_cast<TaskerBase*>(this) , _list.size());

			position = position + 2;

		} else {
			stream.printf("Tasker: %p - %u Tasks Running", static_cast<TaskerBase*>(this) , _list.size());

		}

		if (_parent) {
			Serial.printf(" Parent Tasker %p\n", _parent);
		} else {
			Serial.println();
		}



		typename LoopMethod::taskList_t::iterator it;

		uint index = 0;  // was 0



		for (it = _list.begin(); it != _list.end(); it++ ) {

			index++;

			//uint8_t tempindent = indentlevel;

			if (it->second) {

				//tempindent++;

				auto ptr = it->second;
				//  this means that this task is a nested task manager....
				ptr->debugOut(stream, index, position);



			} else {
				dumpTask( *(it->first.get()) , stream , position, index );
			}

		}

		if (position_orig == 0) {

			stream.println("*****   END   *****\n  ");

		}



	}

	template<class TaskerType>
	TaskerType * addSubTasker (bool doNotDelete = false)
	{

		//std::shared_ptr<TaskerType> ptr = std::make_shared<TaskerType>();  // create new smart pointer to tasker type...  sync or async...

		TaskerType * ptr = new TaskerType;
		typename TaskerType::task_t & task =  this->_add( [ ptr, doNotDelete, this ] (task_t & t) {  // add the loop to it...

			if (!doNotDelete && ptr->isEmpty()) {
				t.setRepeat(false);
				return;
			}
			ptr->loop();
		}, ptr , false).setRepeat(true);

		task.setName("LOOP TASK");
		ptr->setParent(this);
		//return std::weak_ptr<TaskerType>(ptr);
		return ptr;

	}

	bool isEmpty()
	{
		return (LoopMethod::_list.size() == 0);
	}

	void test()
	{
		printprechar(Serial);
	}

	void setParent(TaskerBase * ptr)
	{
		_parent = ptr;
	}

	bool hasTask(const TaskType & task) {
		
		typename LoopMethod::taskList_t::iterator it;

		for (it = _list.begin(); it != _list.end(); ++it ) {
			if (it->first && it->first.get() == &task) {
				return true; 
			}
		}

		return false; 
	}



private:

	TaskType & _add(taskerCb_t Cb, subTaskStorage_t subtasker_p, bool useMicros)
	{

		_list.push_back(
		    taskPairStorage_t(
		        taskStorage_t( new TaskType(Cb, useMicros))
		        , subtasker_p
		    )
		);

		_it = _list.begin();
		TaskType * ret = _list.back().first.get();
		//Serial.printf("Added Task %p, useMicros = %s, SubLoopTasker = %p\n", ret, (useMicros) ? "true" : "false", (subtasker_p) ? subtasker_p : nullptr );
		return *ret;

	}

	TaskType & _add(taskerCb_t Cb, subTaskStorage_t subtasker_p)
	{


		return _add(Cb, subtasker_p, false);

	}

	TaskerBase* _parent{nullptr};



};







#endif


