
#pragma once

#ifndef MY_TASKER_CLASS_H
#define MY_TASKER_CLASS_H

#include <Arduino.h>
#include <functional>
#include <list>
#include <memory>

class TaskerBase {
public:
	virtual ~TaskerBase() {} 


};

template <class TaskType, class LoopMethod>
class Tasker: public LoopMethod, public TaskerBase
{
public:
	typedef LoopMethod loop_m;
	typedef TaskType task_t;
	typedef std::weak_ptr<TaskerBase> baseListPtrType_t;
	typedef std::list<baseListPtrType_t> baseList_t;
	//typedef std::shared_ptr<TaskerType> tasker_p;
	typedef std::function<void(TaskType&)> taskerCb_t;


	static uint8_t tasker_id;

	Tasker() {};

	~Tasker()
	{

		DEBUG_TASKERf("~Tasker() %p\n", this);
		LoopMethod::_list.clear(); 

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

		Serial.printf("task not found\n");
		return false;
	}

	//  not a good function to have..  need to call onEnd for all tasks...  and will leave dangling sub tasks..  have to take ownership of weak pointers.. 
	// void _clearList()
	// {
	// 	LoopMethod::_list.clear();
	// }

	void debugOut(Stream & stream)
	{

		stream.printf("\n*****  Tasker %p *****  ", this);
		stream.printf("Heap: %u\n", ESP.getFreeHeap());
		stream.printf("Tasks Running :%u\n", LoopMethod::_list.size());
		stream.printf("SubLoops Running :%u\n", _baseList.size());
		//stream.printf("us Per Loop :%uus = %u cycles\n", _loopTime,rt microsecondsToClockCycles(_loopTime));

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

		baseListPtrType_t weak_ptr = baseListPtrType_t(ptr); 

		_baseList.push_back( weak_ptr );

		typename TaskerType::task_t & task =  this->add( [ptr, this, doNotDelete, weak_ptr ] (task_t & t) {  // add the loop to it...
			
			//  this checks the count... if list is empty, no one is hanging onto the pointer for use later, and it is empty... then delete... 
			//  But only if the tasker is created with that in mind!!!!
			
			ptr->loop();


			if (!doNotDelete && ptr.unique() && ptr->isEmpty()){

				DEBUG_TASKERf("LAMBDA: Removing %p from tasker %p\n", &t, this);
				//this->remove(t); //  can't remove this as YOU ARE IN IT...
				t.setRepeat(false);
				this->removeSubLoop(weak_ptr);

				//Serial.printf("LAMBDA: REturned \n");

				return; 
			}
			

		}).setRepeat(true);

		DEBUG_TASKERf("SUB TASK loop() resides in task %p in tasker %p \n", &task, this); 

		return ptr;

	}

	bool isEmpty()
	{
		return (LoopMethod::_list.size() == 0);
	}

	void removeSubLoop(baseListPtrType_t const & wp) {
		
		_baseList.remove_if(  [&wp]( baseListPtrType_t p){

			if ( wp.lock() == p.lock() ) {
				Serial.printf("removeSubLoop called and returned TRUE\n"); 
				return true;		
			} else {
				return false; 
			}


		}); 


	}



 private:

 		baseList_t _baseList; 
	// uint32_t _startTime{0};
	// uint32_t _loopTime{0};
	// uint32_t _loopCounter{0};
//	uint32_t _maxWait{0};

	//task_t * _parentLoopTask{nullptr};


};







#endif


