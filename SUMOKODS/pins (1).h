#pragma once

/* 
 * Šis ļauj .ino failā vienkārši pielikt nosaukumu Arduino kājai (pin)
 * Piemēram, digitalWrite(13, HIGH) vietā var rakstīt
 * digitalWrite(Pin::led_arduino, HIGH), kas ir daudz saprotamāk
 */
namespace Pin {
  int led_arduino = 13;
  int led_left = 5;
  int led_right = 11;
  int led_back = 2;

  int button = 12;
  int infrared_start = 18;

  int dip1 = 15;
  int dip2 = 14;
  int dip3 = 16;
  int dip4 = 17;

  int line_left = A2;
  int line_right = A1;
  int line_back = A4;

  int sharp_left = A5;
  int sharp_mid_left = A3;
  int sharp_mid = 7;
  int sharp_mid_right = A8;
  int sharp_right = A7;

  int motor_left_direction = 4;
  int motor_left_pwm = 3;
  int motor_right_direction = 10;
  int motor_right_pwm = 9;
};
