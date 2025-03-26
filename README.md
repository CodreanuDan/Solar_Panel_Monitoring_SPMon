# 🌞 Solar Panel Monitoring  

## 📌 Project Overview  
This project aims to **monitor the performance of a solar panel** based on various **environmental factors** over a long period. The primary focus is on the **software implementation**, ensuring reliable data acquisition, storage, and analysis.  

## 📁 Project Structure  
- **Source Code:** Located in the `src/` subfolder  
- **Platform:** Developed using **PlatformIO**  
- **Microcontroller:** **ESP32**  
- **Cloud Storage:** Data is stored using **Google Spreadsheets**  

## 🛠️ Hardware Components  
The project utilizes the following **sensors** to collect environmental and electrical data:  
- **DHT11** – Temperature & Humidity (1-Wire)  
- **MAX Thermocouple** – Temperature (SPI)  
- **BH1750** – Light Intensity (I2C)  
- **LM35** – Temperature (Analog)  
- **DSM501A** – Dust Concentration (Analog)  
- **Voltage & Current Sensors** – To monitor electrical performance  
- **Spectral Sensor** *(Planned addition)* – To analyze solar spectrum influence  

## 📸 Project Layout  
<p align="center">
    <img src="https://github.com/user-attachments/assets/b83ce9f5-683d-408d-842b-d5cf3262d724" 
         alt="Project Layout" height="400"/>
</p>
<p align="center"><em>Figure 1: Overview of the hardware setup</em></p>

## 🖥️ Software Architecture  
The software is **modular** and structured using **FreeRTOS tasks**:  
1. **Initialization Task** – Sets up hardware and configurations  
2. **Sensor Calibration Task** – Ensures accurate sensor readings  
3. **Main Task (State Machine)** – Manages system operation  
4. **Sensor Measurement Task** – Collects and processes data  
5. **Communication Task** – Sends data to **Google Spreadsheets**  

### 📸 Software Architecture Diagram  
<p align="center">
    <img src="https://github.com/user-attachments/assets/403479f1-2aec-4acd-9114-b51b62b73a25" 
         alt="Software Architecture" height="400"/>
</p>
<p align="center"><em>Figure 2: Software architecture and data flow</em></p>

## 📊 Data Analysis & AI Integration  
Once data acquisition is complete, the project will leverage **AI/ML techniques** to analyze the collected data. The goal is to identify **key environmental factors** affecting the solar panel’s performance and optimize efficiency.  

## 🚀 Future Enhancements  
- Implement **predictive analytics** for performance estimation  
- Add more **environmental sensors** for better accuracy  
- Explore **alternative cloud storage solutions**  

---

## 🔧 Setup & Installation  
1. **Clone the repository:**  
   ```sh
   git clone https://github.com/your-username/Solar_Panel_Monitoring.git
