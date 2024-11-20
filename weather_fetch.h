// weather_fetch.h

#ifndef WEATHER_FETCH_H
#define WEATHER_FETCH_H

#include <curl/curl.h>
#include <json-c/json.h>

#define API_KEY "ab583c13cc6ee6edafb1c0a0a60adab1"
#define CITY "London"
#define OUTPUT_FILE "weather_data.txt"

// Structure to store the response data
struct Memory {
    char *response;
    size_t size;
};

// Function declarations
size_t write_callback(void *data, size_t size, size_t nmemb, void *userp);
void fetch_data();

#endif // WEATHER_FETCH_H
