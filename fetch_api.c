#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "weather_fetch.h"  // Include the header file for modularization

// Callback function to handle incoming data
size_t write_callback(void *data, size_t size, size_t nmemb, void *userp) {
    size_t total_size = size * nmemb;
    struct Memory *mem = (struct Memory *)userp;

    char *ptr = realloc(mem->response, mem->size + total_size + 1);
    if (ptr == NULL) {
        fprintf(stderr, "Not enough memory for response\n");
        return 0;
    }

    mem->response = ptr;
    memcpy(&(mem->response[mem->size]), data, total_size);
    mem->size += total_size;
    mem->response[mem->size] = 0; // Null-terminate the string

    return total_size;
}

void fetch_data() {
    CURL *curl;
    CURLcode res;
    long http_code = 0;
    static int reading_count = 0; // Declare and initialize the counter

    struct Memory chunk = { .response = NULL, .size = 0 };

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if (curl) {
        char url[256];
        snprintf(url, sizeof(url), "http://api.openweathermap.org/data/2.5/weather?q=%s&appid=%s&units=metric", CITY, API_KEY);

        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
        curl_easy_setopt(curl, CURLOPT_FAILONERROR, 1L);  // Fail on HTTP errors

        res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        } else {
            // Get the HTTP response code
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

            if (http_code == 200) { // Check if the response code is OK
                printf("Code is Working\n");

                // Parse JSON data
                struct json_object *parsed_json;
                struct json_object *main;
                struct json_object *wind;
                struct json_object *temperature;
                struct json_object *humidity;
                struct json_object *wind_speed;

                // Parse the JSON response
                parsed_json = json_tokener_parse(chunk.response);
                json_object_object_get_ex(parsed_json, "main", &main);
                json_object_object_get_ex(main, "temp", &temperature);
                json_object_object_get_ex(main, "humidity", &humidity);
                json_object_object_get_ex(parsed_json, "wind", &wind);
                json_object_object_get_ex(wind, "speed", &wind_speed);

                // Write data to a text file
                FILE *file = fopen(OUTPUT_FILE, "a");
                if (file) {
                    fprintf(file, "Weather Data:\n");
                    fprintf(file, "Temperature: %.2f °C\n", json_object_get_double(temperature));
                    fprintf(file, "Humidity: %d%%\n", json_object_get_int(humidity));
                    fprintf(file, "Wind Speed: %.2f m/s\n", json_object_get_double(wind_speed));
                    fclose(file);
                    printf("Weather data written to %s\n", OUTPUT_FILE);

                    // Increment the reading count
                    reading_count++;

                    // Check if we have 12 readings
                    if (reading_count == 12) {
                        // Call the script to calculate average after 12 readings
                        system("/home/rida_batool/CCeep/run_fetch.sh"); // Call your average calculation script
                        reading_count = 0; // Reset the counter after calling the script
                    }
                } else {
                    fprintf(stderr, "Failed to open file for writing\n");
                }

                json_object_put(parsed_json); // Free the parsed JSON object
            } else {
                fprintf(stderr, "API Error: Received HTTP %ld\n", http_code);
            }
        }

        free(chunk.response);
        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();
}

int main() {
    fetch_data();
    return 0;
}
