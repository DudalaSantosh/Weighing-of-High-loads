# Weighing-of-High-loads
It weighs the weight at the port areas and also at the Construction areas
**Overview of the Project:**
This project collects the weight from the sensor and then sends it to the transmitter to combine the weights of all four sensors, which are assigned addresses from 1 to 4. It then sends it through the transceiver to the receiver, where it displays in the seven-segment display.
**Features:**
i) Measures heavy loads using four load sensors with ADCs for high accuracy.
ii) Data is preprocessed at the transmitter to combine all four weighing sensor readings.
iii) Wireless transmission between the RF transceiver module NRF24L01.
iv) Real-time weight will be displayed on the seven-segment display.
**Hardware Used in the Project:**
1) 4 × Weight Sensors
2) 4 × ADC modules
3) NRF24L01 transceiver modules-2 
4) Microcontrollers-STM32F401CCU6
5) Seven-segment display module
6) Power supply is usually 3.3V for the STM32.
**Software Used for the Coding to the Microcontroller:**
1)STM32 Cube IDE for the coding of the ADC, Transmitter and the REceiver boards.
**Block Diagram:**
<img width="628" height="208" alt="image" src="https://github.com/user-attachments/assets/63569ead-851f-4267-8197-11773f70a00d" />
