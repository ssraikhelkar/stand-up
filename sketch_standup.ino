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

void setup() {
  delay(3000);
  total_time = millis();
  time_elapsed = total_time - current_time;
  Serial.begin (9600);
}

void update_last_distances() {
  duration = sonar.ping_median(iterations);
  
  for (int i = 0; i < 9; i++) {
    last_distances[i] = last_distances[i+1];
  }

  last_distances[9] = (duration / 2) * 0.0343;
  if (last_distances[9] >= 400 || last_distances[9] <= 2) {
    last_distances[9] = -1;
  }
  
  delay(500);
}

bool is_standing() {
  int stand_count = 0;

  for (int i = 0; i < 10; i++) {
    if (last_distances[i] > threshold) {
      stand_count++;
    }
  }

  if (stand_count > 6) {
    return true;
  } else {
    return false;
  }
}
 
void loop() {
  total_time = millis();
  time_elapsed = total_time - current_time;
  
  update_last_distances();

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

  if (should_stand != is_standing()) {
//    Serial.print("      BEEEEEEEEP");
    toneAC(100);
    delay(1000);
    toneAC();
  }

  Serial.println();
  
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
