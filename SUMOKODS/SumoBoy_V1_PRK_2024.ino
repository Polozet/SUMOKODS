// Autors: Matīss @ PRK [23.01.2024]

// Pievienojam Arduino kāju nosaukumus SumoBoy robotam no pins.h faila un drive funkciju no drive.h
#include "drive.h"
#include "pins.h"

// Definējam mainīgos 
// Vērtība, zem kuras, nostrādā līnijas sensori
int line_sensor_threshold = 70;

// Mainīgie, kas norāda, vai cīņa ir aktīva un vai pirmā gājiena kustība ir izpildīta
bool ir_started = false;
bool fight_active = false;
bool first_move_done = false;

// Šī funkcija [void setup()] izpildās tikai vienu reizi robota ieslēgšanas brīdī
void setup() {
  // Ieslēdzam Serial monitor, lai varētu caur USB lasīt datus
  Serial.begin(9600);

  // Piešķiram robota elementiem (LEDiem, pogai, slēdžiem, motoriem) izvadu tipu
  pinMode(Pin::led_arduino, OUTPUT);
  pinMode(Pin::led_left, OUTPUT);
  pinMode(Pin::led_right, OUTPUT);
  pinMode(Pin::led_back, OUTPUT);

  // INPUT_PULLUP nozīmē, ka nenospiests stāvoklis būs augsts spriegums (HIGH),
  // bet nospiests - zems (LOW). Poga būs nospiesta, kad digitalRead būs LOW
  pinMode(Pin::button, INPUT_PULLUP);

  // Pults uztvērējs
  pinMode(Pin::infrared_start, INPUT_PULLUP);

  // DIP (4 baltie) slēdži
  // Tāpat kā parastai pogai - INPUT_PULLUP
  pinMode(Pin::dip1, INPUT_PULLUP);
  pinMode(Pin::dip2, INPUT_PULLUP);
  pinMode(Pin::dip3, INPUT_PULLUP);
  pinMode(Pin::dip4, INPUT_PULLUP);

  // Līnijas sensori
  pinMode(Pin::line_left, INPUT);
  pinMode(Pin::line_right, INPUT);
  pinMode(Pin::line_back, INPUT);

  // Pretinieka sensori
  // Nolasīšana ir pretēja (LOW = Pretinieks ir priekšā)
  pinMode(Pin::sharp_left, INPUT);
  pinMode(Pin::sharp_mid_left, INPUT);
  pinMode(Pin::sharp_mid, INPUT);
  pinMode(Pin::sharp_mid_right, INPUT);
  pinMode(Pin::sharp_right, INPUT);
  
  // Motori
  pinMode(Pin::motor_left_direction, OUTPUT);
  pinMode(Pin::motor_left_pwm, OUTPUT);
  pinMode(Pin::motor_right_direction, OUTPUT);
  pinMode(Pin::motor_right_pwm, OUTPUT);

  // Drošības pēc, apstādinām motorus
  delay(100);
  // Drive::Stop();
}

// Galvenā robota funkcija, kas izpildās nepārtraukti pēc setup() funkcijas
void loop() {
  // Gaidām starta signālu no pults vai pogas, ja cīņa vēl nav sākusies
  // Ja starta signāls sagaidīts, tad izpildām pirmo gājienu
  if (fight_active == false) {
    if (digitalRead(Pin::infrared_start) == HIGH) {
      ir_started = true;
      fight_active = true;
      firstMove();
    } else if (digitalRead(Pin::button) == LOW) {  
      // Nomirkšķinām LED 5 sekundes, lai redzētu, ka cīņa sāksies - tā arī ir aizture
      ir_started = false;
      fight_active = true;
      startBlinkWithDelay();
      firstMove();
    }

    // Ja ir nospiests DIP4 slēdzis, tad printējam sensoru vērtības
    if (!digitalRead(Pin::dip4)) {
      printSensors();
      delay(250);
    }
  }

  // Ja cīņa ir aktīva, tad izpildām cīņas stratēģiju
  if (fight_active == true) {
    fightTactic();

    // Pārbaudām, vai nav stop signāls
    // Pārbaudām pulti, tikai, ja ir palaists ar pulti
    if ((ir_started == true && digitalRead(Pin::infrared_start) == LOW) || (ir_started == false && digitalRead(Pin::button) == LOW)) {
      stop();
      fight_active = false;
      stopBlink();
    }
  }
}

// Šī funkcija pārbauda visu sensoru vērtības un atgriež vērtību,
// ar kuru vieglāk rakstīt stratēģijas
int getSensorState() {
  int sensor_state = 0;

  // Vispirms nolasām līnijas sensorus, jo galvenais ir turēties ringā
  if (analogRead(Pin::line_left) < line_sensor_threshold) {
    sensor_state = sensor_state + 1;
    digitalWrite(Pin::led_left, HIGH);
  }
  if (analogRead(Pin::line_right) < line_sensor_threshold) {
    sensor_state = sensor_state + 2;
    digitalWrite(Pin::led_right, HIGH);
  }
  if (analogRead(Pin::line_back) < line_sensor_threshold) {
    sensor_state = sensor_state + 4;
    digitalWrite(Pin::led_back, HIGH);
  }

  // Ja robots atrodas uz līnijas, tad uzreiz atgriežam sensor_state vērtību,
  // ignorējot pretinieka sensorus
  if (sensor_state != 0) {
    return sensor_state;
  }

  // Lasām pretinieka sensorus tikai tad, ja robots neatrodas uz līnijas
  // Sākumā nolasām tikai 3 vidējos sensorus (pārbaudām, vai pretinieks ir pretī)
  // Atceramies, ka pretinieka sensori ir pretēji (LOW = Pretinieks ir priekšā)
  if (digitalRead(Pin::sharp_mid_left) == LOW) {
    sensor_state = sensor_state + 8;
  }
  if (digitalRead(Pin::sharp_mid) == LOW) {
    sensor_state = sensor_state + 16;
  }
  if (digitalRead(Pin::sharp_mid_right) == LOW) {
    sensor_state = sensor_state + 32;
  }

  // Ja robots neredz pretinieku priekšā, tad uzreiz atgriežam sensor_state vērtību,
  // ignorējot sānu sensorus
  if (sensor_state != 0) {
    return sensor_state;
  }

  if (digitalRead(Pin::sharp_left) == LOW) {
    return 64;
  }
  if (digitalRead(Pin::sharp_right) == LOW) {
    return 128;
  }

  return sensor_state;
}

// Šī funkcija izpilda pirmo gājienu, kas notiek cīņas sākumā
// Šeit varam lietot DIP slēdžus, lai izvēlētos sākuma gājienu
void firstMove() {
  if (digitalRead(Pin::dip2) == LOW && digitalRead(Pin::dip1) == LOW) {
    drive(60, 60);
    delay(100);
  } else if (digitalRead(Pin::dip2) == LOW && digitalRead(Pin::dip3) == LOW) {
    drive(100, 100);
    delay(200);
  } else if (digitalRead(Pin::dip2) == LOW) {
    drive(255, 175);
    delay(300);
  } else if (digitalRead(Pin::dip1) == LOW) {
    drive(175, 255);
    delay(300);
  } else if (digitalRead(Pin::dip3) == LOW) {
    // Pakustamies nedaudz uz priekšu
    drive(200, 200);
    delay(100);
    stop();
    long sakuma_laiks = millis();
  
    // Gaidām, līdz ierauga pretinieku visi 3 sensori
    while (digitalRead(Pin::sharp_mid_left) == HIGH && digitalRead(Pin::sharp_mid) == HIGH && digitalRead(Pin::sharp_mid_right) == HIGH) {
      delay(50);

      if (millis() - sakuma_laiks > 5000 || digitalRead(Pin::sharp_left) == HIGH || digitalRead(Pin::sharp_right) == HIGH) {
        break;
      }
    }
    drive(25,25);
    delay(550);
    drive(255, 255);
    delay(100);
    
  }
}

// Robota taktika cīņas laikā
void fightTactic() {
  switch (getSensorState()) {

    // Līnijas sensori
    case 0:
      // Nav ne līnijas, ne pretinieka
      if (digitalRead(Pin::dip2) == LOW && digitalRead(Pin::dip1) == LOW) {
        drive(60,60);
        delay(50);
        break;
      }
      else {
        drive(75, 100);
        delay(50);
        break;
      }

    case 1:
      // Līnijas kreisais sensors
      drive(-200, -200);
      delay(150);
      drive(200, -200);
      delay(100);
      break;

    case 2:
      drive(-200, -200);
      delay(150);
      drive(-200, 200);
      delay(100);// Līnijas labais sensors
      break;

    case 3:
      drive(-200, -200);
      delay(150);
      drive(200, -200);
      delay(100);// Abi priekšējie līnijas sensori
      break;

    case 4:
      drive(200, 200);
      delay(150); // Līnijas aizmugurējais sensors
      break;

    case 5:
      drive(255, 150);
      delay(150);// Līnijas kreisais un aizmugurējais sensors
      break;

    case 6:
      drive(150, 255);
      delay(150); // Līnijas labais un aizmugurējais sensors
      break;

    case 7:
      drive(255, 255);// Visi līnijas sensori - nav iespējams cīņā
      delay(50);
      break;

    // Pretinieka vidus sensori
    case 8:
      // Pretinieka vidus kreisais sensors, kurš skatās krusteniski -
      // tātad pretinieks ir priekšā nedaudz pa labi
      if (digitalRead(Pin::dip2) == LOW && digitalRead(Pin::dip3) == LOW) {
          drive(128, 1);
          delay(50);
          break;
      } else {
          drive(180, 100);
          delay(50);
          break;
        }

    case 16:
      if (digitalRead(Pin::dip2) == LOW && digitalRead(Pin::dip1) == LOW) {
        drive(128,128);
        delay(50);
        break;
      }
      else {
        drive(200, 200);
        delay(50);
        break;
      }

    case 24:
      drive(200, 200);
      delay(50);
      break;

    case 32:
      if (digitalRead(Pin::dip2) == LOW && digitalRead(Pin::dip3) == LOW) {
        drive(1, 128);
        delay(50);
        break;
        }
      else {
        drive(100, 180);
        delay(50);
        break;
       }

    case 48:
      if (digitalRead(Pin::dip2) == LOW && digitalRead(Pin::dip1) == LOW) {
        drive(128,128);
        delay(50);
        break;
      }
      else {
        drive(225, 225);
        delay(50);
        break;
      }

    case 56:
      drive(255, 255);
      delay(50);
      break;

    case 64:
      if (digitalRead(Pin::dip2) == LOW && digitalRead(Pin::dip3) == LOW) {
       drive(-255, 0);
      delay(50);
      } else{
      drive(-200, 200);
      delay(50);
      break;
      }

    case 128:
      if (digitalRead(Pin::dip2) == LOW && digitalRead(Pin::dip3) == LOW) {
       drive(0,-255);
      delay(50);
      } else{
      drive(200, -200);
      delay(50);
      break;
      }
  }
}

// Nomirkšķina LEDus vairākas reizes 5 sekundēs
void startBlinkWithDelay() {
  for (int i = 0; i < 4; i++) {
    digitalWrite(Pin::led_left, HIGH);
    digitalWrite(Pin::led_right, HIGH);
    digitalWrite(Pin::led_back, HIGH);
    delay(250);
    digitalWrite(Pin::led_left, LOW);
    digitalWrite(Pin::led_right, LOW);
    digitalWrite(Pin::led_back, LOW);
    delay(750);
  }

  digitalWrite(Pin::led_left, HIGH);
  digitalWrite(Pin::led_right, HIGH);
  digitalWrite(Pin::led_back, HIGH);
  delay(1000);
  digitalWrite(Pin::led_left, LOW);
  digitalWrite(Pin::led_right, LOW);
  digitalWrite(Pin::led_back, LOW);
}

// Nomirkšķina LEDus vienreiz 2.5 sekundes
void stopBlink() {
  digitalWrite(Pin::led_left, HIGH);
  digitalWrite(Pin::led_right, HIGH);
  digitalWrite(Pin::led_back, HIGH);
  delay(2500);
  digitalWrite(Pin::led_left, LOW);
  digitalWrite(Pin::led_right, LOW);
  digitalWrite(Pin::led_back, LOW);
}

// Izprintējam sensoru vērtības Serial monitor
void printSensors() {
  Serial.print("Line left: ");
  Serial.println(analogRead(Pin::line_left));
  Serial.print("Line back: ");
  Serial.println(analogRead(Pin::line_back));
  Serial.print("Line right: ");
  Serial.println(analogRead(Pin::line_right));
  
  Serial.print("Enemy left: ");
  Serial.println(digitalRead(Pin::sharp_left) == LOW);
  Serial.print("Enemy mid left: ");
  Serial.println(digitalRead(Pin::sharp_mid_left) == LOW);
  Serial.print("Enemy front: ");
  Serial.println(digitalRead(Pin::sharp_mid) == LOW);
  Serial.print("Enemy mid right: ");
  Serial.println(digitalRead(Pin::sharp_mid_right) == LOW);
  Serial.print("Enemy right: ");
  Serial.println(digitalRead(Pin::sharp_right) == LOW);

  Serial.print("DIP1: ");
  Serial.println(digitalRead(Pin::dip1) == LOW);
  Serial.print("DIP2: ");
  Serial.println(digitalRead(Pin::dip2) == LOW);
  Serial.print("DIP3: ");
  Serial.println(digitalRead(Pin::dip3) == LOW);
  Serial.print("DIP4: ");
  Serial.println(digitalRead(Pin::dip4) == LOW);

  Serial.println();
}
