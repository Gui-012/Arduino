// File: DHT11.h
// Author: Guilherme do Amaral Caldas
// Library for the DHT11 Temperature and Humidity Sensor
typedef unsigned char uint8_t;

#define TIMEOUT_TIME 20
#define START_SIGNAL_HIGH 1800
#define START_SIGNAL_LOW 3
#define BYTE_SIZE 8
#define MESSAGE_SIZE 5

uint8_t read_dht(const uint8_t data_pin, int* data);
// Reads the data from the sensor. DTH data pin (0-7) provided as the data_pin.
// data should be a buffer int array with 2 ints.
// Temperature will be stored in data[0] and Humidity in data[1]
// Uses Timer0 and OCR0A