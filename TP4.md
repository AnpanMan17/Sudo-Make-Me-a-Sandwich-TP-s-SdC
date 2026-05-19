





# Desafios

## Desafío #1 

### 1) ¿Qué es checkinstall y para qué sirve?

Cuando por ejemplo descarguemos un archivo.tar.gz y tratamos de preparar la ejecucion con ./ generara un archivo Makefiles (ademas de otras cosas) este archivo podra compilar el codigo fuente y generara los binarios ejecutables, luego usando sudo make install estos archivos compilados se copiaran directamente al sistema, pero se copiaran a distintos directorios. Esto no se entera tu gestor de paquetes dpkg o apt; Entonces la herramienta checkinstall en lugar de hacer esto, intercepta el Makefile, rastrea todos los archivos compilados que se iban a instalar instalando un solo archivo .deb para Ubuntu/Debian o .rpm para Fedora. Haciendolo mas simple para una desintalacion con un comando.

### 2) ¿Se animan a usarlo para empaquetar un hello world ? 

```
#include <stdio.h>

int main(){
    printf("Hello World!\n" , "I will be a pakage .deb");
    return 0;
}
```
y el makefile a usar:

```
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

hacemos make para compilar el codigo y sudo checkinstall para crear el paquete .deb.




### 3) Revisar la bibliografía para impulsar acciones que permitan mejorar la seguridad del kernel, concretamente: evitando cargar módulos que no estén firmados. rootkits ? 

## Desafío #2

### 1) ¿Qué funciones tiene disponible un programa y un módulo ?

### 2) Espacio de usuario o espacio del kernel.

### 3) Espacio de datos.

### 4) Drivers. Investigar contenido de /dev.



primera parte se prepara el entorno de preparacion e instalaicon

primero el sudo apt-get update

luego hacemos:

sudo apt-get install build-essential checkinstall linux-source linux-headers-$(uname -r)

no se pudo instalar el kernel package debido a que es un modulo obsooleto del sistema y asu vez se instalara linux eheaders por los siguientes motivos:

linux-headers-$(uname -r). Aunque tu TP pide linux-source (el código fuente completo del kernel), hoy en día para compilar módulos sencillos lo que el sistema realmente busca son los "headers" (las cabeceras) exactas de la versión del kernel que estás corriendo ahora mismo.


las cosas que se instalaron sirven para los siguiente motivos:

¡Perfecto! Preparar el entorno es el primer gran paso. Para hacer esto, necesitas abrir la terminal de tu sistema operativo Linux (si estás usando una máquina virtual con Ubuntu, Debian, Linux Mint o similares, este proceso te funcionará perfecto porque usan el gestor de paquetes apt).

Aquí tienes el paso a paso detallado de lo que pide tu Trabajo Práctico:

1. Actualizar los repositorios (Recomendado)
Antes de instalar cosas nuevas, siempre es una buena práctica decirle a tu sistema que actualice su lista de software disponible. En tu terminal, escribe esto y presiona Enter:
sudo apt-get update
(Nota: Al usar sudo, el sistema te pedirá tu contraseña. Cuando la escribas, no se verán los asteriscos en la pantalla, pero se está escribiendo. Solo teclea y dale Enter).

2. Instalar los paquetes solicitados
Ahora vamos a ejecutar exactamente el comando que te pide el TP:
sudo apt-get install build-essential checkinstall kernel-package linux-source   

Para que no instales cosas a ciegas, te explico rápido qué hace cada uno de estos programas:

build-essential: Instala las herramientas básicas para compilar código en C/C++ (como el compilador gcc y la herramienta make), indispensables para compilar tu módulo.

checkinstall: Es la herramienta sobre la que te preguntan en el Desafío #1. Sirve para crear paquetes instalables (como los .deb) a partir del código fuente.

kernel-package: Contiene utilidades que facilitan la compilación de kernels de Linux.


linux-source: Descarga el código fuente del kernel de Linux. Como dice tu TP, esta descarga puede demorar unos minutos dependiendo de tu conexión a internet



Ahora tenemos que crearnos una cuenta de gitlab y ademas hacer un fork del repositorio: 
https://gitlab.com/sistemas-de-computacion-unc/kenel-modules.git

Una vez hagamos hcho el fork clonamos el repositorio.

entramos desde la terminal y vamos hasta  el directorio module/
y ejecutamos por terminal make, para compilar con Makefile el modulo

entonces obtendremos un archivo llamado mimodulo.ko el archivo compilado que podemos enchufar o insertar en nuestro kernel de linux.

Hacemos entonces: sudo insmod mimodulo.ko

Si usamos sudo dmesg observaremos un cierto log del kernel y alf inal de este observaremos que le modulo se inserto correctamente:

[17801.039873] mimodulo: module verification failed: signature and/or required key missing - tainting kernel
[17801.040809] Modulo cargado en el kernel.

y con este comando verificaremos que este listado:

$ lsmod | grep mod
mimodulo               12288  0


Ahora lo que haremos es quitar el modulo listado y usar una herramineta llamada modinfo para analizarlo:

Primero ejecutamos el comando para remover rmmod

sudo rmmod mimodulo

revisamos los registros nuevamente con:

sudo dmesg 


y observamos como fue descargado del kernel:

[17943.846696] audit: type=1400 audit(1779073202.030:156): apparmor="DENIED" operation="connect" class="file" profile="/usr/sbin/cups-browsed" name="/run/systemd/resolve/io.systemd.Resolve" pid=22268 comm="cups-browsed" requested_mask="wr" denied_mask="wr" fsuid=117 ouid=991
[18623.406113] Modulo descargado del kernel.


volvemos a listar los modulos cargados con: lsmod | grep mod
no apareciendo nada. tambien se puede verificar el /proc en una seccion que tiene podemos verificar eso con el comando: cat /proc/modules | grep mod no apareciendo nadan.

AHora para un analisis final usaremos los comandos modinfo

Usando modinfo mimodulo.ko:
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


Usando modinfo /lib/modules/$(uname -r)/kernel/crypto/des_generic.ko.zst

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
                26:51:DC:22:BA:1B:6F:1F:9A:28:B1:DB:A8:E9:40:2D:27:23:19:3E:
                5E:6F:D2:41:7E:0A:1A:7A:58:E9:6A:4D:9B:80:DD:03:6D:AB:92:C9:
                57:F6:A2:DF:0C:E8:C1:8C:D9:85:B7:B3:EA:93:E4:12:0B:3F:99:B4:
                17:A8:F0:74:6F:FD:0B:F2:0F:51:C5:0E:97:72:4B:D5:E5:27:BF:84:
                A4:AB:6E:8A:90:0D:AC:15:8D:57:01:95:F2:32:91:47:FF:B7:29:48:
                EA:61:C2:A9:DA:77:8F:49:AD:B8:21:D8:A9:21:76:B8:61:2B:1F:00:
                79:E6:A0:55:E2:63:C4:E6:39:13:A8:42:C6:84:E7:E2:85:8F:65:AD:
                C1:10:D8:2A:4E:9C:33:FB:0A:64:9C:D4:63:D1:5C:DF:2A:7A:D4:E2:
                2B:46:1A:5F:E1:1C:F4:87:EE:02:A0:AA:18:BB:36:3F:3D:8D:24:5F:
                97:0D:FC:7A:73:AB:8C:55:1B:17:4F:70:11:DA:11:DF:E0:6B:E1:53:
                45:98:38:30:08:D1:6E:B9:0E:A4:A4:53:86:13:0C:8A:5F:4E:BA:39:
                DE:43:73:F1:85:96:1F:99:04:05:0B:E1:6E:39:0B:EE:35:A1:0B:17:
                70:CB:7E:30:E2:29:E3:D7:05:7A:F6:F2:C8:1E:CD:C6:E0:9C:AF:D9:
                8D:98:45:A2:1C:32:85:36:1B:36:8E:CB:C1:ED:B2:83:7F:1A:0B:94:
                7A:26:11:70:62:D6:5E:82:2A:AA:9C:E2:9A:EE:83:66:BD:B3:9A:38:
                A0:A1:25:5E:96:18:83:98:72:62:6D:56:B1:68:EA:B8:BA:81:FD:A9:
                33:A0:AD:62:87:5C:12:C0:CC:69:EB:39:B7:2A:39:3D:2E:53:CC:7A:
                97:09:43:66:86:53:F4:56:A8:C6:B5:7E:62:69:EC:A0:65:76:DF:C2:
                C2:9C:0A:33:1F:98:87:C8:06:8C:75:A6:88:E9:1B:CB:38:70:E6:AE:
                D6:84:D3:AC:B4:EB:55:DE:AF:AA:22:8A:55:7F:72:A2:3E:E4:3B:77:
                46:FD:91:62:12:E6:22:76:30:FC:AB:33:15:61:E5:19:73:A9:EA:47:
                0A:01:3C:96:3A:0C:9E:2A:9F:82:EB:FD:18:6B:9D:B9:F4:00:7D:F0:
                F9:5C:E2:1C:4B:FC:52:12:3C:99:94:48:81:9C:25:20:06:36:0B:54:
                61:71:13:9D:4C:93:67:3D:5C:99:7D:3F

```

