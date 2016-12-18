


/*
SticilFace

General Task Manager....

Priority!  not sure how to do this...

0 = always run every loop




*/

#pragma once

#ifndef MY_TASKER_H
#define MY_TASKER_H

#include <Arduino.h>
#include <functional>
#include <list>
#include <memory>


#include "TaskerClass.h"
#include "msTask.h"
#include "Sequence.h"
#include "TaskMethods.h"



typedef Tasker<Task, Sync<Task>> SyncTasker;
typedef Tasker<Task, ASync<Task>> ASyncTasker;
typedef Tasker<Sequence, Sync<Sequence>> SequenceTasker;


#endif

