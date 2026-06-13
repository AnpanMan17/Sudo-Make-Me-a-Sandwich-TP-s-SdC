// Código de simulación digital en la ESP32 para generar señales físicas en los GPIOs 23 y 24 de la Raspberry Pi

const int pinSignal0 = 18; // Conecta al GPIO 23 de la Pi
const int pinSignal1 = 19; // Conecta al GPIO 24 de la Pi

void setup() {
  // Configura los pines GPIO como salidas digitales
  pinMode(pinSignal0, OUTPUT);
  pinMode(pinSignal1, OUTPUT);
}

void loop() {
  // Señal 0 (Temperatura): Cambia de estado cada 2 segundos.
  digitalWrite(pinSignal0, (millis() / 2000) % 2);
  
  // Señal 1 (Presión): Cambia de estado cada 5 segundos.
  digitalWrite(pinSignal1, (millis() / 5000) % 2);
}