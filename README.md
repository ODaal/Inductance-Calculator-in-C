# 📈 Inductance vs Charge Percentage (C Project)

This project implements a **numerical solver and plotter** in **C** that calculates and visualizes the inductance **L** as a function of the charge percentage **pc**.  
It uses **PLplot** for graphing and supports saving/loading user data through a binary file.

---

## 🚀 Features
- Interactive data entry with validation.  
- Binary file storage (`donnee.bin`) for reusing saved values.  
- Root-finding with the **bisection method**.  
- Generates plots of **L vs pc** using the PLplot library.  
- Supports French character encoding on Windows.  

---

## 📊 Example Workflow
- User inputs values for **R, C, pcmin, pcmax, td**.  
- Program computes valid range and roots with **bisection**.  
- Results are plotted automatically.  
- User can choose to **save data** to `donnee.bin` for later reuse.  

---

## 🔮 Future Improvements
- Add support for multiple plotting modes (overlay comparisons).  
- Improve error handling for invalid inputs.  
- Cross-platform support (Linux, macOS).  

---

## 👤 Author
Othmane Daali
