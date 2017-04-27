#include <Tasker.h>


Task tasker;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.printf("\nBEGIN: %u\n", ESP.getFreeHeap() );
   

  tasker.add([](Task & t) { Serial.println("."); }).setTimeout(1000).setRepeat(); //  this task shows the loop is not blocked

  Task & tasker1 = tasker.add().setType(Task::SYNC); //  create a SYNC subtask. 

  Serial.printf("Tasker = %p\n", &tasker );
  Serial.printf("Tasker1 = %p\n", &tasker1 );

  for (uint8_t i = 0; i < 3; i++) {
    tasker1.add( [i](Task & t) {
      Serial.printf("[%u] Running Sync Task %u\n", millis(), i);
    }).setTimeout(500);
  }

  tasker1.add( [](Task & t) {
      Serial.println("Count reached... continuing");
  }).setTimeout(10).setRepeat(1000); //  set repeat and check it every 10ms, this will now block continuation of the sync tasker but in an async fashion... 


  for (uint8_t i = 3; i < 6; i++) {
    tasker1->add( [i](Task & t) {
      Serial.printf("[%u] Running Sync Task %u\n", millis(), i);
    }).setTimeout(500);
  }

delay(5000);

  Serial.printf("READY: %u\n", ESP.getFreeHeap() );

}

void loop() {
  tasker.run();
}
