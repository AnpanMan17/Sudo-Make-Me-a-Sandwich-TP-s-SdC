# Trabajo Práctico N°3.a: Entorno UEFI, Desarrollo y Análisis de Seguridad

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

---

*Objetivo General: Comprender la arquitectura de la Interfaz de Firmware Extensible Unificada (UEFI) como un entorno pre-sistema operativo, desarrollar binarios nativos, entender su formato y ejecutar rutinas tanto en entornos emulados como en hardware físico (bare metal).*

## Introducción

En este trabajo práctico, para lograr el objetivo planteado, se exploran los mecanismos básicos de la UEFI Shell, la gestión de dispositivos mediante handles y protocolos, el uso de variables no volátiles y, también, la información de memoria y hardware que está disponible previo a entrar al sistema operativo.

Partiendo de esta base, se desarrolla una aplicación UEFI en lenguaje C, siguiendo el procedimiento necesario para que el formato del mismo sea el adecuado para el firmware. Además, se realiza el análisis de la representación interna del ejecutable obtenido mediante herramientas de ingeniería inversa (Ghidra), permitiendo establecer una relación entre el código fuente y el binario generado.

Finalmente, se revisa la ejecución en hardware físico, con la finalidad de verificar el comportamiento del binario generado fuera del entorno emulado, reforzando, de esta manera, la comprensión del proceso completo abarca desde el desarrolo hasta la ejecución en bare metal.

## Preparación del Entorno (Linux)

Para comenzar con el laboratorio, se instalaron las dependencias necesarias incluyendo el emulador QEMU, el firmware OVMF y las herramientas de compilación cruzada. Posteriormente, se procedió a la instalación manual/configuración de Ghidra para el análisis inverso.

![image](https://hackmd.io/_uploads/S1ZTzzXR-g.png)

![image](https://hackmd.io/_uploads/BJqsNfX0bg.png)


## Trabajo Práctico 1: Exploración del entorno UEFI y la Shell



### 1.1 Arranque en el entorno virtual

Iniciamos el entorno virtual utilizando QEMU, pasándole como parámetro el binario del firmware OVMF. Al no especificar un disco de arranque, el sistema nos deja directamente en la UEFI Interactive Shell.

![image](https://hackmd.io/_uploads/Hk93rfQAWe.png)




### 1.2 Exploración de Dispositivos (Handles y Protocolos)

A continuación, utilizamos comandos propios de la Shell de UEFI para listar los dispositivos mapeados y la base de datos de Handles. 
![image](https://hackmd.io/_uploads/rk5QyN7AZe.png)

Nota: Al estar emulando sin almacenamiento adjunto, el comando map expone únicamente el dispositivo de bloques BLK0 sin sistema de archivos FS0.

![image](https://hackmd.io/_uploads/HyZIy47Rbl.png)


![image](https://hackmd.io/_uploads/B1W_k4mRbl.png)


![image](https://hackmd.io/_uploads/ByPqJV7CWg.png)

**Pregunta de Razonamiento 1: Al ejecutar el comando map y dh, vemos protocolos e identificadores en lugar de puertos de hardware fijos. ¿Cuál es la ventaja de seguridad y compatibilidad de este modelo frente al antiguo BIOS?**

**Respuesta:** Este modelo implementa una abstracción total del hardware. En lugar de comunicarse directamente con puertos físicos específicos, el firmware interactúa mediante interfaces estandarizadas (Protocolos). Esto aporta una gran ventaja de compatibilidad, ya que el mismo binario UEFI puede interactuar con un dispositivo sin importarle si está conectado por SATA, USB o PCIe. A nivel de seguridad, al evitar el acceso directo a los componentes físicos, se previenen conflictos de hardware y se facilita el control sobre qué ejecutable tiene permisos para interactuar con qué dispositivo.

## 1.3  Análisis de Variables Globales (NVRAM)

En esta fase interactuamos con la NVRAM, volcando la configuración actual del sistema e inyectando una variable personalizada para comprobar la persistencia de la configuración en la etapa BDS.

![image](https://hackmd.io/_uploads/ByxOxN7Abl.png)
![image](https://hackmd.io/_uploads/ryo9lEQC-e.png)


![image](https://hackmd.io/_uploads/HJjzMVX0Wx.png)


**Pregunta de Razonamiento 2: Observando las variables Boot#### y BootOrder, ¿cómo determina el Boot Manager la secuencia de arranque?**

**Respuesta:** El Boot Manager utiliza un sistema de punteros secuenciales. Primero, lee el contenido de la variable BootOrder, la cual contiene un arreglo numérico con la prioridad de arranque (por ejemplo, 0000, 0002). Luego, toma el primer valor de esa lista y busca su variable correspondiente (ej. Boot0000). Dentro de esta última, lee la ruta específica (Device Path) que apunta al ejecutable .efi a iniciar. Si ese dispositivo no está disponible, pasa al siguiente número en la lista de BootOrder.

## 1.4  Footprinting de Memoria y Hardware

Finalmente, extrajimos el mapa de memoria actual y listamos los dispositivos PCI y drivers cargados en el entorno pre-SO.

![image](https://hackmd.io/_uploads/r1THvEQC-x.png)
![image](https://hackmd.io/_uploads/rJBuDV7CZe.png)
![image](https://hackmd.io/_uploads/rkVcPVQAbl.png)


![image](https://hackmd.io/_uploads/r1ugyI7C-x.png)


![image](https://hackmd.io/_uploads/By4PJUm0bx.png)

![image](https://hackmd.io/_uploads/SJlF1U7Cbg.png)

**Pregunta de Razonamiento 3: En el mapa de memoria (memmap), existen regiones marcadas como RuntimeServicesCode. ¿Por qué estas áreas son un objetivo principal para los desarrolladores de malware (Bootkits)?**

**Respuesta:** La criticidad de las regiones RuntimeServices radica en que no se borran de la memoria una vez que el Sistema Operativo (Windows o Linux) toma el control mediante la función ExitBootServices(). Si un desarrollador de malware (Bootkit) logra inyectar código en estas áreas, se garantiza una persistencia absoluta. El código malicioso continuará ejecutándose por debajo del SO con los máximos privilegios de hardware (como el modo SMM o Ring -2), volviéndose completamente indetectable para los motores de antivirus convencionales.


## Trabajo Práctico 2: Desarrollo, compilación y análisis de seguridad

### 2.1. Objetivo
"Crear una aplicación nativa UEFI en C, entender el formato PE/COFF y analizar cómo un descompilador interpreta opcodes a nivel de firmware."

### 2.2. Desarrollo de la Aplicación
En primera instancia, se crea el archivo `aplicacion.c` y el código contenido en dicho archivo es el que se muestra a continuación.

```c
#include <efi.h>

EFI_STATUS efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable) {
    uefi_call_wrapper(SystemTable->ConOut->OutputString, 2,
                      SystemTable->ConOut,
                      L"Iniciando analisis de seguridad...\r\n");

    // Inyección de un software breakpoint (INT3)
    unsigned char code[] = { 0xCC };

    if (code[0] == 0xCC) {
        uefi_call_wrapper(SystemTable->ConOut->OutputString, 2,
                          SystemTable->ConOut,
                          L"Breakpoint estatico alcanzado.\r\n");
    }

    uefi_call_wrapper(SystemTable->BootServices->Stall, 1, 3000000);
    return EFI_SUCCESS;
}
```

**Nota Técnica importante: Incompatibilidad de ABI (System V vs MS ABI)**
Al desarrollar aplicaciones UEFI puro en un entorno Linux utilizando GCC, surge un problema crítico de "Convención de Llamadas" (Calling Convention).
- **UEFI** utiliza nativamente la convención de llamadas de Microsoft de 64 bits (MS ABI), esperando recibir los argumentos de las funciones en los registros `RCX, RDX, R8, R9`.
- **GCC en Linux** compila las llamadas utilizando la convención estándar System V AMD64, pasando los argumentos en los registros `RDI, RSI, RDX, RCX`.

Si llamaramos de forma directa a las funciones de UEFI (por ejemplo, omitiendo el wrapper y usando `SystemTable->ConOut->OutputString(...)`), se produce una lectura de datos incorrecta en los registros del CPU. Esto termina ocasionando un fallo de memoria (Page Fault o "Pantalla negra") al momento de correr en el Hardware real (Bare Metal).

Para solucionar esto de raíz y poder imprimir mensajes de manera segura, el código hace uso de la macro `uefi_call_wrapper(...)`. Esta actúa como un "puente traductor" en tiempo de ejecución, reordenando los parámetros desde la convención de Linux hacia la convención de Windows/UEFI. Así mismo, aplicamos la misma técnica para llamar al servicio de retardo (`BootServices->Stall`), dándonos tiempo suficiente (3 segundos) para leer la pantalla antes de que el firmware se limpie y el sistema regrese al menú original.

### 2.3. Compilación a Formato PE/COFF

#### Compilación
Como primer paso se debe compilar el código anterior, obteniendo el archivo objeto `aplicacion.o`. Para ello se emplea la siguiente línea de comando:

```bash
gcc \
-I/usr/include/efi \
-I/usr/include/efi/x86_64 \
-I/usr/include/efi/protocol \
-fpic \
-ffreestanding \
-fno-stack-protector \
-fno-strict-aliasing \
-fshort-wchar \
-mno-red-zone \
-maccumulate-outgoing-args \
-Wall \
-c -o aplicacion.o aplicacion.c
```

Del bloque anterior, se observa:

- `-ffreestanding`: Indica que el código no depende de un SO convencional.
- `-fno-stack-protector`: Desactiva protecciones pensadas para programas de usuario normales, las cuales, en este caso no son apropiadas.
- `-mno-red-zone`: Deshabilita la red zone.
- `-c`: Compila sin enlazar, se genera el archivo objeto.


![Compilacion](https://hackmd.io/_uploads/rJwlulE0-l.png)

#### Enlazado
Lo siguiente es enlazar el archivo objeto `aplicacion.o` con las bibliotecas de GNU-EFI, a través de este procedimiento se obtiene el ejecutable `aplicacion.so`.
 
Se trabajó con la siguiente línea:

```bash
ld -shared \
-Bsymbolic \
-L/usr/lib \
-L/usr/lib/efi \
-T /usr/lib/elf_x86_64_efi.lds \
/usr/lib/crt0-efi-x86_64.o \
aplicacion.o -o aplicacion.so \
-lefi -lgnuefi
```

- `crt0-efi-x86_64.o`: Archivo de arranque que prepara la ejecución en UEFI.
- `-lefi`y `-lgnuefi`: Enlazan las bibliotecas necesarias para utilizar servicios del firmware.
- `-T,/usr/lib/elf_x86_64_efi.lds`: Usa el script de enlace específico para aplicaciones EFI.
- `-o aplicacion.so`: Crea el binario enlazado.

![Enlazado](https://hackmd.io/_uploads/BJwMdeNA-e.png)

#### Conversión al formato EFI
Como último paso se debe convertir el formato del binario `aplicacion.so` al esperado por UEFI (PE/COFF). Para ello se utiliza el comando `objcopy` para su conversión, el resultado es `aplicacion.efi`.

```bash
objcopy \
-j .text \
-j .sdata \
-j .data \
-j .dynamic \
-j .dynsym \
-j .rel \
-j .rela \
-j .rel.* \
-j .rela.* \
-j .reloc \
--target=efi-app-x86_64 aplicacion.so aplicacion.efi
```

- `--target efi-app-x86_64`: Se ocupa de convertir el binario al formato ejecutable EFI para arquitectura x86_64.

![Formato](https://hackmd.io/_uploads/Hyx7ulNCWl.png)

Posteriormente, para verificar si el formato es el correcto, se escribe `file aplicacion.efi` en la terminal, se espera que se indique `PE32+ executable (EFI application)`.

![Ejecutable](https://hackmd.io/_uploads/r1h7Ox4Rbl.png)

### 2.4. Análisis de Metadatos y Decompilación
En la consigna de este trabajo práctico se propone escribir `readelf -h aplicacion.efi` en la terminal con la finalidad de detallar su información. Sin embargo, se puede anticipar que, en este caso, no es una operación posible. Ésto es debido a que `aplicacion.efi` está en formato `PE/COFF` y, `readelf` es una herramienta que se aplica sobre archivos con formato `ELF`, el cual es utilizado para sistemas operativos como Unix y Linux.

Por lo tanto, si se deseara obtener información, una opción sería aplicar el comando `readelf` sobre `aplicacion.so`, es decir, escribir `readelf -h aplicacion.so`.

![Readelf](https://hackmd.io/_uploads/ry50zBr0Wl.png)

Para complementar el desarrollo de la aplicación, se utiliza `Ghidra` como herramienta de análisis. Este entorno permite examinar el ejecutable `aplicacion.efi`, mostrando las instrucciones en lenguaje ASM y también reconstruyendo de manera aproximada en pseudocódigo C.

Entonces, partiendo de que `Ghidra` ya está instalado, se debe importar el archivo `aplicacion.efi` siguiendo los pasos que se muestran a continuación.

![Importar](https://hackmd.io/_uploads/Sy0FWUBC-g.gif)

Una vez importado el archivo `aplicacion.efi`, el paso que sigue es analizarlo. Para ello, se hace doble click sobre el archivo y, al instante, aparece una ventana solicitando proceder con dicho análisis.

![Analisis](https://hackmd.io/_uploads/rypSSUr0be.png)

Al finalizar, se muestran varias ventanas, de las cuales las de interés son las siguientes.

- `Symbol Tree`: Organiza los símbolos detectados por `Ghidra`, por ejemplo, funciones y datos globales.
- `Listing`: Muestra el contenido del binario en forma de direcciones, bytes e instrucciones ensambladas.
- `Decompile`: Presenta una reconstrucción aproximada del código en pseudocódigo C.

Entonces, para obtener el análisis sobre `efi_main()`, se debe buscar ésta función en la carpeta `Functions`, ubicada en la ventana `Symbol Tree`. Posteriormente, se hace doble click sobre dicha función y automáticamente las ventanas `Listing` y `Decompile` mostrarán, respectivamente, el código en lenguaje ASM y la reconstrucción en pseudocódigo.

![Ventanas](https://hackmd.io/_uploads/H1vcBISA-x.png)

De acuerdo con la imagen anterior, se puede observar que en la ventana del pseudocódigo en C no se muestra la condición `if`. Una posible explicación es que 
el compilador haya optimizado esa instrucción al detectar que la comparación entre `code[0]` y `0xCC` es siempre verdadera. En ese sentido, `Ghidra` salta directamente a imprimir el segundo mensaje.

### 2.5. Preguntas de Razonamiento

### ¿Por qué utilizamos SystemTable->ConOut->OutputString en lugar de la función printf de C?

Porque en una aplicación UEFI aún no estamos trabajando dentro de un sistema operativo en el cual ya existen bibliotecas estándar de C y funciones como `printf`. En ese sentido, en este entorno, la forma adecuada de mostrar textos es a través de los servicios que tiene para ofrecer el mismo firmware; en este caso particular, si se desea imprimir por consola se debe utilizar `SystemTable->ConOut->OutputString`.

### En el pseudocódigo de Ghidra, la condición 0xCC suele aparecer como -52. ¿A qué se debe este fenómeno y por qué importa en ciberseguridad?

Esto pasa debido a que el mismo byte puede interpretarse de diferentes formas, por ejemplo, el valor `0xCC` en hexadecimal es `204` si se lo trata como un número sin signo, pero si se lo interpreta como un entero con signo de 8 bits, siguiendo el `Complemento a 2`, se representa como `-52`.

Es por lo anterior que resulta bastante importante revisar este tipo de situaciones en ciberseguridad, ya que al analizar los distintos archivos binarios posibles, no es suficiente con visualizar los bytes; sino que, también es necesario entender cómo éstos son interpretados por la herramienta que se utiliza. Por ejemplo, un mismo valor podría verse como un dato, como una instrucción o, incluso, como un número negativo. Ésto produce grandes cambios al momento de hacer lectura de un programa determinado.

## Trabajo Práctico 3: Ejecución en Hardware Físico (Bare Metal)

### 3.1. Disco desmontado y formateado en FAT32

**Importancia del formato FAT32:** La especificación UEFI establece como requisito obligatorio que los medios de arranque externos (como un pendrive USB) o las particiones EFI (ESP) utilicen el sistema de archivos de la familia FAT, siendo **FAT32** el estándar universal y nativamente soportado por todas las placas base. Si el disco estuviera formateado en NTFS, exFAT o ext4, el firmware simplemente no podría reconocer el volumen ni montar el sistema de archivos durante la fase de inicialización. Esto haría totalmente imposible la exploración de directorios y la carga tanto de la UEFI Shell como de nuestro archivo ejecutable `.efi`.

En las siguientes capturas se puede observar la correcta preparación del dispositivo (`sdb`), verificando que cuente con el nivel de formato adecuado (FAT32) para garantizar la compatibilidad total en el entorno *Bare Metal*:

![image](https://hackmd.io/_uploads/rJzc-qI0-e.png)
![image](https://hackmd.io/_uploads/SkWlZ9URbg.png)

### 3.2. Disco montado y creacion de la estructura estandarizada de directorios

**Procedimiento de montaje y convenciones del estándar UEFI:**
Para poder leer o escribir en el pendrive, en sistemas Linux es necesario "montar" el dispositivo de bloques crudo (`/dev/sdb1`) en un directorio del sistema de archivos local. Se utiliza `/mnt` (mount) debido a que es el directorio estándar en sistemas Unix diseñado específicamente para el montaje temporal de sistemas de archivos.

Una vez montado, se procede a crear la jerarquía interna `EFI/BOOT`. ¿Por qué esta estructura específica? La especificación UEFI estandariza un *Default Boot Behavior* (Comportamiento de Arranque Predeterminado) para medios extraíbles. Cuando el firmware UEFI inspecciona un pendrive USB, y si no existen variables de arranque previas en la NVRAM para este disco, el *Boot Manager* buscará automáticamente (y como plan de contingencia) la ruta exacta `\EFI\BOOT\` intentando ejecutar el binario allí alojado (usualmente nombrado `BOOTX64.EFI` para arquitecturas x86_64). Generar esta estructura asegura que el dispositivo cumpla rigurosamente con los requisitos del estándar.

![image](https://hackmd.io/_uploads/r1PtX5I0bg.png)

### 3.3. Se descarga la UEFI Shell oficial de TianoCore y se copia nuestra aplicacion.efi del TP2 en la raiz del USB

**Obtención de la Shell interactiva:**
Para poder investigar el entorno e invocar nuestro programa manualmente, descargamos la UEFI Shell precompilada (oficial del proyecto TianoCore). Este archivo lo descargamos y lo renombramos directamente como `EFI/BOOT/BOOTX64.EFI`. Al delegar este archivo clave en dicha ruta, y siguiendo el *Default Boot Behavior* comentado anteriormente, la placa base ejecutará automáticamente esta terminal al encender la máquina.

**La importancia de utilizar exclusivamente el formato autocontenido .efi:**
A continuación, alojamos nuestra `aplicacion.efi` en la raíz del USB (montada temporalmente en `/mnt/`). Es crucial recalcar que **no** copiamos el archivo fuente (`.c`), el código objeto (`.o`) ni la librería enlazada dinámica (`.so`). A nivel de Hardware (Bare Metal), el entorno pre-sistema operativo de la UEFI es muy estricto y carece de herramientas de alto nivel: no posee compiladores, enlazadores ni un cargador que soporte el formato ELF de Linux (`.so`).

El único formato nativo que el firmware UEFI es capaz de interpretar, cargar en memoria y pasarle el hilo de control del microprocesador es el estándar **PE/COFF** empaquetado bajo la extensión `.efi`. Es por ello que realizamos los rigurosos pasos previos de compilación y empaquetado (detallados en la **Sección 2.3 Compilación a Formato PE/COFF** con el uso de `gcc`, `ld` y `objcopy`). El binario final `.efi` contiene todo el lenguaje máquina y las cabeceras requeridas de forma autocontenida y lista para su consumo directo por la BIOS.

![image](https://hackmd.io/_uploads/HyfnN2UCZl.png)
![image](https://hackmd.io/_uploads/SJU4S2IC-e.png)
![image](https://hackmd.io/_uploads/HkE3rALCbe.png)

### 3.4. Se desmonta de forma segura el USB

**La criticidad del desmontaje seguro (sync y umount):**
Debido a la arquitectura del kernel de Linux, todas las operaciones de escritura en disco (como copiar nuestra aplicación o el archivo de la Shell) no ocurren instantáneamente en el medio físico. Para mejorar la eficiencia, el sistema emplea una caché en la memoria RAM y "aplaza" las escrituras reales (I/O asíncrona).

Si retiramos el pendrive abruptamente de la computadora en este momento, existe una certeza enorme de que los archivos `.efi` terminen truncados, parciales o el sistema de archivos FAT32 se corrompa, provocando que la BIOS falle al intentar leerlos. La ejecución de un desmontaje seguro obliga al sistema a volcar (sincronizar) toda la memoria caché pendiente en el hardware físico de la unidad (`sync`) y, finalmente, desvincular el sistema de archivos del directorio `/mnt` (`umount`). De esta forma aseguramos la integridad absoluta de nuestros binarios precargados.

![image](https://hackmd.io/_uploads/SyX2dCLAZg.png)

### 3.5. Se prueba en QEMU la aplicacion.efi

**La simulación con OVMF (EDK II):**
Antes de arriesgarnos a probar el binario en Hardware real, realizamos una simulación utilizando el emulador QEMU. Para que esta emulación se comporte de manera idéntica a una placa madre moderna, le pasamos como parámetro el firmware `OVMF_CODE_4M.fd`. OVMF (Open Virtual Machine Firmware) es el puerto de código abierto del entorno de desarrollo de firmware oficial de Intel (el **EDK II**).

Gracias a la implementación fiel del EDK II, la máquina virtual cuenta con la pila UEFI completa, soporte para sistemas de archivos FAT y carga de binarios PE/COFF, operando funcionalmente como una BIOS moderna en un entorno seguro y aislado.
Al arrancar la máquina virtual mapeando el USB físico (`/dev/sdb`), el *Default Boot Behavior* comentado anteriormente, encuentra a `BOOTX64.EFI` e incializa la Shell interactiva de TianoCore. Al ingresar a la unidad `FS0:` (nuestro pendrive USB) y ejecutar por consola la `aplicacion.efi`, podemos comprobar con éxito cómo la invocación a los Servicios de UEFI (`uefi_call_wrapper(SystemTable->ConOut->OutputString, ...)`) estructurados en nuestro código **C** logran imprimir correctamente los mensajes en pantalla y alcanzar el breakpoint esperado, emulando al 100% las condiciones físicas.

![image](https://hackmd.io/_uploads/HkVM8UYRbx.png)

### 3.6. Se prueba en la BIOS de una Lenovo Thinkpad T450 la aplicacion.efi

**Ejecución definitiva en Hardware físico (Bare Metal):**
El testeo final se llevó a cabo insertando el USB preparado en una laptop Lenovo ThinkPad T450 y entrando a su menú de booteo. Al ubicar y seleccionar nuestro pendrive, la BIOS transfirió el control en tiempo de arranque a la Shell de TianoCore alojada en `\EFI\BOOT\BOOTX64.EFI`.

Es en este momento donde resulta preeminentemente crítico **saber interpretar la tabla de mapeo (Mapping Table)** que inicializa la Shell. En ordenadores físicos con múltiples particiones de discos duros internos y tarjetas de red, los Handles se enumeran prolíficamente (FS0, FS1, BLK0, BLK1, etc).

**¿Por qué ingresamos a `FS0:`?**
Si observamos detenidamente la tabla de la primera captura, vemos que `FS0:` está mapeado al `PciRoot(...) /Pci(...) /USB(...) /HD(...)`. Esta ruta (Device Path) nos indica inequívocamente que `FS0:` corresponde a la partición de un dispositivo extraíble conectado por un puerto USB. De este modo descartamos `FS1:` que, por la firma `/Sata(...)`, sabemos que hace alusión al disco duro mecánico o estado sólido interno de la computadora.

Al tipear el comando `FS0:` e inspeccionar sus listas con el comando `ls`, logramos hallar y mandar a ejecutar de manera manual la `aplicacion.efi`. El resultado obtenido es impecable: la instrucción transita con éxito mostrando los dos strings esperados confirmando que alcanzamos el Breakpoint estático, dándonos tiempo suficiente para su lectura (gracias al `Stall` de 3 segundos).
De esta manera comprobamos de forma tajante que la rutina UEFI diseñada, compilada y abstraída durante la simulación de QEMU, se comporta exactamente de la misma manera sobre una placa electrónica real (Bare Metal), reafirmando el poderoso principio de interoperabilidad, estándar y neutralidad de hardware por parte de UEFI y la convención de su ABI.

![image](https://hackmd.io/_uploads/r1FdLLFR-g.png)
![image](https://hackmd.io/_uploads/rkgwPvIYC-l.png)
![image](https://hackmd.io/_uploads/B13tPUK0Zg.png)

## Trabajo Práctico 4: Depuración con gdb

## 4.1. Depuración con GDB + QEMU

### Preparación y pausa controlada del entorno de ejecución

Para poder observar el comportamiento a bajo nivel de nuestro ejecutable UEFI, es necesario depurar su ejecución en memoria. Para este propósito utilizaremos el depurador GDB conectándolo remotamente al emulador QEMU.

Sin embargo, para garantizar que la depuración sea controlada y no perdernos de la ejecución de nuestro programa, hemos modificado previamente el código fuente de `aplicacion.c` agregando un ciclo infinito al comienzo de la función `efi_main`:

```c
volatile int waiting = 1;
while (waiting) {} 
```

Esto fuerza a que, al momento de ejecutar nuestra aplicacion, el flujo del procesador quede atrapado intencionalmente, ganando el tiempo necesario para conectar el depurador.

A continuación, en una terminal inicializamos nuestra máquina virtual emulada pasando explícitamente ciertos parámetros:

```bash
qemu-system-x86_64 -m 512 -bios /usr/share/ovmf/OVMF.fd -net none -drive file=fat:rw:.,format=raw -s -S
```

Destacamos el uso de los flags -s y -S. El primero abre un servidor GDB en el puerto local 1234, y el segundo ("Freeze CPU at startup") obliga a QEMU a pausar por completo el procesador antes de iniciar el firmware, mostrando una pantalla negra con el mensaje "QEMU [Paused]".

![QEMU-Paused](https://hackmd.io/_uploads/S1_-OwkJze.png)

Para poder correlacionar posteriormente las direcciones de memoria de la máquina virtual (las cuales se asignan de manera dinámica) con nuestro código fuente, precisamos buscar el "offset" de la sección `.text.` Ejecutando la herramienta `objdump` sobre nuestro objeto enlazado (`aplicacion.so`), constatamos que el offset de nuestra sección `.text` (Virtual Memory Address) se ubica en el valor `0x2000`.

![offset-.text](https://hackmd.io/_uploads/BkVC_w1JMl.png)

### Conexión de GDB y suspensión del hilo principal

Con nuestro emulador pausado, abrimos una segunda terminal y lanzamos `gdb`. Para poder interceptar el arranque en tiempo real y conectarnos a QEMU, utilizamos los siguientes comandos:
```gdb
(gdb) target remote localhost:1234
(gdb) set architecture i8086
(gdb) continue
```
Al ejecutar el comando `continue` en GDB, el emulador QEMU se renueva, arrancando el firmware de la UEFI Interactive Shell. Ubicamos nuestro ejecutable en el almacenamiento `FS0:` (la simulación de nuestro disco duro USB virtualizado en el directorio actual) y lanzamos manualmente `aplicacion.efi`.

Aquí es fundamental observar que la aplicación se "congela" (se bloquea el prompt), y al mismo tiempo nuestra terminal de GDB se queda esperando indefinidamente. Esto confirma que nuestro bucle al inicio de la funcion `efi_main` funcionó: la aplicación ingresó en el ciclo infinito dependiente de nuestra variable volátil `waiting` y se encuentra retenida hasta que nosotros intervengamos su valor en memoria.

![uefi-cold](https://hackmd.io/_uploads/SkpRhPykfl.png)

### Calculo de la direccion base de la imagen y cargado de simbolos

Para cargar correctamente los símbolos de depuración usando `add-symbol-file aplicacion.so <dirección>`, debíamos calcular la dirección física donde UEFI incializó la sección `.text` en la memoria ram de nuestra VM. Para ello detuvimos la ejecución de nuestro ciclo `waiting` enviando una señal de interrupción (SIGINT) mediante `Ctrl + C`, congelando el procesador para inspeccionar su RIP (Register Instruction Pointer):

![image](https://hackmd.io/_uploads/HyLWa5ekzx.png)

Al ejecutar `info registers rip`, la CPU nos delató que estaba procesando en ese instante la dirección `0x1e07a045`. 
Para extrapolar esto, recurrimos al disensamblado de nuestro objeto (`objdump -d aplicacion.so`) donde extrajimos dos datos vitales:
1. El offset de la sección `.text` inicia en `0x2000`.
2. La instrucción de lectura del ciclo infinito donde frenamos el emulador (`mov -0x4(%rbp),%eax`) se encuentra en el offset `0x2045`.

Matemáticamente, la dirección de carga de `.text` se calcula restando el offset lógico de la instrucción a nuestro RIP actual, y sumándole el inicio lógico de la sección `.text`: 
`(0x1e07a045 - 0x2045) + 0x2000 = 0x1e07a000`

Sabiendo esto, pasamos a GDB la direccion precisa para el cargado de simbolos:
```gdb
(gdb) add-symbol-file aplicacion.so 0x1e07a000
```
![image](https://hackmd.io/_uploads/BkVC_w1JMl.png)

### Inspección paso a paso (stepi y nexti)

Una vez cargados los símbolos y establecida la vinculación con el código desensamblado, es posible avanzar la ejecución de manera atómica, instrucción por instrucción de máquina (Assembly), para monitorear las variaciones en el estado interno del procesador virtualizado:

**Ejecución de un ciclo simple (`stepi`):**
Avanza exactamente un opcode. Como se observa en la captura, nos sitúa en la instrucción `test %eax,%eax` dentro del bucle de validación.

![image](https://hackmd.io/_uploads/Sy_8Ehekzl.png)
![image](https://hackmd.io/_uploads/SJqdV3g1Mx.png)

**Avanzar saltando subrutinas (`nexti`):**
Adelanta el puntero de ejecución (RIP) hacia la siguiente instrucción secuencial `jne 0x1e07a045`. A diferencia de `step`, si la instrucción actual hubiese sido una llamada (`call`) a una función compleja (como un servicio UEFI), `nexti` la habría ejecutado íntegramente de fondo sin adentrarse dentro de ella, deteniéndose justo en la instruccion siguiente.

![image](https://hackmd.io/_uploads/rkYrrnx1Gx.png)
![image](https://hackmd.io/_uploads/HyZ_S2xkMx.png)

### Resolución del problema de Símbolos (-g) y Breakpoint Físico en 0xCC:

Durante nuestro proceso base, la aplicación fue compilada sin la flag "Debug Symbols" (`-g`). Esto provocó que nuestro depurador careciera de una tabla con los nombres de variables precompiladas (ej. `waiting` o `code`). Enfrentados a este obstáculo técnico, decidimos resolverlo operando a bajísimo nivel directamente sobre la memoria ensamblada.

Primero, mediante la lectura del `objdump` vislumbramos que nuestra instrucción objetivo (`cmp $0xcc,%al`) resposa en el offset numérico `0x207D`. Puesto que sabemos que nuestra base de texto se aloja en `0x1e07a000`, calculamos la dirección física exacta para el Breakpoint:
`0x1e07a000 + (0x207D - 0x2000) = 0x1e07a07D`

Así, asignamos el Breakpoint explícito por registro en memoria:
`(gdb) break *0x1e07a07d`

Segundo, forzamos la evasión del ciclo bloqueante infinito que retiene el programa modificando el valor lógico a "0" directamente sobre el sub-frame de la pila (`$rbp - 4`) que le asignó el compilador:
`(gdb) set {int}($rbp - 4) = 0`
![image](https://hackmd.io/_uploads/HkJZjnxJfe.png)

Tercero, ordenamos la continuación libre del procesador `(gdb) continue`. El programa sale exitosamente del loop y se detiene hasta nuestro Breakpoint colocado justo antes de la evaluación condicional buscada:

![image](https://hackmd.io/_uploads/HkBC9nxyMl.png)

### Comprobación Crítica de los Registros y Memoria (Fase Final)

Habiendo frenado el programa en el instante preciso, corroboramos cómo la CPU interactúa con el `0xCC` inyectado por software:

- **Escrutando memoria (`x/1bx $rbp-5`)**: Revisamos el byte individual plantado en el marco del Stack actual. Se visualiza `0xcc`, validando la inserción del dato en la tabla RAM.
- **Examinando los registros del núcleo (`info registers`)**: Se evidencia que en efecto el registro RAX tiene almacenado el valor hexadecimal procesado (`204` en decimal).
- **Proximidad operativa (`x/10i $rip`)**: Validamos que la siguiente instruccion resulta en `cmp $0xcc,%al`. Demostrando la precisión de calculo de nuestro Breakpoint insertado.

![image](https://hackmd.io/_uploads/H1Ot6ng1fg.png)


## 4.2. Conectar Ghidra a GDB (análisis híbrido)

Habiendo completado la depuración en crudo por la terminal de GDB, pasamos a conectar el depurador integrado de Ghidra (v12.0.4 vía GDB Remoto) en busca de una interfaz visual que nos agilizara el trabajo. Siendo totalmente sinceros, terminamos viendo la misma información pero con paneles mucho más vistosos, e irónicamente terminamos dependiendo de su consola integrada porque se nos dio mucho mejor a la hora de resolver a mano los fallos visuales de la memoria dinámica, aunque el mapa visual de los registros valió la pena.

### Configuración de la conexión e importación

Con nuestro servidor QEMU corriendo en segundo plano y pausado a la espera (`-s -S`), abrimos nuestra aplicación importada en Ghidra e iniciamos su faceta de "Debugger".
Desplegamos el menú `Debugger → Debug aplicacion.efi` y seleccionamos el entorno de conexión in-built "Remote GDB" apuntando directamente a nuestro servidor de emulación local (`localhost:1234`).

![debug_open_ghidra](https://hackmd.io/_uploads/ByY6YyWkMl.png)
![debug_gdb_remote](https://hackmd.io/_uploads/BJkMcJbJfg.png)
![gdb_remote_config](https://hackmd.io/_uploads/SJy_Y1b1zx.png)
![prompt_gdb_connect](https://hackmd.io/_uploads/BJ1dQlZyMl.png)

### Sincronización del entorno, Desensamblado Manual y Breakpoint

A pesar del entorno visual sumamente detallado, en la práctica nos enfrentamos a una limitación técnica: dado que UEFI asigna un nuevo espacio en la memoria RAM de manera dinámica con cada reconexion entre gdb y qemu, el mapeo estático de Ghidra tendió a desincronizarse visualmente con los bytes de ejecución (mostrando porciones en `??`), lo que impedía el uso confiable de los breakpoints gráficos del entorno UI.

Para sortear esto y encontrar nuestro código, la solución fue mucho más "artesanal": nos posicionamos sobre esos bloques con `??` y presionamos la tecla **'D'** (*Disassemble*). Por un comportamiento propio del entorno de Ghidra, esto obliga al desensamblador a forzar la traducción de esa porción de la memoria y revelar las líneas de assembly en vivo que se escondían debajo. 
    
Una vez reveladas, buscamos a simple vista y logramos dar con la misma instrucción condicional (`CMP AL, 0x0cc`) que ya habíamos analizado a fondo en el TP anterior usando solo consola. Así pudimos ubicar a ojo que su dirección física actual recaía en `0x1e07a07d`. Con esto ya localizado, simplemente utilizamos la consola inferior de Ghidra para configurar todo formalmente:

1. **Carga de símbolos re-mapeada:** Inyectamos los símbolos de la aplicación en la nueva dirección base temporal de la memoria virtual `add-symbol-file aplicacion.so 0x1e07a000`.
2. **Fijación del Breakpoint:** Con la confirmación visual de la instrucción gracias a la tecla 'D', marcamos puntualmente la trampa al procesador en esa ubicación: `break *0x1e07a07d`.

![add_symbol_break_CMP](https://hackmd.io/_uploads/rJGkRZ-yGx.png)
![break_interface_ghidra](https://hackmd.io/_uploads/ryRjTZZyze.png)

### Inyección de valores condicionales y captura del registro AL

Completada nuestra configuración perimetral de memoria, forzamos a la Máquina Virtual a salir del bloqueo infinito y comprobamos visualmente nuestro `0xCC`. 
Reescribimos la memoria subyacente modificando desde la consola híbrida de Ghidra el booleano retenido en el stack (`set {int}($rbp - 4) = 0`) para escapar del bucle y liberamos la ejecución de la CPU usando el comando `continue`.

![waiting_zero_and_continue](https://hackmd.io/_uploads/SkHC0W-JGe.png)

La ejecución transcurrió internamente hasta colisionar con nuestro Software Breakpoint inyectando un alto total virtual. 
El análisis híbrido rindió un fruto potente: el panel de *Listing* de Ghidra estacionó su flujo de instrucciones marcando de verde un detenido `CMP AL, 0x0cc`; y, deteniendose justamente en el Breakpoint que le marcamos, junto con el detallado panel *Registers* que nos desglosó el interior matemático del núcleo, señalando explícitamente que el sub-registro lógico `AL` ahora albergaba nuestro valor modificado condicional `cc`.

![continue_break_AL_Register_0xCC](https://hackmd.io/_uploads/H13fJMZJzg.png)


## Conclusiones

Hacer este trabajo práctico fue un viaje intenso, bajando desde un código amigable en C hasta las entrañas mismas de los registros del procesador. Arrancamos interactuando con la UEFI Shell y compilando una aplicación, pero rápido nos dimos cuenta de que programar para un entorno pre-SO tiene sus mañas: tuvimos que pelearnos con la convención de llamadas (las famosas ABI de Microsoft vs System V de Linux) armando un "puente" solo para poder imprimir un texto en pantalla sin que la notebook (nuestra fiel ThinkPad T450 de la facultad) crasheara al probarlo en hardware real (Bare Metal).

Pero sin duda, la etapa más desafiante y gratificante fue la depuración forense a bajo nivel. Al compilar sin símbolos (`-g`), GDB estaba "ciego". Tuvimos que sacar la calculadora, pausar el núcleo de QEMU, agarrar el registro RIP "al vuelo", y restarle offsets lógicos al código desensamblado para atrapar dónde UEFI había cargado nuestro programa dinámicamente en la RAM. 

A partir de ahí, fue domar al procesador: calculamos direcciones en hexadecimal para plantar breakpoints físicos, manipulamos celdas de memoria cruda (`$rbp - 4 = 0`) para escapar de un bucle infinito en el que nosotros mismos nos encerramos, y conectamos el entorno de Ghidra para ver gráficamente el resultado. Confirmar que podíamos inyectar datos y ver cómo el registro `AL` adoptaba nuestro `0xCC` demostró que logramos un control total.

En resumen: armamos un binario nativo .efi desde cero, lo corrimos en QEMU, lo llevamos a la placa física superando la abstracción del hardware, y lo revertimos con ingeniería inversa. Terminamos entendiendo no solo cómo arranca una computadora moderna, sino cómo "piensa" y mastica la memoria el procesador cuando ningún sistema operativo lo está vigilando.
