This is an attempt at a task schedular for ESP8266.  Written in c++. 

Most of the complexities are hidden under the hood, it seems to work very well.  

Key features:

- NonBlocking.  The loop is not blocked, as long as you do not call delay() in your functions.
- Flexible.  All tasks are std::function calls.  
- ASyncTasker - Creates a task instance that runs all tasks concurrently, usefull for driving displays whilst checking buttons etc. 
- SyncTasker - Creates a sequential Tasker that waits for previous event to finish before starting the next. 
- addSubTasker<class TaskerType>(TaskerType tasker) - This function is very powerfull.  It creates a new tasker, and adds the required loop functions to the parent tasker, and returns a shared pointer back to this new Tasker.  Available to any Tasker instance.  It will exist as long as there is a shared pointer count or there are tasks running.  Then it deletes itself.  This allows you in any task to spawn a new tasker to run sync, or async along side existing tasks!!  all non blocking!  
- Templated Tasks and loop methods, allow you to design your own Task types and loop methods if required, the basic types are created for you. 
