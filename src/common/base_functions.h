#ifndef ESP32DEVBOARD_BASE_FUNCTIONS
#define ESP32DEVBOARD_BASE_FUNCTIONS

// debug macros
// to output Strings use <string>.c_str()
#define D(x) Serial.print(x)
#define DL(x) {Serial.println(x);}
#define DF(...) Serial.printf(__VA_ARGS__);

// numbers are GPIO numbers

#define LED           17

#define ROTARY_BUTTON 37
#define ROTARY_A      38
#define ROTARY_B      36
#define BUTTON        39

// I2C
#define SCL           22
#define SDA           21

// IR
#define IR_RECEIVE_EN 2
#define IR_RECEIVE    34 // shared MPU_INT, default IR_RECEIVE (see SJ13)
#define IR_SEND       19

// MPU-6050 Gyro Accelerometer
#define MPU_INT       34 // shared with IR_RECEIVE, default IR_RECEIVE (see SJ13)

// SPI
#define MOSI          13
#define MISO          12
#define SCK           14
#define CS0           15
#define IO1           16
#define IO2           4

// RFM69HCW (SPI)
#define RFM_CS        18
#define RFM_INT       35

// SDCARD (SPI)
#define SDCARD_CS     5

// I2S
#define I2S_LRCK      25 // also known as WCLK
#define I2S_SCK       26 // also known as BCLK
#define I2S_DIN       27
#define I2S_EN        23


void blink(int amount=3, int duration=100);

#define WIFI_TIMEOUT 10e3
extern char nodename[20];
int connect_to_wifi();

#endif
