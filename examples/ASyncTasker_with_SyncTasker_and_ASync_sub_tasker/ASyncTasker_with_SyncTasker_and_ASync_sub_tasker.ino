#include <Tasker.h>


Task tasker;


void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);

  Serial.println("\n\nBegin");

  Task & syncTasker = tasker.add().setType(Task::SYNC); 

  syncTasker.add([](Task & t) { Serial.printf("%u %s\n", millis(), t.getName().c_str() ); }).setName("subTask1").setTimeout(1000);
  syncTasker.add([](Task & t) { Serial.printf("%u %s\n", millis(), t.getName().c_str() ); }).setName("subTask2").setTimeout(1000);
  syncTasker.add([](Task & t) { Serial.printf("%u %s\n", millis(), t.getName().c_str() ); }).setName("subTask3").setTimeout(1000);
  syncTasker.add([](Task & t) { Serial.printf("%u %s\n", millis(), t.getName().c_str() ); }).setName("subTask4").setTimeout(1000);

  Task & asyncTasker = tasker.add().setType(Task::ASYNC);  /* Defaults to ASYNC normally */ 

  asyncTasker.add([](Task & t) { Serial.printf("%u %s\n", millis(), t.getName().c_str() ); }).setName("subTask5").setTimeout(2000);
  asyncTasker.add([](Task & t) { Serial.printf("%u %s\n", millis(), t.getName().c_str() ); }).setName("subTask6").setTimeout(2000);
  asyncTasker.add([](Task & t) { Serial.printf("%u %s\n", millis(), t.getName().c_str() ); }).setName("subTask7").setTimeout(2000);
  asyncTasker.add([](Task & t) { Serial.printf("%u %s\n", millis(), t.getName().c_str() ); }).setName("subTask8").setTimeout(2000);

  syncTasker.add([](Task & t) { Serial.printf("\n%u %s\n", millis(), t.getName().c_str() ); }).setName("subTask9").setTimeout(5000);

  tasker.dump(Serial);
  Serial.println();
}

void loop()
{
  // put your main code here, to run repeatedly:
  tasker.run();
}
