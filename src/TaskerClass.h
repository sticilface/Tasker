
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

	Tasker() {

		_it = _list.begin();
	};

	~Tasker()
	{

		//DEBUG_TASKERf("~Tasker() %p\n", this);
	
		//_list.clear();

	};

	TaskType & add(taskerCb_t Cb, bool useMicros = false )
	{

		return _add(Cb, nullptr , useMicros);

	}



	TaskType & addBefore(taskerCb_t Cb, bool useMicros = false)
	{



	}

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

	//  not a good function to have..  need to call onEnd for all tasks...  and will leave dangling sub tasks..  have to take ownership of weak pointers..
	// void _clearList()
	// {
	// 	LoopMethod::_list.clear();
	// }

	void debugOut(Stream & stream, uint indexNum = 0, uint position = 0)
	{
		yield(); //  for long outputs...

		const uint indexNum_orig = indexNum; 
		const uint position_orig = position; 

		if (position == 0) {
			stream.printf("\n*****  Tasker  *****  \n");
			//stream.printf("Heap: %u\n", ESP.getFreeHeap());
		}

		position = position + 2; 

		if (position_orig) {

			for (uint8_t i =0 ; i < position ; i++) { stream.print(" "); }

			if (_parent) { _parent->test(); }
		    
		    stream.printf("%u. Tasker: %p - %u Tasks Running" ,indexNum_orig  , static_cast<TaskerBase*>(this) , _list.size());

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
		
		uint index = 0;

		for (it = _list.begin(); it != _list.end(); it++ ) {		

			index++;

			//uint8_t tempindent = indentlevel; 

			if (it->second) {
				
				//tempindent++;

				auto ptr = it->second;
				//  this means that this task is a nested task manager.... 
				ptr->debugOut(stream, index, position);



			} else {
				dumpTask( *(it->first.get()) ,stream , position , index );
			}

		}

		if (position_orig == 0) {

			stream.println("*****   END   *****\n  ");

		}

		

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
	TaskerType * addSubTasker (bool doNotDelete = false)
	{

		//std::shared_ptr<TaskerType> ptr = std::make_shared<TaskerType>();  // create new smart pointer to tasker type...  sync or async...

		//std::unique_ptr<TaskerType> ptr = std::make_unique<TaskerType>();  // create new smart pointer to tasker type...  sync or async...

		TaskerType * ptr = new TaskerType; 


		typename TaskerType::task_t & task =  this->_add( [ ptr, doNotDelete, this ] (task_t & t) {  // add the loop to it...

			//  this checks the count... if list is empty, no one is hanging onto the pointer for use later, and it is empty... then delete...
			//  But only if the tasker is created with that in mind!!!!

			if (!doNotDelete && ptr->isEmpty()) {

//				DEBUG_TASKERf("LOOP LAMBDA: Tasker %p is now empty %p setRepeat(false) \n", this , &t);
				//this->remove(t); //  can't remove this as YOU ARE IN IT...
				t.setRepeat(false);

				//delete ptr; 

				//this->removeSubLoop(weak_ptr);

				//Serial.printf("LAMBDA: REturned \n");

				return;
			}
			// } else if ( ptr->isEmpty() ) {
			// 	Serial.printf("LAMBDA: Unable to delete: doNotDelete = %s, unique = %s, ptrcount = %u, isEmpty = %s\n", 
			// 		(doNotDelete)? "true" : "false", (ptr.unique())? "true" : "false", ptr.use_count(), (ptr->isEmpty())? "true" : "false",&t);
			// 	delay(10000);
			// }



			ptr->loop();


		}, ptr , false).setRepeat(true);

		task.setName("LOOP TASK");

		ptr->setParent(this); 

	//	DEBUG_TASKERf("SUB TASK loop() for tasker %p resides in task %p in tasker %p cast to %p\n", ptr ,&task, this, static_cast<TaskerBase*>(this));

		//return std::weak_ptr<TaskerType>(ptr);
		return ptr; 

	}

	bool isEmpty()
	{
		return (LoopMethod::_list.size() == 0);
	}

	void test() {
		printprechar(Serial); 
	}

	// void removeSubLoop(baseListPtrType_t const & wp) {

	// 	_baseList.remove_if(  [&wp]( baseListPtrType_t p){

	// 		if ( wp.lock() == p.lock() ) {
	// 			Serial.printf("removeSubLoop called and returned TRUE\n");
	// 			return true;
	// 		} else {
	// 			return false;
	// 		}


	// 	});


	// }

	void setParent(TaskerBase * ptr) {
		_parent = ptr; 
		//Serial.printf("PARENT in %p set to %p\n", this, _parent);
	}

	// bool begin() {
	// 	Serial.println("Tasker Begin");
	// }


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


