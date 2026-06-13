# Trabajo Práctico N°5: Device drivers

### Asignatura: Sistemas de Computación

**Facultad de Ciencias Exactas, Físicas y Naturales (UNC)**

---

* **Grupo:** Sudo Make A Sandwich
* **Profesores:** Miguel Angel Solinas y Javier Alejandro Jorge

---

### Integrantes y Contacto

| Nombre y Apellido | Correo Electrónico |
| :--- | :--- |
| **Sergio Andrés Fernández Segovia** | _sergio.fernandez.segovia@mi.unc.edu.ar_ |
| **Enzo Leonel Laura Surco** | _enzo.laura.surco@mi.unc.edu.ar_ |
| **Saqib Daniel Mohammad Cabrejos** | _saqib.mohammad@mi.unc.edu.ar_ |


# Desarrollo


Previamente se preparo el hardware que alojara nuestro modulo mediante una raspberry pi zero 2w con Raspberry OS Lite de 64 bits previamente flasheada en la micro SD. en donde nos conetamos a su terminal mediante SSH desde nuestra computadora de destino.


Este trabajo se decidio dividir en varias etapas primero 
Prueba de Concepto (Fase 1): compilar el driver con señales simuladas, cargarlo en la Raspberry Pi Zero 2W y verificar que responde correctamente a los comandos desde la terminal mediante cat y echo. Esto debido a que el metodo que utilizara es uno muy usado en sistema embebidos llamado Cross Compilation, compilando nuestro programa en una computadora Host y probandolo dentro de la arquitecura del procesasor de Destino.

el archivo en cuestion se llamara sdec_prueba.c 

La arquitectura de nuestra raspberry pi es de 64 bits lo cual nos servira para poder crear el ejecutable que correra en la Raspberry Pi:

![alt text](image.png)

Este compilable es de señales simuladas lo que quiere decir que no es el modulo que se insertara de manera final sino uno prueba a fin de que funcione. Teniendo cuidado con el cross compilation

Este código evoluciona el modelo de drv4.c. En lugar de un buffer estático de un solo carácter , implementamos un estado global para seleccionar el canal (0 o 1) mediante la función write , y generamos dos señales simuladas matemáticamente dentro de read  (una senoidal y otra exponencial/cuadrada) para que el TP funcione inmediatamente, permitiendo luego acoplar los pines GPIO reales si lo deseas.



Despues tenemos el codigo de python que basicamente: 
Como el driver se encarga de exponer la interfaz limpia en /dev/SdeC_Sensor, utilizaremos un script de Python corriendo en la Raspberry Pi. Este script leerá el dispositivo cada 1 segundo y actuará como servidor web interactivo para tu PC anfitriona.

Instala Flask en la Pi corriendo: pip install flask o sudo apt install python3-flask.


simulación matemática como un paso previo ("mockup") para asegurar que toda la estructura del driver (el registro en el VFS, la creación de la clase y el nodo en /dev) funcionara correctamente en tu Raspberry Pi sin depender todavía de señales externas físicas ni arriesgar bloqueos de hardware.Sin embargo, tal como se menciona en las conclusiones de la presentación de tu cátedra, el objetivo final es unirlo a un dispositivo de hardware que interactúe con los puertos GPIO.  


EL codigo sdec_prueba:
Este código utiliza la estructura de inicialización automática de tus archivos de referencia (drv3.c y drv4.c). Implementa las macros module_init() y module_exit() , reserva dinámicamente los números mayor y menor con alloc_chrdev_region() y genera el archivo en /dev usando class_create() y device_create().  



Ahora debido a la compilacion cruzada debemos configurar un Makefile especifico con los headers que usa el Kernel de Raspberry PI OS

Una forma limpia y rapida es descargarlos en la Raspberry Pi y traerlos a nuestra computadora local

Nos conectamos por SSH y ejecutamos lo siguiente (este paso se hace por las dudas ya que muy posiblemente ya tengo esos headers):

```
sudo apt update
sudo apt install linux-headers-$(uname -r)
```

Y verificamos donde se enceuntran esos archivos
```
ls -d /usr/src/linux-headers-*
```

y en nuestra pc traemos esos headers mediante la terminal y de manera local ejecutando:

```
mkdir -p ~/rpi_kernel


scp -F /dev/null -r saqibpi@192.168.43.252:/usr/src/linux-headers-6.12.75+rpt-* ~/rpi_kernel/
```
![alt text](image-1.png)



Ahora con los archivos ya de manera local configuraremos nuestro Makefile que servira para compilar los codigos fuente que insertaremos en como modulo en el kernel de la raspberry pi

```
obj-m += sdec_prueba.o

# Apunta directo al directorio v8 de home local, donde se encuentran los headers del kernel para Raspberry Pi
KDIR := $(HOME)/rpi_kernel/linux-headers-6.12.75+rpt-rpi-v8

ARCH := arm64
CROSS_COMPILE := aarch64-linux-gnu-

all:
	make -C $(KDIR) M=$(shell pwd) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) modules

clean:
	make -C $(KDIR) M=$(shell pwd) clean
```

Este Makefile tomara el codigo fuente que inicialmente usaremos de prueba lo compilara en un .o y luego generara un modulo final .ko. Ademas definimos la ruta donde se encuentran los headers del kernel de la Raspberry Pi. Le indicamos que la arquitectiura de destino es una ARM de 64 bits. Y definimos el prefijo de las herramientas de compilacion cruzada. El Makefile y buscara otros comandos en lugar de usar el compilador nativo de mi computadora ```gcc```.

Tambien tenemos una regla ```make``` y una ```clean```, la make cambiara el directorio que se definio para compilar con headers correctos. Los codigos fuente estan en el directorio actual
el clean sirve para limpiar el espacio de trabajo, eliminando todos lo archivos generados durante la compilacion dejando solo el codigo fuente


Entonces ejecutamos el comando ```make``` lo cual nos generara el archivo .ko

esto lo enviamos mediante scp al home de la raspberry pi

```
scp -F /dev/null sdec_prueba.ko saqibpi@192.168.43.252:/home/saqibpi/
```

![](image-2.png)

Cargamos el modulo con:

```
sudo insmod sdec_prueba.ko
```

y comprobamos que el modulo fue cargado correctamente:

![alt text](image-3.png)

notamos que en los permisos empieza con C, lo que confirma que es un character device. Ademas notamos como el sistema le agrego el numero major 236 y el minor 0.

Ademas para permitir la lectura de ese driver tenemos que modificar los permisos mediante:

```
sudo chmod 666 /dev/SdeC_Prueba
```

Entonces haremos unas ruebas sobre el modulo para garantizar su funcionamiento; al ejecutar el comando cat el kernel traduce la ptecicion del usuaio e invoca a la funcion de abstraccion llamada my_read().

Como el driver de prueba inicia por defecto en el Canal 0 el codigo devuelve una cadena de texto basada en la constante 100 sumada a una variable global llamada contador_clicks. cada llamada a cat incrementa dicho contador y el copy_to_user() lo envia correctamente al espacio de usuario.

El comando echo -n"1" envia un byte de informacion hacia el dispositiva, el VFS entonces invocara a la funcion my_write() del driver, quien envia al espacio de usuario con copy_from_user(), al detectar el caracter '1' conmuta inmediatamente debido a la variable de estado llamada canal_seleccionado = 1; y salta  a una base para el canal 1 (constante 500) entregando el valor 502. 

Osea le indicamos al CDD que señal leer.

![alt text](image-4.png)








# -------#
Se creo tambien un codigo en python llamado en ```app.py```

Este codigo actua como un punete entre el CDDy el usuario, crea y aloja una interfaz web interactiva con un grafico en tiempo real.
Cada 1 segunod, lee los datos generados por el controlador del kernel desde el archivo del dispositvo
en /dev/ y se los envia al grafico web. Ademas recibe las ordenes del usuario cuando se clickea en los botones de la web escribiendo un '0' o un '1' para indicarle al controlador del kernel que datos queremos.
Finalmente levantamos el servidor en el puerto 5000 permitiendo que calquier dispositvo en la misma red Wifi pueda ver el panel ingresando en la IP de Raspberry Pi


Una vez hecho esto lo enviamos a la raspberry pi mediante el comando: 

```
scp -F /dev/null app.py saqibpi@192.168.43.252:/home/saqibpi/
```

Y desplegamos la app mediante el comando ```python3```

![alt text](image-5.png)


En nuestra computadora podemos visualizar la UI poniendo en el buscador la IP:PUERTO de la Raspberry PI en la red.

![alt text](image-8.png)

**Nota:** La app.py esta sujeta a modificacion las señales pueden ser otras aparte de temperatura y presion ademas son señales simuladas.



### Implementacion de Señales mediante ESP32 -> Raspberry PI

La ESP32 genera dos señales digitales distintas (frecuencias o estados) en dos de sus pines.

Esos pines se conectan directo a dos pines GPIO de la Raspberry Pi.

Modificamos tu driver en C (sdec_prueba.c) para que use las funciones del kernel <linux/gpio.h> y lea el estado físico de esos pines según el canal que le pida Flask.

La esp32 generara dos señales digitales distintas (frecuencias o estados ) en sus dos pines, esos pines se conectan directo a dos pines GPIO de la Raspberry Pi 
MOdificaremos nuestro dirver en C, para que usea funciones del kernel ```<linux/gpio.h>``` y lea el estado fisico de esos pines segun el canal que le pida nuestra app en Flask.



### Conexion fisica que se realizara:

1. El conexionado físico (Hardware)Como ambas placas manejan 3.3V, la conexión es segura y directa. Vamos a elegir dos pines GPIO estándar de la Raspberry Pi (por ejemplo, el 23 y el 24).ESP32 (Salida)DirecciónRaspberry Pi (Entrada)Rol en el TPGND<--->GND (Pin 6)Masa común obligatoriaGPIO 18---->GPIO 23 (Pin 16)Señal Externa 0 (Temperatura)GPIO 19---->GPIO 24 (Pin 18)Señal Externa 1 (Presión)

# IMAGEN


2. Crearemos un nuevo archivo en nuestro directorio /driver_gpio

Nuestro nuevo archivo ```sdec_driver_gpio.c``` el cual sera nuestro nuevo CDD que incluira la API de GPIO del kernel de Linux

```
#include <linux/gpio.h> // API de GPIO del Kernel
```

Incluimos los pines de la Raspberry PI

```
#define GPIO_SEÑAL_0 535  // (512 de base + 23 del GPIO)
#define GPIO_SEÑAL_1 536  // (512 de base + 24 del GPIO)
```

Solicitaremos al kernel el control de los pines fisicos en la funcion prueba_init():

```
gpio_request(GPIO_SEÑAL_0, "SdeC_Sig0");
gpio_direction_input(GPIO_SEÑAL_0);

gpio_request(GPIO_SEÑAL_1, "SdeC_Sig1");
gpio_direction_input(GPIO_SEÑAL_1);
```

Hacemos una modificacion parecido en prueba_exit() liberando los pines con gpio_free()

Finalmente tambien modificaremos la funcion my_read()

```
int valor_pin = 0;

if (canal_seleccionado == 0) {
    valor_pin = gpio_get_value(GPIO_SEÑAL_0);
    // Convertís el estado físico (0 o 1) a una escala de temperatura visible
    sprintf(buffer_texto, "%d\n", valor_pin ? 25 : 23); 
} else {
    valor_pin = gpio_get_value(GPIO_SEÑAL_1);
    // Convertís el estado físico a una escala de presión visible
    sprintf(buffer_texto, "%d\n", valor_pin ? 1015 : 1012); 
}
```


Con esta arquitectura, cuando se cambie el canal en la pagina web Flask escribira un '0' o un '1' al driver, el driver conmutara su lectura hacia el pin GPIO correspondiente, mediendo el voltaje real que le manda la ESP32 y se lo enviara de vuelta de manera procesada a la interfaz de usuario en un segundo exacto.


***Makefile***: Modificaremos nuestro archivo coincidiendo con el nombre del nuevo codigo fuente: ```obj-m += sdec_driver_gpio.o``` y compilamos.

![alt text](image-6.png)

Enviamos el archivo compilado: ```sdec_driver_gpio``` y la aplicacion ```app.py``` actualizada

```
scp -F /dev/null sdec_driver_gpio.ko saqibpi@192.168.43.252:/home/saqibpi/

scp -F /dev/null app.py saqibpi@192.168.43.252:/home/saqibpi/

```
Repetimos el mismo procedimiento removeremos el antiguo driver y cargaremos el nuevo y modificamos los permisos.

Dentro de la Raspberry Pi
```

sudo rmmod sdec_prueba

# Insertamos el nuevo driver que lee los GPIOs
sudo insmod sdec_driver_gpio.ko

# Le damos permisos globales al nuevo nodo automático
sudo chmod 666 /dev/SdeC_SudoMakeMe
```
![alt text](image-7.png)

Podemos observar como la configuracion nueva funciona. Conectaremos y cargaremos nuestro codigo en ESP32.

El codigo es el siguiente:

```
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
```































PARA EL CODIGO QUE USA LOS PINES 17 Y 27:

gpio_request y gpio_direction_input: En la función constructora (sensor_init), el módulo le pide permiso de control al sistema operativo para usar los pines 17 y 27 de la Pi Zero y explícitamente los configura como entradas de datos.

gpio_get_value: Reemplaza por completo el código de simulación matemática. Ahora, cada vez que la aplicación de usuario (app.py) ejecuta un .read() cada segundo, el driver consulta el estado de voltaje instantáneo en el pin físico correspondiente. Si hay tensión (3.3V) devolverá un 1, y si está a masa (GND) devolverá un 0.

gpio_free: En la función destructora (sensor_exit), cuando descargas el módulo con sudo rmmod, se liberan los pines para que el hardware de la Pi no quede retenido o bloqueado.