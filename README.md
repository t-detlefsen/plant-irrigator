# DIY Automated Plant Irrigator
This repository is for the automated watering of a single plant using an ESP32 communicating with a Raspberry Pi over MQTT. The system can be controlled and monitorred using a web interface using node-red.

This branch contains the the code to be deployed onto the arduino.

### Bill of Materials

| Part  | Item Number | Quantity |
| ------------- | ------------- | ------------- |
| Raspberry Pi 3 Model B | N/A | 1 |
| HiLetgo ESP32 | ESP-WROOM-32 | 1 |
| Temperature and Humidity Sensor | AHT20 | 1 |
| Capacitive Moisture Sensor | N/A | 1 |
| 5V Micro Submersible Mini Pump | N/A | 1 |
| 1/4" PVC Tubing | N/A | 1 |

### File Structure

```
├── plant-irrigator
│   ├── README
│   ├── plant-irrigator
│   │   ├── plant-irrigator.ino
```

### Next Steps
- Write temperature + moisture data to file
- Determine set-point for moisture sensor
- Setup VS Code IDE

### Resources
- [ESP32 MQTT – Publish and Subscribe with Arduino IDE](https://randomnerdtutorials.com/esp32-mqtt-publish-subscribe-arduino-ide/)