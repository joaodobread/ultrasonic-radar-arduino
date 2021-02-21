#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <SPI.h>
#include <Adafruit_GrayOLED.h>
#include <Adafruit_PCD8544.h>
#include <Servo.h>
#include <Ultrasonic.h>
#include <stdio.h>

#define HC_SR04_TRIGGER A2
#define HC_SR04_ECHO A3
Servo servo;
Adafruit_PCD8544 display = Adafruit_PCD8544(7, 6, 5, 4, 3);

Ultrasonic ultrasonic(HC_SR04_TRIGGER, HC_SR04_ECHO);

unsigned long objects[180] = {};

unsigned long timing();
unsigned long calculate_distance();
void draw_dial(int angle);
void draw_object_line(int value, int angle);

void clear_objects();
void setup_display();
void ultrasonic_radar();

void setup()
{
  Serial.begin(9600);
  clear_objects();
  servo.attach(9);
  setup_display();
  delay(1000);
}

void loop()
{
  ultrasonic_radar();
}

void clear_objects()
{
  memset(objects, 0, sizeof(objects));
}

unsigned long calculate_distance()
{
  unsigned long microsec = timing();
  return microsec * 0.034 / 2;
}

unsigned long timing()
{
  digitalWrite(HC_SR04_TRIGGER, LOW);
  delayMicroseconds(2);
  digitalWrite(HC_SR04_TRIGGER, HIGH);
  delayMicroseconds(20);
  digitalWrite(HC_SR04_TRIGGER, LOW);
  return pulseIn(HC_SR04_ECHO, HIGH);
}

void draw_object_line(int value, int angle)
{
  int x0 = 42 - 41 * cos(angle * 3.14 / 180);
  int y0 = 48 - 41 * sin(angle * 3.14 / 180);
  int x1 = 42 - value * cos(angle * 3.14 / 180);
  int y1 = 48 - value * sin(angle * 3.14 / 180);
  display.drawLine(x1, y1, x0, y0, BLACK);
}

void draw_dial(int angle)
{
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.write("Angle: ");
  display.print(angle);
  display.endWrite();
  display.drawCircle(42, 48, 41, BLACK);
  display.drawCircle(42, 48, 31, BLACK);
  display.drawCircle(42, 48, 21, BLACK);
  display.drawCircle(42, 48, 11, BLACK);
  int x = 42 - 41 * cos(angle * 3.14 / 180);
  int y = 48 - 41 * sin(angle * 3.14 / 180);
  display.drawLine(42, 48, x, y, BLACK);
}

void setup_display()
{
  display.begin();
  display.setContrast(60);
  delay(2000);
  display.clearDisplay();
}

void setup_pins()
{
  pinMode(HC_SR04_TRIGGER, OUTPUT);
  pinMode(HC_SR04_ECHO, INPUT);
}

void ultrasonic_radar()
{
  for (int16_t i = 0; i < 180; ++i)
  {
    servo.write(180 - i);
    display.clearDisplay();
    draw_dial(i);
    unsigned long distance = calculate_distance();

    if (distance < 41)
      objects[i] = distance;
    else
      objects[i] = 0;

    int pos = i - 30;
    if (pos < 0)
      pos = 0;
    for (int k = pos; k < i; k++)
      if (objects[k])
        draw_object_line(objects[k], k);
    display.display();
    delay(15);
  }
  clear_objects();

  display.clearDisplay();
  for (int i = 180; i > 0; i--)
  {
    servo.write(180 - i);
    display.clearDisplay();
    draw_dial(i);
    unsigned long distance = calculate_distance();
    if (distance < 41)
      objects[i] = distance;
    else
      objects[i] = 0;

    int pos = i + 30;
    if (pos > 179)
      pos = 179;
    for (int k = pos; k > i; k--)
      if (objects[k])
        draw_object_line(objects[k], k);

    display.display();
    delay(15);
  }
  clear_objects();
}