#pragma once

#ifndef MY_TASKER_MANAGER_H
#define MY_TASKER_MANAGER_H

#include "TaskerClass.h"

class TaskerBase; 





class TaskerManager {
public:
	TaskerManager(TaskerBase & in): _top(&in) {} 
	TaskerManager() {

	} 


private:
	TaskerBase * _top; 

};


#endif

