/*

    Automatic "Power ON" button
    This program just emulates PWR button pressing

    Copyright 2016 Artyom Shcherbakov

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA

    http://www.gnu.org/licenses/gpl-2.0.txt

*/

// Constants :
const long c_stage_init    = 0x01;    // Stage: Initialization.
const long c_stage_wait    = 0x02;    // Stage: Waiting.
const long c_stage_push    = 0x03;    // Stage: Pushing.
const long c_stage_release = 0x04;    // Stage: Releasing.
const long c_stage_done    = 0x05;    // Stage: Done.
const long c_stage_exit    = 0x00;    // Stage: Terminated.
const long c_stage_unkn    = 0xFF;    // Stage: Unknown.
const long c_interval_init = 1000;    // Time interval on initialization.
const long c_interval_wait = 2000;    // Time interval to wait before push.
const long c_interval_push1 = 3000;   // Time interval to push M1.
const long c_interval_push2 = 5000;   // Time interval to push M2.
const long c_interval_release = 10;   // Time interval to release.
const long c_1s_interval = 1000;      // One second interval.
const long c_1m_interval = 60000;     // One minute interval.

// Used here to set a pin numbers :
const int M1Pin = 3; // M1 pin number.
const int M2Pin = 5; // M2 pin number.

// Variables :

// Changeable OUT states :
int M1State = LOW; // M1 pin init state.
int M2State = LOW; // M2 pin init state.

// Time counting variables :
int h = 0;  // Hours
int m = 0;  // Minutes
int s = 0;  // Seconds
int ms = 0; // Milliseconds

// Time calculation variables :
unsigned long ms_prev = 0;
unsigned long ms_start = 0;
unsigned long ms_elapsed = 0;
unsigned long ms_over = 0;

// Stage values
unsigned long M1_stage = 0;
unsigned long M2_stage = 0;

// Stage time values
unsigned long M1_stage_ms = 0;
unsigned long M2_stage_ms = 0;

// Serial conversation :
char* SC = new char[1024]; // String buffer

void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  // first calcTime shot
  calcTime();
  // say about initialization
  sprintf(SC, "");
  sprintf(SC, "%s[%0.2d:%0.2d:%0.2d.%0.3d]: INIT.\n", SC, h, m, s, ms);
  Serial.print(SC);
  // set stages with predefined constants
  M1_stage = c_stage_init;
  M2_stage = c_stage_init;
  // set time of stages with predefined constants
  M1_stage_ms = c_interval_init;
  M2_stage_ms = c_interval_init;
  // set the digital pin as output:
  pinMode(M1Pin, OUTPUT);
  // set the digital pin as output:
  pinMode(M2Pin, OUTPUT);
  // set the LED1 with initial state stored in led1State:
  digitalWrite(M1Pin, M1State);  
  // set the LED2 with initial state stored in led2State:
  digitalWrite(M2Pin, M2State);
}

void calcTime() {
    // This variable will be used to store current time in milliseconds
    unsigned long ms_time = millis();
  
    if (ms_start == 0) {
      ms_start = ms_time;
    }

    ms_elapsed = ms_time - ms_start;
    h = round(ms_elapsed / 3600000);
    if (h == 24) {
      h = 0;
    }
    ms_over = ms_elapsed % 3600000;
    m = round(ms_over / 60000);
    ms_over = ms_over % 60000;
    s = round(ms_over / 1000);
    ms = round(ms_over % 1000);
}

void loop() {
  // here is where you'd put code that needs to be running all the time.

  unsigned long ms_current = millis();

  if (ms_current - ms_prev >= c_1s_interval) {
    sprintf(SC, "");
    calcTime();

    switch(M1_stage) {
      case c_stage_init:
        sprintf(SC, "%s[%0.2d:%0.2d:%0.2d.%0.3d]: M1: INIT.\n", SC, h, m, s, ms);
        M1State = LOW;
        digitalWrite(M1Pin, M1State);
        sprintf(SC, "%s[%0.2d:%0.2d:%0.2d.%0.3d]: M1 = LOW\n", SC, h, m, s, ms);
        M1_stage = c_stage_wait;
        M1_stage_ms = ms_current;
        break;
      case c_stage_wait:
        if (ms_current - M1_stage_ms >= c_interval_init) {
          sprintf(SC, "%s[%0.2d:%0.2d:%0.2d.%0.3d]: M1: WAIT.\n", SC, h, m, s, ms);
          M1State = LOW;
          digitalWrite(M1Pin, M1State);
          sprintf(SC, "%s[%0.2d:%0.2d:%0.2d.%0.3d]: M1 = LOW\n", SC, h, m, s, ms);
          M1_stage = c_stage_push;
          M1_stage_ms = ms_current;
        }
        break;
      case c_stage_push:
        if (ms_current - M1_stage_ms >= c_interval_wait) {
          sprintf(SC, "%s[%0.2d:%0.2d:%0.2d.%0.3d]: M1: PUSH.\n", SC, h, m, s, ms);
          M1State = HIGH;
          digitalWrite(M1Pin, M1State);
          sprintf(SC, "%s[%0.2d:%0.2d:%0.2d.%0.3d]: M1 = HIGH\n", SC, h, m, s, ms);
          M1_stage = c_stage_release;
          M1_stage_ms = ms_current;
        }
        break;
      case c_stage_release:
        if (ms_current - M1_stage_ms >= c_interval_push1) {
          sprintf(SC, "%s[%0.2d:%0.2d:%0.2d.%0.3d]: M1: RELEASE.\n", SC, h, m, s, ms);
          M1State = LOW;
          digitalWrite(M1Pin, M1State);
          sprintf(SC, "%s[%0.2d:%0.2d:%0.2d.%0.3d]: M1 = LOW\n", SC, h, m, s, ms);
          M1_stage = c_stage_done;
          M1_stage_ms = ms_current;
        }
        break;
      case c_stage_done:
        if (ms_current - M1_stage_ms >= c_interval_release) {
          sprintf(SC, "%s[%0.2d:%0.2d:%0.2d.%0.3d]: M1: DONE.\n", SC, h, m, s, ms);
          M1State = LOW;
          digitalWrite(M1Pin, M1State);
          sprintf(SC, "%s[%0.2d:%0.2d:%0.2d.%0.3d]: M1 = LOW\n", SC, h, m, s, ms);
          M1_stage = c_stage_exit;
          M1_stage_ms = ms_current;
        }
        break;
      default:
        break;
    }

    switch(M2_stage) {
      case c_stage_init:
        sprintf(SC, "%s[%0.2d:%0.2d:%0.2d.%0.3d]: M2: INIT.\n", SC, h, m, s, ms);
        M2State = LOW;
        digitalWrite(M2Pin, M2State);
        sprintf(SC, "%s[%0.2d:%0.2d:%0.2d.%0.3d]: M2 = LOW\n", SC, h, m, s, ms);
        M2_stage = c_stage_wait;
        M2_stage_ms = ms_current;
        break;
      case c_stage_wait:
        if (ms_current - M2_stage_ms >= c_interval_init) {
          sprintf(SC, "%s[%0.2d:%0.2d:%0.2d.%0.3d]: M2: WAIT.\n", SC, h, m, s, ms);
          M2State = LOW;
          digitalWrite(M2Pin, M2State);
          sprintf(SC, "%s[%0.2d:%0.2d:%0.2d.%0.3d]: M2 = LOW\n", SC, h, m, s, ms);
          M2_stage = c_stage_push;
          M2_stage_ms = ms_current;
        }
        break;
      case c_stage_push:
        if (ms_current - M2_stage_ms >= c_interval_wait) {
          sprintf(SC, "%s[%0.2d:%0.2d:%0.2d.%0.3d]: M2: PUSH.\n", SC, h, m, s, ms);
          M2State = HIGH;
          digitalWrite(M2Pin, M2State);
          sprintf(SC, "%s[%0.2d:%0.2d:%0.2d.%0.3d]: M2 = HIGH\n", SC, h, m, s, ms);
          M2_stage = c_stage_release;
          M2_stage_ms = ms_current;
        }
        break;
      case c_stage_release:
        if (ms_current - M2_stage_ms >= c_interval_push2) {
          sprintf(SC, "%s[%0.2d:%0.2d:%0.2d.%0.3d]: M2: RELEASE.\n", SC, h, m, s, ms);
          M2State = LOW;
          digitalWrite(M2Pin, M2State);
          sprintf(SC, "%s[%0.2d:%0.2d:%0.2d.%0.3d]: M2 = LOW\n", SC, h, m, s, ms);
          M2_stage = c_stage_done;
          M2_stage_ms = ms_current;
        }
        break;
      case c_stage_done:
        if (ms_current - M2_stage_ms >= c_interval_release) {
          sprintf(SC, "%s[%0.2d:%0.2d:%0.2d.%0.3d]: M2: DONE.\n", SC, h, m, s, ms);
          M2State = LOW;
          digitalWrite(M2Pin, M2State);
          sprintf(SC, "%s[%0.2d:%0.2d:%0.2d.%0.3d]: M2 = LOW\n", SC, h, m, s, ms);
          M2_stage = c_stage_exit;
          M2_stage_ms = ms_current;
        }
        break;
      default:
        break;
    }
    
    ms_prev = ms_current;

    // Write out console buffer
    Serial.print(SC);
  }
}