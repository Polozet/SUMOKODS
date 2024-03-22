#pragma once

// Pievienojam Arduino kāju nosaukumus SumoBoy robotam no pins.h faila
#include "pins.h"

/* 
 * Šis ļauj vienkārši izsaukt braukšanas funkcijas
 * Piemēram, drive()
 *
 * Ātruma robežas (-255 - +255)
 * Braukšanai taisni, var mainīt katra motora ātrumu
 */

void drive(int left_speed, int right_speed) {
  // HIGH var nebūt taisni visiem robotiem, ja tavs robots
  // brauc atpakaļgaitā vai griežas, tad apmaini HIGH vietā ar LOW
  // Parasti: HIGH = Uz priekšu LOW = Atpakaļgaitā
  
  // Iestatām virzienu katram motoram, atkarībā no ātruma
  // Ja ātrums ir pozitīvs, braucam taisni, ja negatīvs, tad atpakaļgaitā
  if (left_speed > 0) {
    digitalWrite(Pin::motor_left_direction, HIGH);
  } else {
    digitalWrite(Pin::motor_left_direction, LOW);
  }
  if (right_speed > 0) {
    digitalWrite(Pin::motor_right_direction, HIGH);
  } else {
    digitalWrite(Pin::motor_right_direction, LOW);
  }

  if (left_speed < -255 || left_speed > 255) {
    left_speed = 255;
  }
  if (right_speed < -255 || right_speed > 255) {
    right_speed = 255;
  }

  // Iestatām ātrumu katram motoram
  analogWrite(Pin::motor_left_pwm, abs(left_speed));
  analogWrite(Pin::motor_right_pwm, abs(right_speed));
}

void stop() {
  digitalWrite(Pin::motor_left_direction, HIGH);
  digitalWrite(Pin::motor_right_direction, HIGH);

  // Iestatām ātrumu abiem motoriem 0
  analogWrite(Pin::motor_left_pwm, 0);
  analogWrite(Pin::motor_right_pwm, 0);
}
