#include <Tasker.h>

/*
 *  Creates a sync list of tasks.  Each run after the next
 * 
 */

Task tasker; 

void setup() {
  Serial.begin(115200);
  tasker.setType(Task::SYNC); 
  Serial.print("\nBOOTING....");
    // prints hello after 2 seconds. 
    tasker.add( [] (Task & t) {
        Serial.println("hello"); 
    }).setTimeout(2000); 
   // prints hello again, 5 seconds after first hello
    tasker.add( [] (Task & t) {
        Serial.println("hello again"); 
    }).setTimeout(5000);  
Serial.println("READY\n");
}

void loop() {
tasker.run();
}
