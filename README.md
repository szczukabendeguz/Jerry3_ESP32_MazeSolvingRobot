# Jerry 3.0 – ESP32 Maze-Solving Robot

An autonomous maze-solving robot built for the **"Mobile Robots in the Maze"** competition at **Óbuda University, Hungary**. This third-generation robot features a major upgrade from Arduino to ESP32, enabling WiFi-based real-time tuning, advanced sensor fusion, and parallel processing.

For more detailed documentation and project context, please visit: [ArduinoCompetition](https://github.com/szczukabendeguz/ArduinoCompetition)

## Tech Stack

[![Tech Stack](https://skillicons.dev/icons?i=cpp,arduino,vscode)](https://skillicons.dev)

**Hardware:** ESP32 WROOM-32 (Wemos D1 R32), MPU-6050, MFRC522 RFID, Sharp IR sensors, L298N motor driver  
**Software:** PlatformIO, Arduino framework, AsyncWebServer, ArduinoJson, PID control, Kalman filtering

---

## Project Goal

Jerry 3.0 is a compact **16×16 cm autonomous robot** designed to navigate and solve mazes using wall-following algorithms, RFID-based navigation, and gyroscope-assisted turns. The robot leverages the ESP32's **240 MHz dual-core processor** and built-in WiFi to provide:

- **Real-time web interface** for live sensor monitoring and PID tuning via WiFi (SoftAP mode)
- **Kalman-filtered IR sensors** for accurate wall distance measurement
- **RFID navigation** for directional guidance using maze tags
- **Gyroscope-assisted motion control** for precise 90° turns
- **Adaptive wall-following** that centers the robot between walls or follows a single wall

This project represents a significant evolution from our previous Arduino-based designs, with the ESP32's dual-core architecture enabling parallel sensor processing and web communication without performance bottlenecks.

---

## Getting Started

### Prerequisites

- [PlatformIO IDE](https://platformio.org/) (VS Code extension recommended)
- ESP32 board (Wemos D1 R32 or compatible)
- USB cable for programming

### Installation

1. **Clone the repository:**
   ```bash
   git clone https://github.com/szczukabendeguz/Jerry3_ESP32_MazeSolvingRobot.git
   cd Jerry3_ESP32_MazeSolvingRobot
   ```

2. **Open in PlatformIO:**
   - Open the project folder in VS Code with PlatformIO extension installed
   - PlatformIO will automatically install dependencies listed in `platformio.ini`

3. **Upload to ESP32:**
   ```bash
   pio run --target upload
   ```

4. **Monitor serial output:**
   ```bash
   pio device monitor
   ```

### Connecting to the Web Interface

1. After uploading, the ESP32 will create a WiFi access point (SoftAP mode)
2. Connect to the WiFi network: **SSID** and password will be displayed in serial monitor
3. Open a browser and navigate to the IP address shown (typically `192.168.4.1`)
4. Use the web interface to:
   - View live sensor data (IR distances, gyroscope angles)
   - Tune PID parameters in real-time
   - Switch between profiles (sprint mode, precision mode)
   - Start/stop the robot

---

## Architecture

### Key Components

- **Sensor Processing**  
  Reads from 3 Sharp IR distance sensors (front, left, right) and applies Kalman filtering to reduce noise and improve stability.

- **PID Control**  
  Implements wall-following logic: centers the robot when walls are detected on both sides, or maintains a constant distance from a single wall.

- **RFID Navigation**  
  MFRC522 module (SPI) reads directional tags placed in the maze, providing navigation instructions (forward, left, right, stop, start, dead-end).

- **Motion Control**  
  Two DC motors with L298N driver provide tank-style movement. MPU-6050 gyroscope measures rotation during turns for accurate 90° movements.

- **Web Interface**  
  ESP32's built-in AsyncWebServer hosts a real-time dashboard for monitoring and tuning without reprogramming.

### Wall-Following Algorithm

The adaptive wall-following logic handles three scenarios:

- **Both walls detected:** Centers the robot between walls using PID control
- **Single wall detected:** Maintains a set distance from the detected wall
- **No walls detected:** Relies on gyroscope data to maintain heading

---

## Environment Variables

No environment variables are required. Configuration is done through:

- **PID parameters:** Tunable via web interface or in `src/main.cpp`
- **Motor speeds:** Defined in `src/main.cpp` (min/max speeds, turn speeds)
- **Sensor calibration:** IR sensor models and Kalman filter parameters in `src/main.cpp`
- **WiFi credentials:** SoftAP SSID/password configured in `src/pid_webinterface.cpp`

---

## Testing

The project includes a web-based testing interface:

```bash
# Open test/test.html in a browser after connecting to the robot's WiFi
# This provides a visual dashboard for sensor data and control
```

For hardware testing:

1. Place the robot in a test maze
2. Connect to the WiFi interface
3. Monitor sensor readings and adjust PID parameters
4. Use the web interface to start the maze-solving routine

---

## What We Learned

- **ESP32 vs Arduino:** The upgrade to ESP32 provided a massive performance boost, enabling more complex algorithms and real-time WiFi tuning
- **Dual-core architecture:** Running sensor control on one core and web communication on the other eliminated performance bottlenecks
- **Kalman filtering:** Essential for stable IR sensor readings in a noisy environment
- **Real-time tuning:** WiFi-based parameter adjustment saved countless hours during testing and competition preparation

---

# Jerry 3.0 – ESP32 Labirintus-megoldó Robot

Autonóm labirintus-megoldó robot, amelyet az **Óbudai Egyetem** **"Mobilrobotok a labirintusban"** versenyére építettünk. Ez a harmadik generációs robot jelentős frissítést kapott: Arduinoról ESP32-re váltottunk, ami lehetővé teszi a WiFi-alapú valós idejű hangolást, fejlett szenzorfúziót és párhuzamos feldolgozást.

A projekt részletesebb kifejtése itt érhető el: [ArduinoCompetition](https://github.com/szczukabendeguz/ArduinoCompetition)

## Tech Stack

[![Tech Stack](https://skillicons.dev/icons?i=cpp,arduino,vscode)](https://skillicons.dev)

**Hardware:** ESP32 WROOM-32 (Wemos D1 R32), MPU-6050, MFRC522 RFID, Sharp IR szenzorok, L298N motor driver  
**Software:** PlatformIO, Arduino framework, AsyncWebServer, ArduinoJson, PID szabályozás, Kalman-szűrés

---

## Projekt célja

A Jerry 3.0 egy kompakt **16×16 cm-es autonóm robot**, amely falkövetési algoritmusok, RFID-alapú navigáció és giroszkóp-asszisztált fordulók segítségével navigál és old meg labirintusokat. A robot az ESP32 **240 MHz-es dual-core processzorát** és beépített WiFi-jét használja a következőkhöz:

- **Valós idejű web interface** élő szenzor-monitorozáshoz és PID hangoláshoz WiFi-n keresztül (SoftAP mód)
- **Kalman-szűrt IR szenzorok** pontos faltávolság-méréshez
- **RFID navigáció** irányított útmutatáshoz labirintus tag-ek használatával
- **Giroszkóp-asszisztált mozgásvezérlés** precíz 90°-os fordulókhoz
- **Adaptív falkövetés**, amely középre igazítja a robotot két fal között, vagy egyetlen falat követ

Ez a projekt jelentős evolúciót jelent korábbi Arduino-alapú terveinkhez képest, az ESP32 dual-core architektúrája lehetővé teszi a párhuzamos szenzorfeldolgozást és web kommunikációt teljesítményvesztés nélkül.

---

## Kezdő lépések

### Előfeltételek

- [PlatformIO IDE](https://platformio.org/) (VS Code extension ajánlott)
- ESP32 board (Wemos D1 R32 vagy kompatibilis)
- USB kábel programozáshoz

### Telepítés

1. **Repository klónozása:**
   ```bash
   git clone https://github.com/szczukabendeguz/Jerry3_ESP32_MazeSolvingRobot.git
   cd Jerry3_ESP32_MazeSolvingRobot
   ```

2. **Megnyitás PlatformIO-ban:**
   - Nyisd meg a projekt mappát VS Code-ban telepített PlatformIO extension-nel
   - A PlatformIO automatikusan telepíti a `platformio.ini`-ben felsorolt függőségeket

3. **Feltöltés ESP32-re:**
   ```bash
   pio run --target upload
   ```

4. **Serial output monitorozása:**
   ```bash
   pio device monitor
   ```

### Csatlakozás a web interface-hez

1. Feltöltés után az ESP32 létrehoz egy WiFi access point-ot (SoftAP mód)
2. Csatlakozz a WiFi hálózathoz: az **SSID** és jelszó megjelenik a serial monitorban
3. Nyiss meg egy böngészőt és navigálj a megjelenített IP címre (általában `192.168.4.1`)
4. Használd a web interface-t a következőkhöz:
   - Élő szenzor adatok megtekintése (IR távolságok, giroszkóp szögek)
   - PID paraméterek valós idejű hangolása
   - Profilok közötti váltás (sprint mód, precíziós mód)
   - Robot indítása/leállítása

---

## Architektúra

### Fő komponensek

- **Szenzor feldolgozás**  
  3 Sharp IR távolságszenzor (elülső, bal, jobb) olvasása és Kalman-szűrés alkalmazása a zaj csökkentésére és stabilitás javítására.

- **PID szabályozás**  
  Falkövetési logika implementálása: középre igazítja a robotot, ha mindkét oldalon fal van, vagy állandó távolságot tart egy faltól.

- **RFID navigáció**  
  MFRC522 modul (SPI) olvassa a labirintusban elhelyezett irányító tag-eket, navigációs utasításokat adva (előre, balra, jobbra, stop, start, zsákutca).

- **Mozgásvezérlés**  
  Két DC motor L298N driver-rel tank-szerű mozgást biztosít. Az MPU-6050 giroszkóp méri a forgást fordulók során a pontos 90°-os mozgásokhoz.

- **Web interface**  
  Az ESP32 beépített AsyncWebServer-e valós idejű dashboard-ot szolgáltat monitorozáshoz és hangoláshoz újraprogramozás nélkül.

### Falkövetési algoritmus

Az adaptív falkövetési logika három forgatókönyvet kezel:

- **Mindkét fal észlelve:** PID szabályozással középre igazítja a robotot a falak között
- **Egyetlen fal észlelve:** Beállított távolságot tart az észlelt faltól
- **Nincs fal észlelve:** Giroszkóp adatokra támaszkodik az irány megtartásához

---

## Environment variable-ök

Nincsenek szükséges environment variable-ök. A konfiguráció a következőkön keresztül történik:

- **PID paraméterek:** Web interface-en vagy a `src/main.cpp`-ben hangolhatók
- **Motor sebességek:** A `src/main.cpp`-ben definiálva (min/max sebességek, fordulási sebességek)
- **Szenzor kalibráció:** IR szenzor modellek és Kalman filter paraméterek a `src/main.cpp`-ben
- **WiFi hitelesítő adatok:** SoftAP SSID/jelszó a `src/pid_webinterface.cpp`-ben konfigurálva

---

## Tesztelés

A projekt tartalmaz egy web-alapú teszt interface-t:

```bash
# Nyisd meg a test/test.html fájlt böngészőben, miután csatlakoztál a robot WiFi-jéhez
# Ez egy vizuális dashboard-ot biztosít szenzor adatokhoz és vezérléshez
```

Hardware teszteléshez:

1. Helyezd a robotot egy teszt labirintusba
2. Csatlakozz a WiFi interface-hez
3. Monitorozd a szenzor leolvasásokat és állítsd be a PID paramétereket
4. Használd a web interface-t a labirintus-megoldó rutin indításához