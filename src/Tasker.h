


/*
SticilFace

General Task Manager....

Priority!  not sure how to do this...

0 = always run every loop

to add Y N is running, enabled, disabled, 



*/

#pragma once


// #define DEBUG_TASKER Serial

#ifdef DEBUG_TASKER
#define  DEBUG_TASKERf(...)  DEBUG_TASKER.printf(__VA_ARGS__)
#else
#define  DEBUG_TASKERf(...)  {}
#endif



#ifndef MY_TASKER_H
#define MY_TASKER_H

#include <Arduino.h>
#include <functional>
#include <list>
#include <memory>

#include "TaskerClass.h"
#include "Task.h"
#include "TaskMethods.h"



typedef Tasker<Task, Sync<Task>> SyncTasker;
typedef Tasker<Task, ASync<Task>> ASyncTasker;  //  runs all the tasks at the same time.  multitasking..


typedef std::shared_ptr<SyncTasker> pSyncTasker;
typedef std::shared_ptr<ASyncTasker> pASyncTasker;




#endif

