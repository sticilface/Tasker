#include <Tasker.h>


ASyncTasker tasker;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  Serial.println("\n\nBegin"); 

  SyncTasker & syncTasker = *tasker.addSubTasker<SyncTasker>();

  syncTasker.add([](Task & t) { Serial.printf("%u %s\n",millis(), t.name() ); }).setName("subTask1").setTimeout(1000); 
  syncTasker.add([](Task & t) { Serial.printf("%u %s\n",millis(), t.name() ); }).setName("subTask2").setTimeout(1000); 
  syncTasker.add([](Task & t) { Serial.printf("%u %s\n",millis(), t.name() ); }).setName("subTask3").setTimeout(1000); 
  syncTasker.add([](Task & t) { Serial.printf("%u %s\n",millis(), t.name() ); }).setName("subTask4").setTimeout(1000); 

  ASyncTasker & asyncTasker = *syncTasker.addSubTasker<ASyncTasker>();
  
  asyncTasker.add([](Task & t) { Serial.printf("%u %s\n",millis(), t.name() ); }).setName("subTask5").setTimeout(2000); 
  asyncTasker.add([](Task & t) { Serial.printf("%u %s\n",millis(), t.name() ); }).setName("subTask6").setTimeout(2000); 
  asyncTasker.add([](Task & t) { Serial.printf("%u %s\n",millis(), t.name() ); }).setName("subTask7").setTimeout(2000); 
  asyncTasker.add([](Task & t) { Serial.printf("%u %s\n",millis(), t.name() ); }).setName("subTask8").setTimeout(2000); 

  syncTasker.add([](Task & t) { Serial.printf("\n%u %s\n",millis(), t.name() ); }).setName("subTask9").setTimeout(5000); 

  tasker.debugOut(Serial);
}

void loop() {
  // put your main code here, to run repeatedly:
tasker.loop();
}
