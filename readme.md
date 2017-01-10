This is an attempt at a task schedular for ESP8266.  Written in c++. 

Most of the complexities are hidden under the hood, it seems to work very well.  

# Key features:

- NonBlocking.  The loop is not blocked, as long as you do not call delay() in your functions.
- Flexible.  All tasks are std::function calls so can use std::bind or lambdas.  
- ASyncTasker - Creates a task instance that runs all tasks concurrently, usefull for driving displays whilst checking buttons etc. non premtive multitasking! 
	- This feature has a timeout feature so each loop cycle it will visit each task to see if it needs running.  If one task takes more than the timeout then it exits, continuing where it left of next loop.  This allows you to maintain say wifi or other tasks as well.  
- SyncTasker - Creates a sequential Tasker that waits for previous event to finish before starting the next. 
	-  This is good for running a sequence of events that depend on the previous finishing.  This tasker can be set to repeat which will continue to loop around. See Blink example using this. 
- addSubTasker<class TaskerType>(TaskerType tasker) - This function is very powerfull.  It creates a new tasker, and adds the required loop functions to the parent tasker, and returns a pointer back to this new Tasker.  Available to any Tasker instance.  This allows you in any task to spawn a new tasker to run sync, or async along side existing tasks!!  all non blocking!  
- Templated Tasks and loop methods, allow you to design your own Task types and loop methods if required, the basic typedefs are created for you `ASyncTasker` and `SyncTasker`.  Plans are to implement light weight methods and tasks that don't occupy much RAM.  So you can develop using the verbose outputs, and switch to lightweight for production. 

### Examples

Basic
```cpp
#include <Tasker.h>

ASyncTasker tasker; 

void setup() {
  Serial.begin(115200);
  Serial.print("\nBOOTING....");
    // prints hello after 10 seconds. 
    tasker.add( [] (Task & t) {
        Serial.println("hello"); 
    }).setTimeout(10000); 
   // prints hello again after 20 seconds 
    tasker.add( [] (Task & t) {
        Serial.println("hello again"); 
    }).setTimeout(20000);  
Serial.println("READY\n");
}

void loop() {
tasker.loop();
}
```

These two tasks run at the same time. 
This next example adds two tasks to a SyncTasker, which waits for the first task to run before running the next. Declare `SyncTasker tasker; ` instead of `ASyncTasker tasker; ` then each task will follow the next. 
### Tasks
Tasks are created internally with std::unique_ptr, they are deleted when they are complete.  A reference is returned to the pointer at creation allowing you to set and change parameters, but do not convert to a pointer, as the lifetime of the object is not guarunteed!  you have been warned!  I may implement a method to check if a task is still running...

Each task callback is called with a reference to the task, so all task properties are changable within the callback.  This allows you to test a condition and then change it at run time. 
eg
```cpp
   Task & t = tasker.add( [] (Task & t) {
        Serial.println("hello again"); 
        t.setRepeat(false); //  changes to no repeating, and deletes the task.
        uint32_t count = t.count; // returns the run number. 
        t.setTimeout(1000); //  change the timout to 1 second from 20 seconds! 
    }).setTimeout(20000).setRepeat(true); 
```

Tasks can also be named `.setName(const char *)`, good for debugging as the name will appear in the debug output.   
### Creating subtasks
```
auto subtasker = tasker.addSubTasker<SyncTasker>();
```
This gives you a pointer to the sub tasker, behind the scenes it adds a loop() to the parent tasker, which is automatically deleted when the count of tasks == 0. This is unless you use `auto subtasker = tasker.addSubTasker<SyncTasker>(true)`, which prevents the subtasker from being deleted.  Unless you've set this to true, it is probably safe to assume that the tasker has been deleted and you're accessing a null ptr! 

### Debug out
Using `tasker.debugOut(Serial);` gives you a list of all the current running tasks, and subtasks. 
```
*****  Tasker  *****  
Heap: 43320
Tasker: 0x3ffef290 - 4 Tasks Running
    1. 0x3fff0a0c [N][Debug Out] priority 0, timeout = 10000, runcount = 1
    2. Tasker: 0x3fff0a88 - 4 Tasks Running Parent Tasker 0x3ffef290
       └ 1. 0x3fff0b14 [N][SubTask1.1] timeout = 30000, runcount = 0
        └ 2. 0x3fff0b74 [N][SubTask1.2] timeout = 30000, runcount = 0
         └ 3. 0x3fff0bd4 [N][SubTask1.3] timeout = 30000, runcount = 0
          └ 4. 0x3fff0c3c [N][SubTask1.4] timeout = 30000, runcount = 0
    3. Tasker: 0x3fff0cb8 - 1 Tasks Running Parent Tasker 0x3ffef290
        └ 1. Tasker: 0x3fff0e88 - 1 Tasks Running Parent Tasker 0x3fff0cb8
            1. Tasker: 0x3fff1328 - 20 Tasks Running Parent Tasker 0x3fff0e88
               └ 1. 0x3fff13c4 [N][supernested] timeout = 10000, runcount = 0
                └ 2. 0x3fff140c [N][supernested] timeout = 10000, runcount = 0
                 └ 3. 0x3fff1474 [N][supernested] timeout = 10000, runcount = 0
*****   END   *****
```
Tasks run async just appear as numbers, tasks run sync and depend on the previous finishing have └ and are indented.  The memory location of all taskers and tasks is given to help debugging. 

### Other
- There are plans for a priority approach but it has not been implemented yet... watch this space. 
- I also plan to write lightweight less RAM hungry versions.  





