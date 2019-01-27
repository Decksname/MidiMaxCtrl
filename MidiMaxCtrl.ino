/*
   Based on the examples that shows how to make a simple seven keys MIDI keyboard with volume control
    http://www.arduino.cc/en/Tutorial/MidiDevice
   
   
   Created: Jan 22th 2019
   Author: max.schenk@gmx.net
   
   
*/

#include "MIDIUSB.h"
#include "PitchToNote.h"
#define NUM_BUTTONS  1

const uint8_t button1 = 2;
const uint8_t button2 = 3;
const uint8_t button3 = 4;
const uint8_t button4 = 5;
const uint8_t button5 = 6;
const uint8_t button6 = 7;
const uint8_t button7 = 8;

const int ledPin =  13;      // the number of the LED pin

const int intensityPot = 0;  //A0 input

const uint8_t buttons[NUM_BUTTONS] = {button1, };
const byte notePitches[NUM_BUTTONS] = {C2};

uint8_t notesTime[NUM_BUTTONS];
uint8_t pressedButtons = 0x00;
uint8_t previousButtons = 0x00;
uint8_t intensity;

void setup() {
  // initialize the LED pin as an output:
  pinMode(ledPin, OUTPUT);
  
  for (int i = 0; i < NUM_BUTTONS; i++)
    pinMode(buttons[i], INPUT_PULLUP);
}


void loop() {
  readButtons();
  readIntensity();
  //playNotes();
  sendControls();
}

// First parameter is the event type (0x0B = control change).
// Second parameter is the event type, combined with the channel.
// Third parameter is the control number number (0-119).
// Fourth parameter is the control value (0-127).

void controlChange(byte channel, byte control, byte value) {
  midiEventPacket_t event = {0x0B, 0xB0 | channel, control, value};
  MidiUSB.sendMIDI(event);
}

void readButtons()
{
  for (int i = 0; i < NUM_BUTTONS; i++)
  {
    if (digitalRead(buttons[i]) == HIGH)
    {
      bitWrite(pressedButtons, i, 1);
      delay(50);
    }
    else
      bitWrite(pressedButtons, i, 0);
  }
}

void readIntensity()
{
  intensity = 127;
}

void playNotes()
{
  for (int i = 0; i < NUM_BUTTONS; i++)
  {
    if (bitRead(pressedButtons, i) != bitRead(previousButtons, i))
    {
      if (bitRead(pressedButtons, i))
      {
        bitWrite(previousButtons, i , 1);
        noteOn(0, notePitches[i], intensity);
        MidiUSB.flush();
        Serial.print("ON");
        // turn LED on:
        digitalWrite(ledPin, HIGH);
      }
      else
      {
        bitWrite(previousButtons, i , 0);
        noteOff(0, notePitches[i], 0);
        MidiUSB.flush();
        Serial.print("OFF");
        // turn LED off:
        digitalWrite(ledPin, LOW);
      }
    }
  }
}

void sendControls()
{
  for (int i = 0; i < NUM_BUTTONS; i++)
  {
    if (bitRead(pressedButtons, i) != bitRead(previousButtons, i))
    {
      if (bitRead(pressedButtons, i))
      {
        bitWrite(previousButtons, i , 1);
        controlChange(0x01, 0x03, 0x00);

        
        MidiUSB.flush();
        Serial.print("CTRL SEND");
        // turn LED on:
        digitalWrite(ledPin, HIGH);
      }
      else
      {
      bitWrite(previousButtons, i , 0);
        Serial.print("CTRL OFF");
        // turn LED off:
        digitalWrite(ledPin, LOW);
      }
    }
  }
}

// First parameter is the event type (0x09 = note on, 0x08 = note off).
// Second parameter is note-on/note-off, combined with the channel.
// Channel can be anything between 0-15. Typically reported to the user as 1-16.
// Third parameter is the note number (48 = middle C).
// Fourth parameter is the velocity (64 = normal, 127 = fastest).

void noteOn(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOn = {0x09, 0x90 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOn);
}

void noteOff(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOff = {0x08, 0x80 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOff);
}
