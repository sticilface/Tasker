#include <Tasker.h>

/*
 *  Recreate Blink Example
 *  This uses a SyncTasker instance.  So no other tasks are running.
 *  It would be better to use a nested subtasker and run other tasks alongside it. See that example.
 */

#define pin 2

Task tasker;

void setup()
{
  Serial.begin(115200);
  Serial.print("\nBOOTING....");

  pinMode(pin, OUTPUT);
  digitalWrite(pin, LOW);

  tasker.setType(Task::SYNC);

  //  turn on
  tasker.add( [] (Task & t) {
    digitalWrite(pin, HIGH);
  }).setTimeout(200).setRepeat(true); 

  // turn off 100ms later
  tasker.add( [] (Task & t) {
    digitalWrite(pin, LOW) ;
  }).setTimeout(100).setRepeat(true); 

  Serial.println("READY\n");
}

void loop()
{
  tasker.run();
}