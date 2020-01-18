/**
 * PoC for a standalone library dependent on SPI only
 * to control ST7789 lcd w/o CS pin
 * it uses hardware spi
 *
 * currently the screen is rotated by coordinate manipulation
 * to avoid the need for offsets and possible uint16_t
 * correct view: pins of the lcd on the right side
 *
 * it's meant to be ported to avr c-code
 *
 * inspired by
 *  - https://github.com/ananevilya/Arduino-ST7789-Library
 *  - http://www.technoblogy.com/show?L6I
 *    (library on http://www.technoblogy.com/list?2LJ2)
 */

#include <Arduino.h>
#include <SPI.h>
#include "base_functions.h"
#include "st7789_cmds.h"

#define RST CS0 // reset (works as cs as well, as no cs available)
#define BLK IO1 // back light
#define DC IO2  // data/command

const SPISettings settings(24000000, MSBFIRST, SPI_MODE3);
const int ysize = 240, xsize = 240, yoffset = 0, xoffset = 0, invert = 1, rotate = 0;

uint8_t x_base = 0, y_base = 0;

void send_data(uint8_t data) {
  // DF("dat: 0x%02x\n", data);
  digitalWrite(DC, 1);
  SPI.beginTransaction(settings);
  SPI.transfer(data);
  SPI.endTransaction();
}

void send_command(uint8_t cmd) {
  // DF("cmd: 0x%02x\n", cmd);
  digitalWrite(DC, 0);
  SPI.beginTransaction(settings);
  SPI.transfer(cmd);
  SPI.endTransaction();
}

void write_multi_data(int* buf, uint8_t len) {
  digitalWrite(DC, 1);
  SPI.beginTransaction(settings);
  for (uint8_t i=0; i<len; i++) {
    SPI.transfer(buf[i]);
  }
  SPI.endTransaction();
}

/*
 * we assume the coordinates can't get larger than  255!
 * if > 255 we would need to use uint16_t:
 *   send_data(x0 >> 8);
 *   send_data(x0 & 0xFF);
 *
 */
void set_addr_window(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1) {
  send_command(ST7789_CASET); // column
  send_data(0);
  send_data(x0);
  send_data(0);
  send_data(x1);

  send_command(ST7789_RASET); // row
  send_data(0);
  send_data(y0);
  send_data(0);
  send_data(y1);

  send_command(ST7789_RAMWR); // write to RAM
}

/*
 * convert rgb color to uint16_t value
 */
uint16_t convert_color(uint8_t r, uint8_t g, uint8_t b) {
  return (r & 0xf8)<<8 | (g & 0xfc)<<3 | b>>3;
}

void draw_point(uint8_t x, uint8_t y, uint16_t color) {
  // swap coordinates for now
  y = (x+y)-(x=y); // swap x, y
  y = ST7789_TFTHEIGHT - y;
  x = ST7789_TFTWIDTH - x;

  send_command(ST7789_RASET);
  int buf_y[4] = { 0, yoffset+y, 0, yoffset+y };
  write_multi_data(buf_y, 4);

  send_command(ST7789_CASET);
  int buf_x[4] = { 0, xoffset+x, 0, xoffset+x };
  write_multi_data(buf_x, 4);

  send_command(ST7789_RAMWR);
  send_data(color >> 8);
  send_data(color & 0xff);
}

void fill_rect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint16_t color) {
  h = (w+h)-(w=h); // swap w, h
  y = (x+y)-(x=y); // swap x, y
  y = ST7789_TFTHEIGHT - y - h;
  x = ST7789_TFTWIDTH - x - w;

  if ((x >= ST7789_TFTWIDTH) || (y >= ST7789_TFTHEIGHT)) return;

  if ((x + w -1) >= ST7789_TFTWIDTH) w = ST7789_TFTWIDTH - x;
  if ((y + h -1) >= ST7789_TFTHEIGHT) w = ST7789_TFTHEIGHT - y;

  set_addr_window(x, y, x+w-1, y+h-1);

  uint8_t hi = color >> 8, lo = color & 0xFF;

  digitalWrite(DC, 1);
  SPI.beginTransaction(settings);
  for (y=h; y>0; y--) {
    for (x=w; x>0; x--) {
      SPI.transfer(hi);
      SPI.transfer(lo);
    }
  }
  SPI.endTransaction();
}

void fill_screen(uint16_t color) {
  fill_rect(0, 0, ST7789_TFTWIDTH, ST7789_TFTHEIGHT, color);
}

/*
 * draw line according to bresenham's line algorithm
 */
void draw_line(uint8_t x_from, uint8_t y_from, uint8_t x_to, uint8_t y_to, uint16_t color) {
  int sx, sy, e2, err;
  int dx = abs(x_to - x_from);
  int dy = abs(y_to - y_from);
  if (x_from < x_to) sx = 1; else sx = -1;
  if (y_from < y_to) sy = 1; else sy = -1;
  err = dx - dy;
  for (;;) {
    draw_point(x_from, y_from, color);
    if (x_from==x_to && y_from==y_to) return;
    e2 = err<<1;
    if (e2 > -dy) { err = err - dy; x_from = x_from + sx; }
    if (e2 < dx) { err = err + dx; y_from = y_from + sy; }
  }
}

/*
 * initialize screen
 * startup sequence:
 *   cmd: 0x01
 *   cmd: 0x11
 *   cmd: 0x3a
 *   dat: 0x05
 *   cmd: 0x21
 *   cmd: 0x36
 *   dat: 0x00
 *   cmd: 0x29
 */
void init() {
  SPI.begin(SCK, MISO, MOSI);
  // reset
  digitalWrite(RST, HIGH);
  delay(50);
  digitalWrite(RST, LOW);
  delay(50);
  digitalWrite(RST, HIGH);
  delay(50);

  digitalWrite(DC, 1); // data/command high per default

  send_command(0x01); // software reset
  delay(150);
  send_command(0x11); // out of sleep mode
  delay(500);
  send_command(0x3a);
  send_data(0x05); // color mode 16bit
  send_command(0x20+invert); // invert
  send_command(0x36);
  send_data(rotate<<5); // MADCTL
  send_command(0x29); // display on
  delay(100);
}

void setup() {
  pinMode(LED, OUTPUT);
  pinMode(BLK, OUTPUT);
  pinMode(DC, OUTPUT);
  pinMode(RST, OUTPUT);

  // uart
  Serial.begin(115200);
  Serial.setTimeout(2000);
  while(!Serial) { }

  DL("Hello there.");
  blink(1, 100);

  // DF("color: 0x%04x\n", convert_color(255, 0, 0));

  init();

  // clear and turn on screen background light
  fill_screen(MAGENTA);
  digitalWrite(BLK, 1); // turn on backlight

  for(uint8_t i=0; i<240; i++) {
    draw_point(i, 120, GREEN);
  }

  uint8_t x0=0, y0=0, x1=100, y1=50, w=100, h=50;
  fill_rect(x0,y0,w,h,YELLOW);
  draw_line(x0,y0,x1,y1,BLACK);

  DL("done");
}

void loop() {
}
