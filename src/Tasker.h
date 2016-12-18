


/*
SticilFace

General Task Manager....

Priority!  not sure how to do this...

0 = always run every loop




*/

#pragma once


#include <Arduino.h>
#include <functional>
#include <list>
#include <memory>


#include "TaskerClass.h"
#include "msTask.h"
#include "usTask.h"
#include "TaskMethods.h"



typedef Tasker<Task, Sync<Task>> SyncTasker;
typedef Tasker<Task, ASync<Task>> ASyncTasker;

typedef Tasker<usTask, Sync<usTask>> usSyncTasker;
typedef Tasker<usTask, ASync<usTask>> usASyncTasker;