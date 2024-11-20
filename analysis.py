import serial
import time
import matplotlib.pyplot as plt
from collections import deque
import math

# Define thresholds for anomalies
HEART_RATE_LOW = 50     # beats per minute
HEART_RATE_HIGH = 100   # beats per minute
TEMP_LOW = 35.0         # degrees Celsius
TEMP_HIGH = 37.5        # degrees Celsius
MOVEMENT_THRESHOLD = 0.1  # Minimum movement intensity to consider active
REST_PERIOD_THRESHOLD = 30  # Seconds of inactivity to trigger alert

# Initialize serial connection
SERIAL_PORT = 'COM3'  # Replace with your Arduino's serial port
BAUD_RATE = 9600      # Ensure this matches the Arduino's baud rate

# Data storage for plotting
heart_rate_data = deque(maxlen=30)
temperature_data = deque(maxlen=30)
movement_data = deque(maxlen=30)
timestamps = deque(maxlen=30)

last_movement_time = time.time()  # Track the last time significant movement was detected

def read_data_from_arduino(serial_connection):
    """
    Reads data from the Arduino and parses it into heart rate, temperature, and accelerometer values.
    """
    try:
        line = serial_connection.readline().decode('utf-8').strip()
        if line.startswith("HR:") and "TEMP:" in line and "ACC:" in line:
            parts = line.split(',')
            heart_rate = int(parts[0].split(':')[1])
            temperature = float(parts[1].split(':')[1])
            acc_values = parts[2].split(':')[1].split('|')
            acc_x = float(acc_values[0])
            acc_y = float(acc_values[1])
            acc_z = float(acc_values[2])
            return heart_rate, temperature, acc_x, acc_y, acc_z
    except Exception as e:
        print(f"Error parsing data: {e}")
    return None, None, None, None, None

def detect_anomalies(heart_rate, temperature):
    """
    Checks for anomalies in heart rate and temperature.
    """
    anomalies = []
    if not (HEART_RATE_LOW <= heart_rate <= HEART_RATE_HIGH):
        anomalies.append(f"Heart rate anomaly: {heart_rate} BPM")
    if not (TEMP_LOW <= temperature <= TEMP_HIGH):
        anomalies.append(f"Temperature anomaly: {temperature} °C")
    return anomalies

def compute_movement_intensity(acc_x, acc_y, acc_z):
    """
    Calculates the movement intensity based on accelerometer readings.
    """
    return math.sqrt(acc_x**2 + acc_y**2 + acc_z**2)

def plot_data():
    """
    Plots the heart rate, temperature, and movement intensity data.
    """
    plt.figure(figsize=(12, 8))
    
    # Plot heart rate
    plt.subplot(3, 1, 1)
    plt.plot(timestamps, heart_rate_data, label='Heart Rate (BPM)', color='blue', marker='o')
    plt.axhline(HEART_RATE_LOW, color='red', linestyle='--', label='HR Lower Limit')
    plt.axhline(HEART_RATE_HIGH, color='red', linestyle='--', label='HR Upper Limit')
    plt.title('Heart Rate Over Time')
    plt.ylabel('Heart Rate (BPM)')
    plt.legend()
    plt.grid(True)
    
    # Plot temperature
    plt.subplot(3, 1, 2)
    plt.plot(timestamps, temperature_data, label='Temperature (°C)', color='green', marker='o')
    plt.axhline(TEMP_LOW, color='red', linestyle='--', label='Temp Lower Limit')
    plt.axhline(TEMP_HIGH, color='red', linestyle='--', label='Temp Upper Limit')
    plt.title('Body Temperature Over Time')
    plt.ylabel('Temperature (°C)')
    plt.legend()
    plt.grid(True)
    
    # Plot movement intensity
    plt.subplot(3, 1, 3)
    plt.plot(timestamps, movement_data, label='Movement Intensity', color='purple', marker='o')
    plt.axhline(MOVEMENT_THRESHOLD, color='red', linestyle='--', label='Activity Threshold')
    plt.title('Movement Intensity Over Time')
    plt.xlabel('Time (s)')
    plt.ylabel('Movement Intensity')
    plt.legend()
    plt.grid(True)
    
    plt.tight_layout()
    plt.show()

def main():
    global last_movement_time
    print("Starting heart rate, temperature, and movement monitoring...")
    start_time = time.time()
    try:
        with serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1) as arduino:
            time.sleep(2)  # Allow time for connection to stabilize
            while True:
                heart_rate, temperature, acc_x, acc_y, acc_z = read_data_from_arduino(arduino)
                if heart_rate is not None and temperature is not None:
                    current_time = time.time() - start_time
                    movement_intensity = compute_movement_intensity(acc_x, acc_y, acc_z)
                    
                    print(f"Heart Rate: {heart_rate} BPM, Temperature: {temperature} °C, Movement Intensity: {movement_intensity:.2f}")
                    
                    # Append data for plotting
                    heart_rate_data.append(heart_rate)
                    temperature_data.append(temperature)
                    movement_data.append(movement_intensity)
                    timestamps.append(round(current_time))
                    
                    # Detect anomalies
                    anomalies = detect_anomalies(heart_rate, temperature)
                    if anomalies:
                        print("Anomalies detected:")
                        for anomaly in anomalies:
                            print(f" - {anomaly}")
                    
                    # Check for inactivity
                    if movement_intensity > MOVEMENT_THRESHOLD:
                        last_movement_time = time.time()
                    elif time.time() - last_movement_time > REST_PERIOD_THRESHOLD:
                        print(f"Alert: Rest period too long! No significant movement for {REST_PERIOD_THRESHOLD} seconds.")
                    
                    # Plot data every 30 seconds
                    if len(timestamps) >= 30 and int(current_time) % 30 == 0:
                        plot_data()
                    
                time.sleep(1)
    except serial.SerialException as e:
        print(f"Error connecting to Arduino: {e}")
    except KeyboardInterrupt:
        print("Monitoring stopped.")

if __name__ == "__main__":
    main()
