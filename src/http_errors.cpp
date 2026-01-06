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

void handleHttpError(Inkplate &d, int httpCode)
{
    String errorMessage = getHttpErrorMessage(httpCode);
    drawErrorMessage(d, errorMessage);
}
