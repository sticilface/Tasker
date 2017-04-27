#include <Tasker.h>


Task tasker;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.printf("\nBEGIN: %u\n", ESP.getFreeHeap() );

  Serial.printf("size of tasker = %u\n", sizeof(tasker)); 
   
  tasker.setTimeout(500); 
  tasker.setMicros(true); 
  tasker.setName("MainLoop"); 

  Task * dotter = &tasker.add([](Task & t) { Serial.print("."); }).setTimeout(100).setRepeat(true); //  this task shows the loop is not blocked

  Task & tasker1 = tasker.add().setType(Task::SYNC); //  create a SYNC subtask; 
  tasker1.onEnd([]() { Serial.println("Tasker1 is deleted") ;  tasker.dump(Serial); }) ; 
  tasker1.setName("SYNC set of Tasks"); 

  Serial.printf("Tasker = %p\n", &tasker );
  Serial.printf("Tasker1 = %p\n", &tasker1 );

  for (uint8_t i = 0; i < 3; i++) {
    tasker1.add( [i](Task & t) {
      Serial.printf("[%u] Running Sync Task %u\n", millis(), i);
    }).setTimeout(200).setName( String(i)).setRepeat(i + 1);
  }
  
  tasker1.add( [](Task & t) {
      Serial.println("Count reached... continuing");
  }).setTimeout(1000).setRepeat(5).setName("3"); //  set repeat and check it every 10ms, this will now block continuation of the sync tasker but in an async fashion... 
  
  for (uint8_t i = 3; i < 6; i++) {
    tasker1.add( [i](Task & t) {
      Serial.printf("[%u] Running Sync Task %u\n", millis(), i);
    }).setTimeout(500).setName(String(i+1)).setRepeat(i + 1); 
  }


  tasker1.add( [dotter](Task & t) {
      tasker.remove(dotter); 
      Serial.println("DONE");   
  }).setName("endTask"); //  set repeat and check it every 10ms, this will now block continuation of the sync tasker but in an async fashion... 


  tasker.dump(Serial); 

  Serial.printf("READY: %u\n", ESP.getFreeHeap() );

}

void loop() {
  tasker.run();
}
