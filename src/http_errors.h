#pragma once

#include "Arduino.h"
#include "Inkplate.h"

// ESP32 HTTPClient error codes
// Source: https://github.com/espressif/arduino-esp32/blob/master/libraries/HTTPClient/src/HTTPClient.h
const int HTTPC_ERROR_CONNECTION_REFUSED = -1;
const int HTTPC_ERROR_SEND_HEADER_FAILED = -2;
const int HTTPC_ERROR_SEND_PAYLOAD_FAILED = -3;
const int HTTPC_ERROR_NOT_CONNECTED = -4;
const int HTTPC_ERROR_CONNECTION_LOST = -5;
const int HTTPC_ERROR_NO_STREAM = -6;
const int HTTPC_ERROR_NO_HTTP_SERVER = -7;
const int HTTPC_ERROR_TOO_LESS_RAM = -8;
const int HTTPC_ERROR_ENCODING = -9;
const int HTTPC_ERROR_STREAM_WRITE = -10;
const int HTTPC_ERROR_READ_TIMEOUT = -11;

/**
 * Handle HTTP errors and display appropriate user-friendly messages
 * @param d Inkplate display reference
 * @param httpCode The HTTP response code or error code
 */
void handleHttpError(Inkplate &d, int httpCode);

/**
 * Get a user-friendly error message for a given HTTP error code
 * @param httpCode The HTTP response code or error code
 * @return User-friendly error message string
 */
String getHttpErrorMessage(int httpCode);
