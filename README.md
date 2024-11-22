# Real-Time Environmental Data Fetcher

This project is a C-based application designed to fetch real-time environmental data from an API, store it in a file, and calculate average values after every 12 readings. The automation ensures the program runs continuously and processes data seamlessly, requiring minimal user intervention.

## Features
- Fetches real-time data from a free API.
- Writes data to `weather_data.txt` for logging and analysis.
- Automatically calculates averages after 12 readings.
- Automates the entire process using a shell script.
- Operates continuously until the system is powered off.

## Prerequisites
To run this project, ensure you have the following installed:
- **Windows Subsystem for Linux (WSL)** with Ubuntu
- **gcc (GNU Compiler Collection)**


## Project Structure
- `fetch_api.c`: The main program to fetch and log data.
- `run_fetch.sh`: A shell script to automate the execution.
- `weather_data.txt`: The file where the data is stored.

## How It Works
1. **Start the program**: Run the command below in the WSL terminal.
   ```bash
   ./run_fetch.sh
