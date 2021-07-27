#include "NewPing.h"
#include "toneAC.h"

#define TRIGGER_PIN  10
#define ECHO_PIN     11
#define MAX_DISTANCE 400

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

float duration, distance;
float last_distances[10];
int iterations = 20;
float threshold = 50.0;
float total_time;
float current_time = 0.0;
float time_elapsed;
bool should_stand = false;

// wait for ultrasonic sensor to warmup, then set time variables and begin loop
void setup() {
  delay(3000);
  total_time = millis();
  time_elapsed = total_time - current_time;
  Serial.begin (9600);
}

// updates the last_distances array by removing the oldest distance point and appending the newest point of data to the end of the array
void update_last_distances() {
  duration = sonar.ping_median(iterations);

  // moves each array element back 1 position
  for (int i = 0; i < 9; i++) {
    last_distances[i] = last_distances[i+1];
  }

  // replace the last index of array with a new distance point based on the speed of sound and time for ping to reach back
  last_distances[9] = (duration / 2) * 0.0343;

  // if data point out of range of sensor's bounds, replace with more reasonable values
  if (last_distances[9] >= 400) {
    last_distances[9] = 400;
  }
  else if (last_distances[9] <= 2) {
    last_distances[9] = -1;
  }

  delay(500);
}

// using the data in the last_distances array and a threshold value, decide whether person is standing or sitting
bool is_standing() {
  int stand_count = 0;

  // loop through last_distances array and increment stand_count each time data point is found over the threshold
  for (int i = 0; i < 10; i++) {
    if (last_distances[i] > threshold) {
      stand_count++;
    }
  }

  // if at least 60% of the most recent data points indicate the person is further than the threshold, assume they are standing
  if (stand_count > 6) {
    return true;
  } else {
    return false;
  }
}
 
void loop() {
  // update total_time and time_elapsed from last stand/sit change
  total_time = millis();
  time_elapsed = total_time - current_time;

  // update the last_distances list
  update_last_distances();

  // print to serial monitor is_standing and values of last_distances 
  if (is_standing() == false) {
    Serial.print(" SITTING: ");
  } else {
    Serial.print(" STANDING: ");
  }
  Serial.print("[ ");
  for (int i = 0; i < 10; i++) {
    Serial.print(last_distances[i]);
    Serial.print(", ");
  }
  Serial.print("]");

  // if stand/sit preference does not match what person is doing, sound the buzzer at 400 Hz for 1 second
  if (should_stand != is_standing()) {
//   Serial.print("      BEEEEEEEEP");
    toneAC(400);
    delay(1000);
    toneAC();
  }
  Serial.println();

  // after ~40 sec, switch stand/sit preference and print change in serial monitor
  if (time_elapsed >= 1.0*40.0*1000.0) {
    current_time = millis();
    if (should_stand) {
      should_stand = false;
      Serial.println("---------Sit now.-------------");
    } else {
      should_stand = true;
      Serial.println("---------Stand now.---------------");
    }
  }
  
  delay(500);
}
