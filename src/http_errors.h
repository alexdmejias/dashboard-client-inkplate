#pragma once

#include "Arduino.h"
#include "Inkplate.h"
#include "HTTPClient.h"

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

/**
 * Get a debug hint for a given HTTP error code
 * @param httpCode The HTTP response code or error code
 * @return Debug hint string to help users diagnose the issue
 */
String getHttpErrorDebugHint(int httpCode);
