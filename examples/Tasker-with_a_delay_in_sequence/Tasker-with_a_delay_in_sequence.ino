#include <Tasker.h>


ASyncTasker tasker;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.printf("\nBEGIN: %u\n", ESP.getFreeHeap() );
   

  //  pSyncTasker is a shared_ptr...  

  tasker.add([](Task & t) { Serial.println("."); }).setTimeout(1000).setRepeat(); //  this task shows the loop is not blocked

  pSyncTasker tasker1 = tasker.addSubTasker<SyncTasker>();



  Serial.printf("Tasker = %p\n", &tasker);
  Serial.printf("Tasker1 = %p\n", tasker1.get());

  for (uint8_t i = 0; i < 3; i++) {
    tasker1->add( [i](Task & t) {
      Serial.printf("[%u] Running Sync Task %u\n", millis(), i);
    }).setTimeout(500);
  }

  tasker1->add( [](Task & t) {
    if (t.count > 1000) {
      Serial.println("Count reached... continuing");
      t.setRepeat(false);
    }
  }).setTimeout(10).setRepeat(); //  set repeat and check it every 10ms, this will now block continuation of the sync tasker but in an async fashion... 

  for (uint8_t i = 3; i < 6; i++) {
    tasker1->add( [i](Task & t) {
      Serial.printf("[%u] Running Sync Task %u\n", millis(), i);
    }).setTimeout(500);
  }


delay(20000);

  Serial.printf("READY: %u\n", ESP.getFreeHeap() );

}

void loop() {
  tasker.loop();
}