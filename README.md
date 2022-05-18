# DIY Automated Plant Irrigator
This is a repository for an automated plant irrigator.

### File Structure

```
├── plant-irrigator
│   ├── README
│   ├── software
│   │   ├── ...
│   ├── hardware
│   │   ├── ...
```

### Bill of Materials

| Part  | Item Number | Quantity |
| ------------- | ------------- | ------------- |
| Raspberry Pi 3 Model B | N/A | 1 |
| HiLetgo ESP32 | ESP-WROOM-32 | 1 |
| Temperature and Humidity Sensor | AHT20 | 1 |
| Capacitive Moisture Sensor | N/A | 1 |

### To-Do's
- Test Moisture Sensor over long periods to determine constants
- Track soil moisture over time
- Connect pump to ESP32 and issue commands via MQTT
- Make electronic schematic (sensor + pump => Arduino => Pi)
- Reorganize GitHub
