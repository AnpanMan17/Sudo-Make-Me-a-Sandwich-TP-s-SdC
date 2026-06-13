// Código de simulación digital en la ESP32 para generar señales físicas en los GPIOs 23 y 24 de la Raspberry Pi

const int pinSignal0 = 18; // Conecta al GPIO 23 de la Pi
const int pinSignal1 = 19; // Conecta al GPIO 24 de la Pi

void setup() {
  // Configura los pines GPIO como salidas digitales
  pinMode(pinSignal0, OUTPUT);
  pinMode(pinSignal1, OUTPUT);
}

unsigned long ultimoCambio0 = 0;
unsigned long ultimoCambio1 = 0;
int estado0 = 0;
int estado1 = 0;
int intervalo0 = 1000;
int intervalo1 = 1000;

void loop() {
  unsigned long tiempoActual = millis();

  // Señal 0 (Temperatura): Cambia de estado en tiempos aleatorios (ej. 0.5s a 3s)
  if (tiempoActual - ultimoCambio0 >= intervalo0) {
    estado0 = !estado0;
    digitalWrite(pinSignal0, estado0);
    ultimoCambio0 = tiempoActual;
    intervalo0 = random(500, 3000); // Genera el próximo intervalo aleatorio
  }
  
  // Señal 1 (Presión): Cambia de estado en tiempos aleatorios (ej. 1s a 6s)
  if (tiempoActual - ultimoCambio1 >= intervalo1) {
    estado1 = !estado1;
    digitalWrite(pinSignal1, estado1);
    ultimoCambio1 = tiempoActual;
    intervalo1 = random(1000, 6000); // Genera el próximo intervalo aleatorio
  }
}