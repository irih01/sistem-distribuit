===========================
PROIECT – SISTEM DISTRIBUIT
===========================

IMPORTANT
---------
Acest proiect NU a fost realizat folosind Arduino IDE.
Proiectul este dezvoltat și gestionat EXCLUSIV cu PlatformIO.



DESCRIERE GENERALĂ
------------------
Proiectul implementează un sistem distribuit de tip Master–Node,
bazat pe microcontrolere compatibile Arduino/ESP, cu senzori multipli,
comunicație prin WiFi și protocol binar custom.

Structura proiectului este conform standardelor PlatformIO,
folosind separarea clară între:
 - src (cod aplicație)
 - include (headere)
 - lib (biblioteci proprii)
 - test (teste și experimente)




CERINȚE
-------
- Visual Studio Code
- PlatformIO IDE extension
- Toolchain corespunzător plăcii utilizate (ESP32 / Arduino UNO R4 WiFi etc.)
- Nu este necesar Arduino IDE




STRUCTURA PROIECTULUI
---------------------
/platformio.ini        – configurația PlatformIO
/src/
  /master/             – cod pentru dispozitivul MASTER
  /node/               – cod pentru dispozitivele NODE
/include/
  /sensors/            – headere senzori (AHT21, BMP280, GAS etc.)
/lib/
  /network/            – transport Master / Node
  /protocol/           – protocol binar custom
  /oled/               – control display OLED






SELECTAREA ROLULUI (MASTER / NODE)
---------------------------------
Rolul dispozitivului este determinat de:
 - fișierul sursă compilat (master.cpp sau node.cpp)
 - mediul PlatformIO configurat în platformio.ini

Nu se folosesc define-uri din Arduino IDE și nici sketch-uri .ino.


COMPILARE ȘI UPLOAD
-------------------
1. Deschide proiectul în Visual Studio Code
2. Asigură-te că PlatformIO este instalat
3. Selectează mediul dorit din platformio.ini 
4. Rulează:
   - Build
   - Upload
   - Monitor (opțional)




BIBLIOTECI
----------
Toate bibliotecile sunt gestionate de PlatformIO.
NU se folosesc biblioteci instalate manual din Arduino IDE.


NOTE IMPORTANTE
---------------
- Codul este C++ pur (nu .ino)
- Inițializarea și gestionarea proiectului diferă de Arduino IDE
- Structura modulară permite extindere ușoară (senzori noi, noduri noi)
- PlatformIO este OBLIGATORIU pentru compilare și upload


