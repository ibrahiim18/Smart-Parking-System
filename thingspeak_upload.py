import serial
import time
import requests

# Configure Serial Port (Update 'COM3' to your Arduino's port)
SERIAL_PORT = 'COM3'  # Change to your Arduino's port (e.g., /dev/ttyUSB0 on Linux)
BAUD_RATE = 9600      # Match the baud rate in Arduino code

# ThingSpeak Configuration
THINGSPEAK_API_KEY = '5JMD7PY6EAEKO29C'  # Replace with your ThingSpeak Write API Key
THINGSPEAK_URL = 'https://api.thingspeak.com/update'

def send_to_thingspeak(spot1, spot2, gate):
    """
    Send data to ThingSpeak
    """
    payload = {
        'api_key': THINGSPEAK_API_KEY,
        'field1': spot1,
        'field2': spot2,
        'field3': gate
    }
    try:
        response = requests.post(THINGSPEAK_URL, data=payload)
        if response.status_code == 200:
            print(f"Data sent to ThingSpeak: Spot1={spot1}, Spot2={spot2}, Gate={gate}")
        else:
            print(f"Failed to send data to ThingSpeak. HTTP {response.status_code}")
    except Exception as e:
        print(f"Error: {e}")

def main():
    # Initialize Serial Connection
    try:
        arduino = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
        time.sleep(2)  # Allow time for Arduino to reset
        print("Connected to Arduino!")
    except Exception as e:
        print(f"Error connecting to Arduino: {e}")
        return

    while True:
        try:
            if arduino.in_waiting > 0:
                line = arduino.readline().decode('utf-8').strip()
                print(f"Received: {line}")

                if "Spot 1 Status" in line and "Spot 2 Status" in line and "Gate Status" in line:
                    try:
                        spot1_status = int(line.split("Spot 1 Status: ")[1].split(",")[0])
                        spot2_status = int(line.split("Spot 2 Status: ")[1].split(",")[0])
                        gate_status = int(line.split("Gate Status: ")[1])

                        send_to_thingspeak(spot1_status, spot2_status, gate_status)
                    except (IndexError, ValueError) as parse_error:
                        print(f"Error parsing data: {parse_error}")
                else:
                    print("Invalid data format")

            time.sleep(15) 
        except KeyboardInterrupt:
            print("Program stopped by user")
            break
        except Exception as e:
            print(f"Error: {e}")
            break

    arduino.close()

if __name__ == "__main__":
    main()
