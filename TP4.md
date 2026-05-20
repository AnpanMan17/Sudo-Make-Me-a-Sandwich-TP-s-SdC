# Trabajo Práctico N°4: Modulos de Kernel

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


# Desafios

## Desafío #1 

### 1) ¿Qué es checkinstall y para qué sirve?

Cuando por ejemplo descarguemos un `archivo.tar.gz` y tratamos de preparar la ejecucion con `./` generara un archivo Makefiles (ademas de otras cosas) este archivo podra compilar el codigo fuente y generara los binarios ejecutables, luego usando`sudo make install` estos archivos compilados se copiaran directamente al sistema, pero se copiaran a distintos directorios. De esto no se entera el gestor de paquetes `dpkg`o `apt`; Entonces la herramienta **checkinstall** en lugar de hacer esto, intercepta el **Makefile**, rastrea todos los archivos compilados que se iban a instalar, instalando un solo archivo `.deb` para Ubuntu/Debian o `.rpm` para Fedora. Haciendolo mas simple para una instalacion/desintalacion con un solo comando.

### 2) ¿Se animan a usarlo para empaquetar un hello world ? 

``` C
#include <stdio.h>

int main(){
    printf("Hello World! %s\n", "I will be a pakage .deb");
    return 0;
}
```
y el makefile a usar:

``` Makefile
CC = gcc
CFLAGS = -Wall

all: hello_world

hello_world: hello_world.c
	$(CC) $(CFLAGS) -o hello_world hello_world.c

install: hello_world
	cp hello_world /usr/local/bin/

clean:
	rm -f hello_world

```

hacemos `make` para compilar el codigo y `sudo checkinstall` para crear el paquete `.deb`.

si ejecutamos en la terminal el comando `hello_world` obtendremos como resultado:

```
saqib@saqibdaniel:~/Desktop/Sudo-Make-Me-a-Sandwich-TP-s-SdC/desafio1$ hello_world
Hello World! I will be a pakage .deb
```
_**NOTA:**_ el paquete `.deb` se encuentra en la carpeta `desafio1/` del directorio raiz.

* [**`sudo-make-a-sandwich/desafio1/`**](https://github.com/Sachan-2512/Sudo-Make-Me-a-Sandwich-TP-s-SdC/tree/TP4/desafio1)

### 3) Revisar la bibliografía para impulsar acciones que permitan mejorar la seguridad del kernel, concretamente: evitando cargar módulos que no estén firmados. rootkits ? 

Primero que la definicion de un **rootkit** es un conjunto de herramientas de software maliciocoso diseñado para obtener o subir permisos a **root** en una computadora de forma no autorizada. Los crackers intentan penetrar el sistema mediante rootkits instalados como **Modulos de Kernel**; para asi obtener el control total del **Sistema operativo**; de esta forma pueden ocultar archivos, procesos y conexiones de red sin que el **antivirus** lo vea.

Los **modulos firmados** sirven como defensa para que un atacante no cargue un **rootkit** en el **kernel**. Existe una medida de seguridad donde se requiere que los modulos esten `firmados criptograficamente`, ya sea con una clave publica-privada por ejemplo; o por desarrolladores autorizados que firman los modulos con la clave privada y el kernel la verifica con la clave publica. De esta forma si alguien intenta cargar un modulo malicioso, el kernel verificara la firma digital contra sus llaves de confianza. Si la firma del modulo no coincide el sistema rechaza la carga (insmod fallara).

Con Secure Boot activado:

1) la BIOS/UEFI verifica el bootloader
2) el bootloader verifica el kernel
3) el kernel verifica módulos

Eso crea una cadena de confianza (“chain of trust”).

## Desafío #2

### 1) ¿Qué funciones tiene disponible un programa y un módulo ?

Un programa normal tiene a su disposicion todas las librerias estandar del sistema, como la libreria estandar de C (libc). Un modulo no usa la libreria estandar de C ni ninguna otra, se comunica solo con las funciones internas de la API que se exponen en el nucleo de Linux.

### 2) Espacio de usuario o espacio del kernel.

El espacio de usuario es el entorno restringido donde corren las aplicaciones que no tienen acceso directo al hardware. El espacio de Kernel es el corazon del SO con privilegios absolutos es donde se gestiona la memoria, procesador y el hardware fisico; si un modulo de kernel tiene un error, se corrompe el sistema y provoca un cuelgue generalizado llamado `Kernel Panic`.

### 3) Espacio de datos.

En el espacio de usuario, un programa (gracias a la `memoria virtualizada`) cree que tiene toda la memoria para si misma, y la memoria de un programa A esta bien separada de la de un programa B. En el espacio del kernel, la memoria no se divide asi, todos los modulos comparten un unico espacio de direcciones de memoria con el resto del nucleo, pero un `puntero mal dirigido` de un modulo puede llegar a sobrescribir datos vitales de otro componente del SO.

### 4) Drivers. Investigar contenido de /dev.

El Driver, es un especifico tipo de modulo de kernel diseñado para servir como `traductor` entre el SO y el dispositivo fisico.

El directorio `/dev` contiene archivos que son nodos de dispositivos que sirven como puertas de comunicacion, el disco duro por ejemplo se representa como `/dev/sda` un programa de usuario puede pedirle al SO leer este archivo.

## Preparacion:

```
sudo apt-get update
```

Luego hacemos:

``` bash
sudo apt-get install build-essential checkinstall linux-source linux-headers-$(uname -r)

No se pudo instalar el "kernel package" debido a que es un modulo obsoleto del sistema y a su vez se instalara linux-headers por los siguientes motivos:

linux-headers-$(uname -r). sirve para compilar módulos sencillos lo que el sistema realmente busca son los "headers" (las cabeceras) exactas de la versión del kernel que estás corriendo ahora mismo.
```

Las cosas que se instalaron sirven para los siguiente motivos:

* **build-essential:** Instala las herramientas básicas para compilar código en C/C++ (como el compilador gcc y la herramienta make), indispensables para compilar tu módulo.

* **checkinstall:**  Sirve para crear paquetes instalables (como los .deb) a partir del código fuente.

* **kernel-package (aunque no se instalo):** Contiene utilidades que facilitan la compilación de kernels de Linux.

* **linux-source:** Descarga el código fuente del kernel de Linux. 


Ahora tenemos que crearnos una cuenta de gitlab y ademas hacer un fork del repositorio: 
https://gitlab.com/sistemas-de-computacion-unc/kenel-modules.git

Una vez hagamos hcho el fork clonamos el repositorio.

Entramos desde la terminal y vamos hasta  el directorio `module/`
y ejecutamos por terminal `make`, para compilar con Makefile el modulo.

Entonces obtendremos un archivo llamado `mimodulo.ko` que sera el archivo compilado que podemos enchufar o insertar en nuestro kernel de linux.

### Hacemos entonces: 

```
sudo insmod mimodulo.ko
```

Si usamos `sudo dmesg` observaremos un cierto log del kernel, y al final de este observaremos que el modulo se inserto correctamente:
```
[17801.039873] mimodulo: module verification failed: signature and/or required key missing - tainting kernel
[17801.040809] Modulo cargado en el kernel.
```

Y con este comando verificaremos que este listado:

```
$ lsmod | grep mod
mimodulo               12288  0
```

Ahora lo que haremos es quitar el modulo listado y usar una herramienta llamada ```modinfo``` para analizarlo:

Primero ejecutamos el comando para remover `rmmod`

```
sudo rmmod mimodulo
```

Revisamos los registros nuevamente con:

```
sudo dmesg 
```

Y observamos como fue descargado del kernel:

```
[17943.846696] audit: type=1400 audit(1779073202.030:156): apparmor="DENIED" operation="connect" class="file" profile="/usr/sbin/cups-browsed" name="/run/systemd/resolve/io.systemd.Resolve" pid=22268 comm="cups-browsed" requested_mask="wr" denied_mask="wr" fsuid=117 ouid=991
[18623.406113] Modulo descargado del kernel.
```

Volvemos a listar los modulos cargados con: 
```
lsmod | grep mod
```

Tambien se puede verificar el /proc en una seccion que tiene podemos verificar eso con el comando: 
```
cat /proc/modules | grep mod 
```

En ambos casos no aparecio nada.

Ahora para un analisis final usaremos los comandos **modinfo**:

Usando `modinfo mimodulo.ko`

```
modinfo mimodulo.ko
filename:       /home/saqib/Desktop/Sudo-Make-Me-a-Sandwich-TP-s-SdC/kenel-modules/part1/module/mimodulo.ko
author:         Catedra de SdeC
description:    Primer modulo ejemplo
license:        GPL
srcversion:     C6390D617B2101FB1B600A9
depends:        
retpoline:      Y
name:           mimodulo
vermagic:       6.8.0-117-generic SMP preempt mod_unload modversions 
```

Usando `modinfo /lib/modules/$(uname -r)/kernel/crypto/des_generic.ko.zst`

```
filename:       /lib/modules/6.8.0-117-generic/kernel/crypto/des_generic.ko.zst
alias:          crypto-des3_ede-generic
alias:          des3_ede-generic
alias:          crypto-des3_ede
alias:          des3_ede
alias:          crypto-des-generic
alias:          des-generic
alias:          crypto-des
alias:          des
author:         Dag Arne Osvik <da@osvik.no>
description:    DES & Triple DES EDE Cipher Algorithms
license:        GPL
srcversion:     B56606AD918CF0074D320DB
depends:        libdes
retpoline:      Y
intree:         Y
name:           des_generic
vermagic:       6.8.0-117-generic SMP preempt mod_unload modversions 
sig_id:         PKCS#7
signer:         Build time autogenerated kernel key
sig_key:        6D:04:C4:BF:F4:02:05:5D:BD:2A:6A:72:72:2B:44:11:FF:31:09:80
sig_hashalgo:   sha512
signature:      5B:34:93:6A:54:A2:A1:FF:0E:D0:4A:80:58:E0:1F:9D:87:8F:A4:0F:
                E0:3E:8B:87:66:16:08:74:5E:31:D1:B6:4B:A1:27:DB:12:7F:0F:DA:
                .
                .
                .
```
### 1) ¿Qué diferencias se pueden observar entre los dos modinfo ? 

El modulo oficial **des_generic** cuenta con campos criptograficos como: `sig_id`, `signer`, `sig_key`, `sig_hashalgo` y la `signature`. Fue **firmado durante la compilacion oficial del kernel**, el modulo **mimodulo.ko** `no tiene firma`. 

El modulo oficial **des_generic** posee la etiqueta `intree`, que significa que `pertenece al arbol de codigo fuente oficial`.

El modulo oficial contiene muchas lineas de `alias`, esto le sirve al kernel para saber exactamente que algoritmos o dispositivos de hardware deben "despertar" a este driver. El otro modulo carece de alias.

El modulo oficial depende de otro modulo llamado `libdes`, nuestro modulo es autonomo. 

### 2) ¿Qué drivers/modulos estan cargados en sus propias pc? comparar las salidas con las computadoras de cada integrante del grupo. Expliquen las diferencias. Carguen un txt con la salida de cada integrante en el repo y pongan un diff en el informe.

Cada integrante debe ejecutar en su terminal (posicionado en el directorio **`lsmod`**):

```
lsmod > lsmod_apellido.txt
```

Para comprobar las diferencias de a pares:

```
diff -u lsmod_mohammad.txt lsmod_sergio.txt
diff -u lsmod_mohammad.txt lsmod_laura_surco.txt
diff -u lsmod_sergio.txt lsmod_laura_surco.txt
```

Para comprobar las diferencias de los tres archivos a la vez:

```
diff3 lsmod_mohammad.txt lsmod_sergio.txt lsmod_laura_surco.txt
```
La diferencia entre modulos se vera dada principalmente por el hardware fisico que se posee.

Las salidas y comparaciones entre la computadora de cada integrante del grupo estaran en la carpeta `lsmod/` del directorio raiz:

* [**`sudo-make-a-sandwich/lsmod/`**](https://github.com/Sachan-2512/Sudo-Make-Me-a-Sandwich-TP-s-SdC/tree/TP4/lsmod)

### 3) ¿cuales no están cargados pero están disponibles? que pasa cuando el driver de un dispositivo no está disponible. 

Los controladores o módulos que están disponibles pero no cargados se encuentran almacenados físicamente en el disco duro, específicamente en la ruta `/lib/modules/$(uname -r)/kernel/`. En este estado, el sistema reconoce su existencia pero **no consumen memoria RAM**, ya que no han sido insertados en el núcleo debido a que el hardware asociado no está conectado o en uso.

Por otro lado, cuando el controlador de un dispositivo no está disponible, al conectar dicho hardware el kernel solo logra detectar su identificador (`Device ID`). Al no encontrar un módulo con el alias correspondiente para manejarlo, no se crea el nodo del dispositivo en `/dev`, y por lo tanto, el dispositivo queda completamente inutilizable.


### 4) Correr hwinfo en una pc real con hw real y agregar la url de la información de hw en el reporte. 

Para instalar la herramienta y generar el reporte abreviado:

```
sudo apt install hwinfo
hwinfo --short > informe_hardware.txt
```
Los reportes de los integrantes de grupos estaran en la carpeta `informes_hardware/` del directorio raiz:

* [**`sudo-make-a-sandwich/informes_hardware/`**](https://github.com/Sachan-2512/Sudo-Make-Me-a-Sandwich-TP-s-SdC/tree/TP4/informes_hardware)

### 5) ¿Qué diferencia existe entre un módulo y un programa  ? 

El modulo no inicia en la funcion `main()`, sino que tiene funciones de ciclo de vida como `init_module`. Es **basada en eventos del kernel**, no es secuencial. No usa la libc, sino que **usa APIs internas del kernel**. Un error corrompe la memoria del SO, provocando un `Kernel Panic`.

### 6) ¿Cómo puede ver una lista de las llamadas al sistema que realiza un simple helloworld en c?

Se utiliza la herramienta de rastreo strace. Si tienes tu ejecutable compilado, corres en la terminal:

```
strace ./hello_world
```

Esto interceptará y mostrará en pantalla cada interacción del programa con el kernel (como `execve`, `brk`, `openat`, y la llamada `write` para imprimir el texto en la pantalla).

### 7) ¿Qué es un segmentation fault? ¿Cómo lo maneja el kernel y como lo hace un programa?

Es una **excepción de hardware** generada por la Unidad de Manejo de Memoria `(MMU)` cuando un proceso intenta acceder a segmentos de memoria a los cuales no tiene permisos (por ejemplo, memoria que pertenece al kernel o a otro proceso).

**Manejo por parte del kernel:**
El procesador interrumpe la ejecución del programa y le cede el control al kernel. Este identifica qué proceso causó la infracción y le envía una señal de terminación, típicamente `SIGSEGV` (señal de violación de segmento).

**Manejo por parte de un programa:**
Por defecto, los programas no están diseñados para defenderse de este tipo de errores; al recibir la señal `SIGSEGV`, el proceso finaliza inmediatamente (crash). Sin embargo, un programador puede preparar el programa para capturar y manejar esta señal de forma controlada utilizando funciones como `signal()` o `sigaction()`.

### 8) ¿Se animan a intentar firmar un módulo de kernel ? y documentar el proceso ?  https://askubuntu.com/questions/770205/how-to-sign-kernel-modules-with-sign-file

Si, lo haremos desde el `mimodulo.c` editaremos el codigo fuente por: 

``` C
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Catedra de SdeC");
MODULE_DESCRIPTION("Primer modulo ejemplo");

int modulo_lin_init(void) {
    printk(KERN_INFO "[Equipo Sudo-Make-Me-a-Sandwich] Modulo cargado en el kernel.\n");
    return 0;
}

void modulo_lin_clean(void) {
    printk(KERN_INFO "[Equipo Sudo-Make-Me-a-Sandwich] Modulo descargado del kernel.\n");
}

module_init(modulo_lin_init);
module_exit(modulo_lin_clean);
```

Haremos lo siguiente para borrar el archivo compilado viejo:

```
make clean
make
```

Y lo volvemos a insertar con:

```
sudo insmod mimodulo.ko
```

Y lo descargamos con el siguiente comando:

```
sudo rmmod mimodulo
```

### 9) Agregar evidencia de la compilación, carga y descarga de su propio módulo imprimiendo el nombre del equipo en los registros del kernel. 

Para observar la evidencia se utilizara el siguiente comando:

```
sudo dmesg | tail -n 15
```

Y lo que nos ofrece por salida es:

```
 5570.953735] mimodulo: loading out-of-tree module taints kernel.
[ 5570.953754] mimodulo: module verification failed: signature and/or required key missing - tainting kernel
[ 5570.954131] [Equipo Sudo-Make-Me-a-Sandwich] Modulo cargado en el kernel.
[ 5600.081610] [Equipo Sudo-Make-Me-a-Sandwich] Modulo descargado del kernel.
```

### 10) ¿Que pasa si mi compañero con secure boot habilitado intenta cargar un módulo firmado por mi? 

El intento fallara y la terminal devolvera un error de operacion no permitida, aunque el modulo este firmado la clave privada que usaste es tuya. Pero el chip UEFU/BIOS de la computadora de mi compañera no confia en ese clave publica. Se puede cargar siempre y cuando mi compañero cargue mi certificado publico dentro del gestor MOK de su propia maquina.


### 11) Dada la siguiente nota https://arstechnica.com/security/2024/08/a-patch-microsoft-spent-2-years-preparing-is-making-a-mess-for-some-linux-users/ 

* ¿Cuál fue la consecuencia principal del parche de Microsoft sobre GRUB en sistemas con arranque dual (Linux y Windows)?

> El parche aplicó una actualización de políticas `SBAT` (Secure Boot Advanced Targeting) diseñada para bloquear `versiones antiguas y vulnerables` del cargador de arranque GRUB. Sin embargo, debido a un error de implementación, el parche bloqueó también las `versiones legítimas y actualizadas` de `GRUB` de muchas distribuciones de `Linux` (como Ubuntu, Mint, Debian). Esto causó que los sistemas con Dual Boot no pudieran iniciar Linux, mostrando pantallas de error de violación de seguridad.

* ¿Qué implicancia tiene desactivar Secure Boot como solución al problema descrito en el artículo?

> Desactivar `Secure Boot` permite el arranque inmediato de GRUB y de `Linux` saltándose el `bloqueo de Microsoft`. Sin embargo, la implicancia negativa es que se elimina por completo la barrera de protección del firmware en el inicio. Esto deja expuesta a la computadora ante ataques de malware avanzados que se inyectan antes de que cargue el sistema operativo (como bootkits o rootkits).

* ¿Cuál es el propósito principal del Secure Boot en el proceso de arranque de un sistema?

> Su propósito es garantizar que la computadora inicie utilizando únicamente software confiable. Para ello, verifica las firmas digitales criptográficas de cada pieza de código que se ejecuta en el arranque (el firmware `UEFI/LEGACY`, el cargador de arranque `(GRUB)`, el `kernel` y los `módulos críticos`). Si alguna pieza no está firmada o su firma fue alterada, el sistema detiene el arranque para prevenir la ejecución de código malicioso.

