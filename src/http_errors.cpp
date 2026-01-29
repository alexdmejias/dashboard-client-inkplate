#include <ArduinoJson.h>
#include "http_errors.h"
#include "draw.h"

String getHttpErrorMessage(int httpCode)
{
    // Handle negative error codes (HTTPClient library errors)
    switch (httpCode)
    {
    case HTTPC_ERROR_CONNECTION_REFUSED:
        return "Connection refused by server";
    case HTTPC_ERROR_SEND_HEADER_FAILED:
        return "Failed to send request headers";
    case HTTPC_ERROR_SEND_PAYLOAD_FAILED:
        return "Failed to send request data";
    case HTTPC_ERROR_NOT_CONNECTED:
        return "Not connected to server";
    case HTTPC_ERROR_CONNECTION_LOST:
        return "Connection lost to server";
    case HTTPC_ERROR_NO_STREAM:
        return "No data stream available";
    case HTTPC_ERROR_NO_HTTP_SERVER:
        return "Server is not HTTP server";
    case HTTPC_ERROR_TOO_LESS_RAM:
        return "Out of memory";
    case HTTPC_ERROR_ENCODING:
        return "Invalid data encoding";
    case HTTPC_ERROR_STREAM_WRITE:
        return "Failed to write data";
    case HTTPC_ERROR_READ_TIMEOUT:
        return "Timeout reading from server";
    default:
        break;
    }

    // Handle standard HTTP status codes
    if (httpCode >= 400 && httpCode < 500)
    {
        // Client errors
        switch (httpCode)
        {
        case 400:
            return "Bad request";
        case 401:
            return "Unauthorized";
        case 403:
            return "Forbidden";
        case 404:
            return "Not found";
        case 408:
            return "Request timeout";
        default:
            return "Client error: " + String(httpCode);
        }
    }
    else if (httpCode >= 500 && httpCode < 600)
    {
        // Server errors
        switch (httpCode)
        {
        case 500:
            return "Internal server error";
        case 502:
            return "Bad gateway";
        case 503:
            return "Service unavailable";
        case 504:
            return "Gateway timeout";
        default:
            return "Server error: " + String(httpCode);
        }
    }

    // Generic error for unknown codes
    return "HTTP error: " + String(httpCode);
}

String getHttpErrorDebugHint(int httpCode)
{
    // Handle negative error codes (HTTPClient library errors)
    switch (httpCode)
    {
    case HTTPC_ERROR_CONNECTION_REFUSED:
    case HTTPC_ERROR_NOT_CONNECTED:
    case HTTPC_ERROR_CONNECTION_LOST:
        return "Check server address in config.txt";
    case HTTPC_ERROR_READ_TIMEOUT:
        return "Server may be slow or unreachable";
    case HTTPC_ERROR_NO_HTTP_SERVER:
        return "Verify server URL is correct";
    case HTTPC_ERROR_TOO_LESS_RAM:
        return "Image may be too large";
    default:
        break;
    }

    // Handle standard HTTP status codes
    if (httpCode == 401)
    {
        return "Check authentication credentials";
    }
    else if (httpCode == 404)
    {
        return "Verify server endpoint exists";
    }
    else if (httpCode == 408 || httpCode == 504)
    {
        return "Server may be slow or unreachable";
    }
    else if (httpCode >= 500 && httpCode < 600)
    {
        return "Server issue - try again later";
    }

    // Default hint
    return "Check network and server settings";
}

void handleHttpError(Inkplate &d, int httpCode, const String &responseBody)
{
    String fullMessage;
    String debugHint;

    bool usedServerErrorJson = false;

    // Only try to parse JSON for HTTP error status codes
    if (httpCode >= 400 && responseBody.length() > 0)
    {
        StaticJsonDocument<256> doc;
        DeserializationError err = deserializeJson(doc, responseBody);

        if (!err)
        {
            String serverError = doc["error"] | "";
            String serverMessage = doc["message"] | "";
            int statusCode = doc["statusCode"] | 0;

            if (serverError.length() > 0 || serverMessage.length() > 0)
            {
                // Compose rich message from server fields
                fullMessage = "";
                if (serverError.length() > 0)
                {
                    fullMessage += "Error: " + serverError;
                }
                if (statusCode > 0)
                {
                    fullMessage += " | Status Code: " + String(statusCode);
                }

                // Use server's .message as the debug hint, else fall back to our default below
                if (serverMessage.length() > 0)
                {
                    debugHint = serverMessage;
                }
                usedServerErrorJson = true;
            }
        }
    }

    if (!usedServerErrorJson)
    {
        // Fallback: show generic error info
        fullMessage = getHttpErrorMessage(httpCode);
        debugHint = getHttpErrorDebugHint(httpCode);
    }

    drawErrorMessage(d, fullMessage, debugHint);
}
