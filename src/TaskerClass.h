
#pragma once

#ifndef MY_TASKER_CLASS_H
#define MY_TASKER_CLASS_H

#include <Arduino.h>
#include <functional>
#include <list>
#include <memory>



template <class TaskType, class LoopMethod>
class Tasker: public LoopMethod
{
public:
	typedef LoopMethod loop_m;
	typedef TaskType task_t;
	//typedef std::shared_ptr<TaskerType> tasker_p;
	typedef std::function<void(TaskType&)> taskerCb_t;


	static uint8_t tasker_id;

	Tasker() {};

	~Tasker()
	{

		Serial.printf("~Tasker() %p\n", this);
		clearList(); 

	};

	TaskType & add(taskerCb_t Cb, bool useMicros = false)
	{

		LoopMethod::_list.push_back(typename LoopMethod::task_t(new TaskType(Cb, useMicros)));
		LoopMethod::_it = LoopMethod::_list.begin();


		// TaskType * ret = &*LoopMethod::_list.back();

		//Serial.printf("Added Task %p\n", ret);

		return *LoopMethod::_list.back();

	}

	TaskType & addBefore(taskerCb_t Cb, bool useMicros = false)
	{



	}

	TaskType & addAfter(taskerCb_t Cb, bool useMicros = false)
	{



	}

	bool goToTask(TaskType & task, bool runImmediately = false) {

	typename LoopMethod::taskList_t::iterator it;
		for (it = LoopMethod::_list.begin(); it != LoopMethod::_list.end(); ++it ) {
			if (&task == it->get()) {	
				LoopMethod::_it == it  ;
				if (runImmediately) {
					it->run(0, true); 
				}
				return true;
			} 
		}

		return false; 
	}


	bool remove(TaskType & task)
	{
		debugOut(Serial);

		typename LoopMethod::taskList_t::iterator it;

		for (it = LoopMethod::_list.begin(); it != LoopMethod::_list.end(); ++it ) {

			if (&task == it->get()) {	

				LoopMethod::_list.erase(it); 

				if (LoopMethod::_it == it && LoopMethod::_it != LoopMethod::_list.end()) {	
					LoopMethod::_it++;
				} 

				return true;
			} 
		}

		Serial.println("task not found");
		return false;
	}

	void clearList()
	{
		LoopMethod::_list.clear();
	}

	void debugOut(Stream & stream)
	{

		stream.printf("\n*****  Tasker %p *****  ", this);
		stream.printf("Heap: %u\n", ESP.getFreeHeap());
		stream.printf("Tasks Running :%u\n", LoopMethod::_list.size());
		//stream.printf("us Per Loop :%uus = %u cycles\n", _loopTime, microsecondsToClockCycles(_loopTime));

		typename LoopMethod::taskList_t::iterator it;
		uint16_t index = 0;

		for (it = LoopMethod::_list.begin(); it != LoopMethod::_list.end(); it++ ) {
			index++;
			TaskType & t = **it;
			stream.printf("[%s][%2u@%p] pr %u\tRun %3u\t\n", (t.name()) ? t.name() : "null" , index, &t, t.getPriority(), t.count  );

		}
		stream.println("\n*****   END   *****\n  ");

	}

	// template<class TaskerType>
	// TaskerType & addSubTasker (bool deleteAfter = true)
	// {

	// 	TaskerType * ptr = new TaskerType;  // create new pointer to tasker type...  sync or async...

	// 	task_t & task = this->add( [ptr] (task_t & t) {  // add the loop to it...

	// 		ptr->loop();

	// 	}).setRepeat(true);

	// 	if (deleteAfter) {
	// 		ptr->SetEmptyFn( [&task, this, ptr] () {
	// 			this->remove(task);
	// 			delete ptr;
	// 		});
	// 	}

	// 	Serial.printf("[addSubTasker] Creating SubTask : %p = addr in parent tasker\n", &task);

	// 	return *ptr;

	// }

	template<class TaskerType>
	std::shared_ptr<TaskerType> addSubTasker (bool doNotDelete = false)
	{

		std::shared_ptr<TaskerType> ptr = std::make_shared<TaskerType>();  // create new smart pointer to tasker type...  sync or async...

		typename TaskerType::task_t & task =  this->add( [ptr, this, doNotDelete] (task_t & t) {  // add the loop to it...
			
			//  this checks the count... if list is empty, no one is hanging onto the pointer for use later, and it is empty... then delete... 
			//  But only if the tasker is created with that in mind!!!!
			if (!doNotDelete && ptr.unique() && ptr->isEmpty()){

				//Serial.printf("LAMBDA: Removing %p from tasker %p\n", &t, this);
				//this->remove(t); //  can't remove this as YOU ARE IN IT...
				t.setRepeat(false);
				//Serial.printf("LAMBDA: REturned \n");

				return; 
			}
			ptr->loop();

		}).setRepeat(true);

		return ptr;

	}

	bool isEmpty()
	{

		return (LoopMethod::_list.size() == 0);
	}



#endif



// private:

	// uint32_t _startTime{0};
	// uint32_t _loopTime{0};
	// uint32_t _loopCounter{0};
//	uint32_t _maxWait{0};

	//task_t * _parentLoopTask{nullptr};


};