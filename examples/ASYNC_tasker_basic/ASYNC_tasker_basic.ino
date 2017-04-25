#include <Tasker.h>

/*
 *  Creates a ASync list of tasks.  Each checked for a run after every loop. 
 * 
 */


Task tasker; 

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
tasker.run();
}