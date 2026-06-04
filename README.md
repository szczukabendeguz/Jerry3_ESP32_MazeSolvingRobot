# Jerry 3.0 - ESP32 Maze-Solving Robot

An autonomous maze-solving robot built for the **Mobile Robots in the Maze** competition at **Obuda University, Hungary**. This third-generation robot started as an Arduino-based firmware and now runs on ESP32 with live tuning, improved sensor handling, and a modular codebase.

For more detailed documentation and project context, please visit: [ArduinoCompetition](https://github.com/szczukabendeguz/ArduinoCompetition)
For the current web tuning stack, see: [ESP32VariableTuner-public](https://github.com/szczukabendeguz/ESP32VariableTuner-public)
Project details and documentation are also available at: [teamjerry.hu](https://teamjerry.hu)

## Tech Stack

[![Tech Stack](https://skillicons.dev/icons?i=cpp,arduino,vscode)](https://skillicons.dev)

**Hardware:** ESP32 WROOM-32 (Wemos D1 R32), MPU-6050, MFRC522 RFID, Sharp IR sensors, L298N motor driver  
**Software:** PlatformIO, Arduino framework, ESP32VariableTuner, ESPAsyncWebServer, AsyncTCP, ArduinoJson, PID control, Kalman filtering

---

## Project Goal

Jerry 3.0 is a compact **16x16 cm autonomous robot** designed to navigate and solve mazes using wall-following algorithms, RFID-based navigation, and gyroscope-assisted turns. The robot leverages the ESP32's **240 MHz dual-core processor** and built-in WiFi to provide:

- **Live variable tuning** for PID and motion values via ESP32VariableTuner
- **Kalman-filtered IR sensors** for accurate wall distance measurement
- **RFID navigation** for directional guidance using maze tags
- **Gyroscope-assisted motion control** for precise 90 degree turns
- **Adaptive wall-following** that centers the robot between walls or follows a single wall
- **Modular firmware** split into audio, motion, RFID, sensor, and tuner modules

This project represents a significant evolution from our previous Arduino-based designs. The old monolithic custom web interface was replaced with ESP32VariableTuner, and the codebase was refactored to keep motion, sensing, audio, RFID, and tuning logic in separate files.

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
   - Open the project folder in VS Code with the PlatformIO extension installed
   - PlatformIO will automatically install dependencies listed in `platformio.ini`

3. **Upload to ESP32:**
   ```bash
   pio run --target upload
   ```

4. **Monitor serial output:**
   ```bash
   pio device monitor
   ```

### Connecting to the Tuning Interface

1. After uploading, the ESP32 creates a WiFi access point (SoftAP mode)
2. Connect to the WiFi network: the **SSID** and password will be displayed in the serial monitor
3. Open a browser and navigate to the IP address shown (typically `192.168.4.1`)
4. Use the ESP32VariableTuner UI to:
   - View live variable values
   - Tune PID parameters in real time
   - Adjust motion limits and wall-following thresholds
   - Monitor telemetry while the robot is running

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
  Two DC motors with L298N driver provide tank-style movement. MPU-6050 gyroscope measures rotation during turns for accurate 90 degree movements.

- **Live Tuning Interface**  
  ESP32VariableTuner hosts the current tuning dashboard and replaces the old custom web server.

### Wall-Following Algorithm

The adaptive wall-following logic handles three scenarios:

- **Both walls detected:** Centers the robot between walls using PID control
- **Single wall detected:** Maintains a set distance from the detected wall
- **No walls detected:** Relies on gyroscope data to maintain heading

---

## Environment Variables

No environment variables are required. Configuration is done through:

- **PID parameters:** Tunable via ESP32VariableTuner or in `src/main.cpp`
- **Motor speeds:** Defined in `src/main.cpp` and used by the motion module
- **Sensor calibration:** IR sensor models and Kalman filter parameters in `src/sensors.cpp`
- **WiFi credentials:** SoftAP SSID/password configured in `src/main.cpp`

---

## Testing

The project includes a tuning and testing interface through ESP32VariableTuner:

```bash
# Connect to the robot's WiFi network
# Open the tuner UI in a browser at the ESP32 IP address
```

For hardware testing:

1. Place the robot in a test maze
2. Connect to the WiFi tuning interface
3. Monitor sensor readings and adjust PID parameters
4. Use the live tuner to start the maze-solving routine

---

## What We Learned

- **ESP32 vs Arduino:** The upgrade to ESP32 provided a massive performance boost, enabling more complex algorithms and live tuning
- **Modular architecture:** Splitting the firmware into separate files made the code easier to maintain and extend
- **Kalman filtering:** Essential for stable IR sensor readings in a noisy environment
- **Live tuning:** ESP32VariableTuner saved time during testing and competition preparation

---

# Jerry 3.0 - ESP32 Labirintus-megoldo Robot

Autonom labirintus-megoldo robot, amelyet az **Obudai Egyetem** **"Mobilrobotok a labirintusban"** versenyere epitettunk. Ez a harmadik generacios robot Arduinorol ESP32-re valtott, es most mar ESP32VariableTuner-rel, jobb szenzorfeldolgozassal es modularis kodszerkezettel fut.

A projekt regebbi versenykontextusa itt erheto el: [ArduinoCompetition](https://github.com/szczukabendeguz/ArduinoCompetition)
A jelenlegi live tuning megoldas: [ESP32VariableTuner-public](https://github.com/szczukabendeguz/ESP32VariableTuner-public)
A projektről részletek és a dokumentáció itt érhető el: [teamjerry.hu](https://teamjerry.hu)

## Tech Stack

[![Tech Stack](https://skillicons.dev/icons?i=cpp,arduino,vscode)](https://skillicons.dev)

**Hardware:** ESP32 WROOM-32 (Wemos D1 R32), MPU-6050, MFRC522 RFID, Sharp IR szenzorok, L298N motor driver  
**Software:** PlatformIO, Arduino framework, ESP32VariableTuner, ESPAsyncWebServer, AsyncTCP, ArduinoJson, PID szabalyzas, Kalman-szures

---

## Projekt celja

A Jerry 3.0 egy kompakt **16x16 cm-es autonóm robot**, amely falkovetesi algoritmusok, RFID-alapu navigacio es giroszkop-asszisztalt fordulok segitsegevel navigal es old meg labirintusokat. A robot az ESP32 **240 MHz-es dual-core processzorat** es beepitett WiFi-jat hasznalja a kovetkezokhoz:

- **Live variable tuning** PID es mozgasi ertekekhez ESP32VariableTuner-rel
- **Kalman-szurt IR szenzorok** pontos faltavolsag-mereshez
- **RFID navigacio** iranyitott utmutatashoz labirintus tag-ek hasznalataval
- **Giroszkop-asszisztalt mozgasvezles** preciz 90 fokos fordulokhoz
- **Adaptiv falkovetes**, amely kozepre igazitja a robotot ket fal kozott, vagy egyetlen falat kovet
- **Modularis firmware**, amely audio, motion, RFID, sensor es tuner modulokra van bontva

Ez a projekt jelentosen valtozott a korabbi Arduino-alapu verziohoz kepest. A regi egyfajos, egyedi web interface helyett most ESP32VariableTuner-t hasznalunk, es a kodot kulon fajlokba rendeztuk a jobb karbantarthatosag erdekeben.

---

## Kezdo lepesek

### Elofeltetelek

- [PlatformIO IDE](https://platformio.org/) (VS Code extension ajanlott)
- ESP32 board (Wemos D1 R32 vagy kompatibilis)
- USB kabel programozashoz

### Telepites

1. **Repository klónozasa:**
   ```bash
   git clone https://github.com/szczukabendeguz/Jerry3_ESP32_MazeSolvingRobot.git
   cd Jerry3_ESP32_MazeSolvingRobot
   ```

2. **Megnyitas PlatformIO-ban:**
   - Nyisd meg a projekt mappat VS Code-ban telepitett PlatformIO extension-nel
   - A PlatformIO automatikusan telepiti a `platformio.ini`-ben felsorolt fuggosegeket

3. **Feltoltes ESP32-re:**
   ```bash
   pio run --target upload
   ```

4. **Serial output monitorozasa:**
   ```bash
   pio device monitor
   ```

### Csatlakozas a tuning interface-hez

1. Feltoltes utan az ESP32 letrehoz egy WiFi access point-ot (SoftAP mod)
2. Csatlakozz a WiFi halozathoz: az **SSID** es jelszo megjelenik a serial monitorban
3. Nyiss meg egy bongeszot es navigalj a megjelenitett IP cimre (altalaban `192.168.4.1`)
4. Hasznald az ESP32VariableTuner UI-t a kovetkezokhoz:
   - Live variable ertekek megtekintese
   - PID parameterek valos ideju hangolasa
   - Mozgasi limitek es falkovetesi kuszobok allitasa
   - Telemetria figyelese futas kozben

---

## Architektura

### Fobb komponensek

- **Szenzor feldolgozas**  
  3 Sharp IR tavolsagszenzor (elso, bal, jobb) olvasasa es Kalman-szures alkalmazasa a zaj csokkentesere es stabilitas javitasara.

- **PID szabalyzas**  
  Falkovetesi logika: kozepre igazitja a robotot, ha mindket oldalon fal van, vagy allando tavolsagot tart egy faltol.

- **RFID navigacio**  
  MFRC522 modul (SPI) olvassa a labirintusban elhelyezett iranyito tag-eket, navigacios utasitasokat adva (elore, balra, jobbra, stop, start, zsakuutca).

- **Mozgasvezles**  
  Ket DC motor L298N driver-rel tank-szeru mozgast biztosit. Az MPU-6050 giroszkop meri a forgast fordulok soran a pontos 90 fokos mozgashoz.

- **Live tuning interface**  
  Az ESP32VariableTuner adja a jelenlegi tuning dashboard-ot, es a regi custom web szervert valtja le.

### Falkovetesi algoritmus

Az adaptiv falkovetesi logika harom forgatokonyvet kezel:

- **Mindket fal eszlelve:** PID szabalyzassal kozepre igazitja a robotot a falak kozott
- **Egyetlen fal eszlelve:** Beallitott tavolsagot tart az eszlelt faltol
- **Nincs fal eszlelve:** Giroszkop adatokra tamaszkodik az irany megtartasahoz

---

## Environment variable-ok

Nincsenek szukseges environment variable-ok. A konfiguracio a kovetkezokon keresztul tortenik:

- **PID parameterek:** ESP32VariableTuner-rel vagy a `src/main.cpp`-ben allithatok
- **Motor sebessegek:** A `src/main.cpp`-ben definialva, a motion modul hasznalja oket
- **Szenzor kalibracio:** IR szenzor modellek es Kalman filter parameterek a `src/sensors.cpp`-ben
- **WiFi hitelesito adatok:** SoftAP SSID/jelszo a `src/main.cpp`-ben konfiguralt

---

## Teszteles

A projekt tartalmaz egy tuning es teszt interface-t az ESP32VariableTuner-en keresztul:

```bash
# Csatlakozz a robot WiFi halozatahoz
# Nyisd meg a tuning UI-t egy bongeszoben az ESP32 IP cimen
```

Hardware teszteleshez:

1. Helyezd a robotot egy teszt labirintusba
2. Csatlakozz a WiFi tuning interface-hez
3. Monitorozd a szenzor leolvasasokat es allitsd be a PID parametereket
4. Hasznald az live tuner-t a labirintus-megoldo rutin inditasahoz

---

## Amit megtanultunk

- **ESP32 vs Arduino:** Az ESP32-re valo atallas sokkal nagyobb teljesitmenyt adott, es lehetove tette a komplexebb algoritmusokat es a live tuningot
- **Modularis architektura:** A kod kulon fajlokra bontasa sokkal konnyebbe tette a karbantartast es a bovitest
- **Kalman-szures:** Fontos a stabil IR szenzorleolvasashoz zajos kornyezetben
- **Live tuning:** Az ESP32VariableTuner sok idot sporolt meg a teszteles es a versenyfelkeszites soran
