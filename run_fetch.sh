#!/bin/bash

# Define file paths
counter_file="counter.txt"
weather_file="weather_data.txt"
average_file="weather_averages.txt"

# Email recipient
EMAIL_RECIPIENT="ridabatool522006@gmail.com"  # Replace with your email

# Temperature thresholds
TEMP_HIGH_THRESHOLD=30.0  # Hot temperature threshold
TEMP_LOW_THRESHOLD=11.0   # Cold temperature threshold

# Initialize counter file if it doesn't exist
if [ ! -f "$counter_file" ]; then
    echo 0 > "$counter_file"
fi

# Infinite loop to run every 5 seconds
while true; do
    # Run the C program to fetch data
    ./fetch_api

    # Increment counter
    count=$(<"$counter_file")
    count=$((count + 1))
    echo "$count" > "$counter_file"

    # Check if we reached 12 readings
    if [ "$count" -eq 12 ]; then
        # Initialize variables for averaging
        total_temp=0
        total_humidity=0
        total_wind=0
        temp_count=0
        humidity_count=0
        wind_count=0

        # Process each line in the weather data file
        while IFS= read -r line; do
            case $line in
                "Temperature:"*)
                    temp_value=$(echo "$line" | grep -oE '[0-9]+(\.[0-9]+)?')
                    if [[ "$temp_value" =~ ^[0-9]+(\.[0-9]+)?$ ]]; then
                        total_temp=$(echo "$total_temp + $temp_value" | bc)
                        temp_count=$((temp_count + 1))
                    fi
                    ;;
                "Humidity:"*)
                    humidity_value=$(echo "$line" | grep -oE '[0-9]+(\.[0-9]+)?')
                    if [[ "$humidity_value" =~ ^[0-9]+(\.[0-9]+)?$ ]]; then
                        total_humidity=$(echo "$total_humidity + $humidity_value" | bc)
                        humidity_count=$((humidity_count + 1))
                    fi
                    ;;
                "Wind Speed:"*)
                    wind_value=$(echo "$line" | grep -oE '[0-9]+(\.[0-9]+)?')
                    if [[ "$wind_value" =~ ^[0-9]+(\.[0-9]+)?$ ]]; then
                        total_wind=$(echo "$total_wind + $wind_value" | bc)
                        wind_count=$((wind_count + 1))
                    fi
                    ;;
            esac
        done < "$weather_file"

        # Calculate averages
        avg_temp="N/A"
        avg_humidity="N/A"
        avg_wind="N/A"

        if [ "$temp_count" -gt 0 ]; then
            avg_temp=$(echo "$total_temp / $temp_count" | bc -l)
            avg_temp=$(printf "%.2f" "$avg_temp")  # Ensure 2 decimal places for comparison
        fi
        if [ "$humidity_count" -gt 0 ]; then
            avg_humidity=$(echo "$total_humidity / $humidity_count" | bc -l)
            avg_humidity=$(printf "%.2f" "$avg_humidity")
        fi
        if [ "$wind_count" -gt 0 ]; then
            avg_wind=$(echo "$total_wind / $wind_count" | bc -l)
            avg_wind=$(printf "%.2f" "$avg_wind")
        fi

        # Append averages to the output file
        {
            echo "Averages for Last 12 entries:"
            echo "Average Temperature: $avg_temp °C"
            echo "Average Humidity: $avg_humidity%"
            echo "Average Wind Speed: $avg_wind m/s"
            echo "-----------------------------------"
        } >> "$average_file"

        # Generate alerts for temperature
        if (( $(echo "$avg_temp > $TEMP_HIGH_THRESHOLD" | bc -l) )); then
            # Email alert for hot temperature
            echo -e "Subject:Temperature Alert: Very Hot\n\nAlert: It is very hot! Average Temperature: $avg_temp °C" | \
            msmtp "$EMAIL_RECIPIENT"
        elif (( $(echo "$avg_temp < $TEMP_LOW_THRESHOLD" | bc -l) )); then
            # Email alert for cold temperature
            echo -e "Subject:Temperature Alert: Very Cold\n\nAlert: It is very cold! Average Temperature: $avg_temp °C" | \
            msmtp "$EMAIL_RECIPIENT"
        fi

        # Reset counter and clear weather data
        echo 0 > "$counter_file"
        > "$weather_file"
    fi

    # Wait for 5 seconds before the next iteration
    sleep 5
done

