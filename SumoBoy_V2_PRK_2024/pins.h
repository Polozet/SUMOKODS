#pragma once

/* 
 * Šis ļauj .ino failā vienkārši pielikt nosaukumu Arduino kājai (pin)
 * Piemēram, digitalWrite(13, HIGH) vietā var rakstīt
 * digitalWrite(Pin::led_arduino, HIGH), kas ir daudz saprotamāk
 */
namespace Pin {
  int led_arduino = 17;
  int led_left = 12;
  int led_right = 6;
  int led_back = 13;

  int button = A2;
  int infrared_start = 0;
  int infrared_stop = 1;

  int dip1 = 7;
  int dip2 = 15;
  int dip3 = 16;
  int dip4 = 14;

  int line_left = A4;
  int line_right = A3;
  int line_back = A5;

  int sharp_mid_left = A0;
  int sharp_mid = A1;
  int sharp_mid_right = 4;

  int motor_left_direction = 11;
  int motor_left_pwm = 5;
  int motor_right_direction = 10;
  int motor_right_pwm = 9;
};
