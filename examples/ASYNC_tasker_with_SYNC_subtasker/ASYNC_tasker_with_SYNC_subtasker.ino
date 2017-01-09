#include <Tasker.h>

/*
 *  Creates a ASYNC tasker, adds some tasks to it, then creates a SYNC subtasker that executes three tasks one after the other. 
 *  The sync subtasker is done after 3 seconds, then the other two tasks run. 
 */

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
    }).setTimeout(10000);  

    auto subtasker = tasker.addSubTasker<SyncTasker>(); 

    subtasker->add( [] (Task & t) {
        Serial.println("This is Subtask 1"); 
    }).setTimeout(1000);
    subtasker->add( [] (Task & t) {
        Serial.println("This is Subtask 2"); 
    }).setTimeout(1000);
    subtasker->add( [] (Task & t) {
        Serial.println("This is Subtask 3"); 
    }).setTimeout(1000);     
    
Serial.println("READY\n");
}

void loop() {
tasker.loop();
}
