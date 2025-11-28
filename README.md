# EventBoard - RTC-Driven Message Display System  

## 📌 Overview 
EcoLight Maestro is a smart street-light automation system built using the LPC2148 ARM7 microcontroller.  

It controls LED street lights based on:  
- RTC scheduled time (18:00–06:00) 
- Real-time ambient light intensity using an LDR  
 
The system improves power efficiency by enabling lights only when needed. It also features admin interrupt mode, RTC editing, LCD display, and a modular embedded-C firmware structure.  
 
---

## 🎯 Features  
- ⏰ *RTC-based Scheduling* – Controls lights automatically between 6 PM to 6 AM. 
- 🌙 *LDR-Based Intensity Control* – Turns LEDs ON only when ambient light is low.  
- 🖥 *Real-Time Display* – Date, time, and day displayed on 16x2 LCD.  
- 🔧 *Interrupt-Driven Admin Mode* – Edit RTC values using keypad.  
- 🔢 *Keypad Input with Validation* – Hours, minutes, seconds, day, date, month, year.
- 💡 *LED Output Simulation* - Represents actual street-light behavior.
- 🔔 *Optional Buzzer Alerts*   

---

## 🖥 Hardware Requirements 
Supporting passive components

- *LPC2148 ARM7 Microcontroller*  
- *16x2 LCD Display*  
- *4x4 Keypad*  
- *LDR (Light Dependent Resistor)*  
- *LEDs (Street Light Simulation)*
- *Buzzer (Optional)*  

---

## 💾 Software Requirements 
- Embedded C  
- *Keil µVision (C Compiler)*  
- *Flash Magic* (for programming LPC2148)  

---

## 🔄 System Workflow 

1. Initialize LCD, ADC, RTC, keypad, LEDs, and external interrupt.  
2. In the main loop:
   - Read RTC time, date, and day
   - Display on LCD
3. If time is between 18:00 and 06:00:
   - Read LDR using ADC
   - If light < threshold → Lights ON
   - Else → Lights OFF
4. Otherwise → Lights remain OFF
5. When interrupt button is pressed:
   - Enter Admin Mode
   - Display menu:
     1. EDIT RTC INFO
     2. EXIT
6. If the user chooses EDIT:
   - Enter fields: Hours, Minutes, Seconds, Day, Date, Month, Year
   - Validate each input
   - Update RTC registers upon valid entry
7. Return to main loop and resume automatic operation.  
  
---

## 📂 Project File Structure & Descriptions

    |-- EcoLight_Main.c              # Main loop, RTC & ADC logic
    |
    |--lcd.c / lcd.h                # LCD display driver  
    |
    |--keypad.c / keypad.h          # Keypad scanning and input handling  
    |
    |-- adc.c / adc.h                # ADC driver for LDR reading
    |
    |-- rtc.c / rtc.h                # RTC initialization, get/set time-date
    |  
    |-- settings.c / settings.h      # RTC editing menu logic and validation  
    |  
    |-- interrupt.c / interrupt.h    # External interrupt for admin mode  
    |  
    |-- delay.c / delay.h            # Delay utility functions  
    |  
    |--pin_config.c / pin_config.h  # PINSEL and GPIO configuration
    |
    |--defines.h / types.h          # Common macros and data types
    |
---
