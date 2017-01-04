
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
	//typedef TaskType* task_p;
	typedef std::function<void(TaskType&)> taskerCb_t;

	static uint8_t tasker_id; 

	Tasker() {};
	~Tasker() {};

	TaskType & add(taskerCb_t Cb, bool useMicros = false)
	{
		//debugOut(Serial);

		//Serial.println("ADD called");
		LoopMethod::_list.push_back(typename LoopMethod::task_t(new TaskType(Cb, useMicros)));
		LoopMethod::_it = LoopMethod::_list.begin();

//		debugOut(Serial);
		LoopMethod::listEmptyFnFlag = false;

		TaskType * ret = &*LoopMethod::_list.back();

		Serial.printf("Added 0x%p\n", ret);

		return *LoopMethod::_list.back();

		// TaskType & ret = *LoopMethod::_list.back();
		// LoopMethod::sort();
		// LoopMethod::_it = LoopMethod::_list.begin();
		// return ret;
	}

	TaskType & addBefore(taskerCb_t Cb, bool useMicros = false)
	{



	}

	TaskType & addAfter(taskerCb_t Cb, bool useMicros = false)
	{



	}


	bool remove(TaskType & task)
	{
		debugOut(Serial);

		Serial.printf("Removing Task: @%p ", &task );
		
		typename LoopMethod::taskList_t::iterator it;

		for (it = LoopMethod::_list.begin(); it != LoopMethod::_list.end(); /*  leave empty */ ) {

			if (&task == it->get()) {

				if (LoopMethod::_it == it) { ++LoopMethod::_it; } //  this line advances the stored iterator... if the one being deleted is due to be run next!!!

				it = LoopMethod::_list.erase(it);
				Serial.println("done");


				return true;
			} else {
				it++;
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

		stream.println("\n*****  Tasker *****  ");
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

	template<class TaskerType>
	TaskerType & addSubTasker (bool deleteAfter = true)
	{

		TaskerType * ptr = new TaskerType;  // create new pointer to tasker type...  sync or async...

		task_t & task = this->add( [ptr] (task_t & t) {  // add the loop to it...

			ptr->loop();

		}).setRepeat(true);

		if (deleteAfter) {


			ptr->SetEmptyFn( [&task, this, ptr] () {

				Serial.println("[addSubTasker]  Deleting SubTask"); 
				this->remove(task);
				delete ptr;

			});



		}

		Serial.printf("[addSubTasker] Creating SubTask : 0x%p = addr in parent tasker\n", &task);

		return *ptr; 

	}

#endif



// private:

	// uint32_t _startTime{0};
	// uint32_t _loopTime{0};
	// uint32_t _loopCounter{0};
//	uint32_t _maxWait{0};


};