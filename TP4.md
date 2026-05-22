# Trabajo Práctico N°4: Módulos de Kernel

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


# Desafíos

## Desafío #1 

### 1) ¿Qué es checkinstall y para qué sirve?

Cuando descargamos un proyecto en formato `tar.gz`, normalmente se lo descomprime para luego ejecutar el proceso de compilación correspondiente. En muchos casos, esto genera un `Makefile`, que permite compilar e instalar el programa con `make` y `sudo make install`. El detalle es que esa instalación se realiza directamente sobre el sistema y el gestor de paquetes `dpkg` o `apt` no registra esos cambios. En ese sentido, **checkinstall** se utiliza para interceptar la instalación y generar un único paquete instalable, por ejemplo un `.deb` en Debian o Ubuntu, o un `.rpm` en otras distribuciones, facilitan tanto la instalación como la desinstalación posterior.

### 2) ¿Se animan a usarlo para empaquetar un hello world ? 

``` C
#include <stdio.h>

int main(){
    printf("Hello World! %s\n", "I will be a package .deb");
    return 0;
}
```
El makefile a usar:

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

Hacemos `make` para compilar el código y `sudo checkinstall` para crear el paquete `.deb`.

Si ejecutamos en la terminal el comando `hello_world` obtendremos como resultado:

```
saqib@saqibdaniel:~/Desktop/Sudo-Make-Me-a-Sandwich-TP-s-SdC/desafio1$ hello_world
Hello World! I will be a package .deb
```
_**NOTA:**_ el paquete `.deb` se encuentra en la carpeta `desafio1/` del directorio raiz.

* [**`sudo-make-a-sandwich/desafio1/`**](https://github.com/Sachan-2512/Sudo-Make-Me-a-Sandwich-TP-s-SdC/tree/TP4/desafio1)

### 3) Revisar la bibliografía para impulsar acciones que permitan mejorar la seguridad del kernel, concretamente: evitando cargar módulos que no estén firmados. rootkits ? 

Primero, la definición de un **rootkit** es un conjunto de herramientas de software malicioso diseñado para obtener o escalar privilegios hasta **root** en una computadora de forma no autorizada. Los crackers intentan penetrar el sistema mediante rootkits implementados como **Módulos de Kernel** para obtener control privilegiado sobre el **Sistema operativo**. De esta forma pueden ocultar archivos, procesos y conexiones de red dificultando su detección por parte del **antivirus**.

Los **módulos firmados** sirven como defensa para que un atacante no cargue un **rootkit** en el **kernel**. Existe una medida de seguridad donde se requiere que los módulos estén `firmados criptográficamente` con un par de claves pública y privada. Los desarrolladores autorizados firman los módulos con la clave privada, y el kernel la verifica con la clave pública. De esta forma, si alguien intenta cargar un módulo malicioso, el kernel verificará la firma digital contra sus llaves de confianza. Si la firma del módulo no coincide o falta, el sistema rechaza la carga (insmod fallará), dificultando la carga de rootkits en kernel space.

Con **Secure Boot** activado:

1) El firmware UEFI verifica el bootloader
2) El bootloader verifica el kernel
3) El kernel verifica módulos

Eso crea una cadena de confianza (“chain of trust”).

## Desafío #2

### 1) ¿Qué funciones tiene disponible un programa y un módulo?

Un **programa** normal tiene a su disposición todas las librerías estándar del sistema, como por ejemplo, la librería estándar de C (libc). 

Un **módulo**, por su parte, no usa la librería estándar de C ni ninguna otra, sino que se comunica únicamente con las funciones internas expuestas por la API del núcleo de Linux.

### 2) Espacio de usuario o espacio del kernel.

El **espacio de usuario** es el entorno restringido donde corren las aplicaciones que no tienen acceso directo al hardware. 

El **espacio del kernel** es el corazón del SO con privilegios absolutos, es donde se gestionan la memoria, el procesador y el hardware físico; si un módulo de kernel tiene un error grave, puede corromper el sistema y provocar un cuelgue generalizado llamado `Kernel Panic`.

### 3) Espacio de datos.

En el **espacio de usuario**, un programa (gracias a la `memoria virtualizada`) opera como si tuviera un espacio de memoria exclusivo, y la memoria de un programa A está bien separada de la de un programa B. 

En el **espacio del kernel**, la memoria no se divide así, sino que todos los módulos comparten un único espacio de direcciones de memoria con el resto del núcleo, por lo tanto, un `puntero mal dirigido` de un módulo puede llegar a sobrescribir datos vitales de otro componente del SO.

### 4) Drivers. Investigar contenido de /dev.

Un **driver**, es un tipo específico de módulo de kernel diseñado para actuar como `traductor` entre el SO y el dispositivo físico.

El **directorio `/dev`** contiene archivos que son nodos especiales de dispositivos que sirven como interfaces de comunicación. El disco duro, por ejemplo, se representa como `/dev/sda`, y un programa de usuario puede pedirle al SO leer o escribir sobre ese dispositivo.

## Preparacion:

```
sudo apt-get update
```

Luego hacemos:

``` bash
sudo apt-get install build-essential checkinstall linux-source linux-headers-$(uname -r)
```
No fue necesario instalar el `kernel-package` ya que es un paquete obsoleto en varias distribuciones modernas.

Los paquetes instalados cumplen las siguientes funciones:

* **build-essential:** Instala las herramientas básicas para compilar código en C/C++ (como el compilador gcc y la herramienta make), indispensables para compilar tu módulo.

* **checkinstall:**  Sirve para crear paquetes instalables (como los .deb) a partir del código fuente.

* **kernel-package (aunque no se instalo):** Contiene utilidades que facilitan la compilación de kernels de Linux.

* **linux-source:** Proporciona el código fuente del kernel de Linux. 

* **linux-headers-$(uname -r):** Proporciona las cabeceras (`headers`) correspondientes a la versión actual del kernel, las cuales son necesarias para compilar módulos compatibles con el sistema en ejecución.

Ahora tenemos que crearnos una cuenta de Gitlab y además hacer un fork del repositorio: 
https://gitlab.com/sistemas-de-computacion-unc/kenel-modules.git

Una vez hecho el fork clonamos el repositorio.

Entramos desde la terminal y vamos hasta  el directorio `module/`
y ejecutamos por terminal `make`, para compilar con Makefile el módulo.

Entonces obtendremos un archivo llamado `mimodulo.ko` que corresponde al módulo compilado y puede cargarse dinámicamente en el kernel de Linux.

### Hacemos entonces: 

```
sudo insmod mimodulo.ko
```

Si usamos `sudo dmesg` observaremos los registros del kernel, y al final de este observaremos que el módulo fue insertado correctamente:

![insmod](https://hackmd.io/_uploads/SyZ6wj2yzx.png)


Y con el siguiente comando verificaremos que el módulo aparece listado entre los módulos cargados:

![grep](https://hackmd.io/_uploads/B1NWdo21Mg.png)


Ahora lo que haremos es quitar el módulo listado y usar una herramienta llamada ```modinfo``` para analizarlo:

Primero ejecutamos el comando para remover `rmmod`

```
sudo rmmod mimodulo
```

Revisamos los registros nuevamente con:

```
sudo dmesg 
```

Y observamos como fue descargado del kernel:

![rmmod](https://hackmd.io/_uploads/rkyDOinkGg.png)

Volvemos a listar los módulos cargados con: 
```
lsmod | grep mod
```

También puede verificarse esta información consultando /proc/modules mediante el siguiente comando:

```
cat /proc/modules | grep mod 
```

En ambos casos no apareció nada, poniendo en evidencia que el módulo fue descargado del kernel exitosamente.

![grepcat](https://hackmd.io/_uploads/BJOCOjnkzg.png)

Ahora para un análisis final usaremos los comandos **modinfo**:

Usando `modinfo mimodulo.ko`

![modinfomimodulo](https://hackmd.io/_uploads/r16sNin1zx.png)

Usando `modinfo /lib/modules/$(uname -r)/kernel/crypto/des_generic.ko.zst`

![modinfodesgeneric](https://hackmd.io/_uploads/SJ6hEih1fg.png)

### 1) ¿Qué diferencias se pueden observar entre los dos modinfo? 

El módulo oficial **des_generic** cuenta con campos criptográficos como: `sig_id`, `signer`, `sig_key`, `sig_hashalgo` y la `signature`. Fue **firmado durante la compilación oficial del kernel**, en cambio, el módulo **mimodulo.ko** `no tiene firma`. 

El módulo oficial **des_generic** posee la etiqueta `intree`, que significa que `pertenece al árbol oficial de código fuente del kernel`.

El módulo oficial contiene líneas de `alias`, esto le sirve al kernel para saber exactamente qué algoritmos o dispositivos de hardware deben "despertar" a este driver. Por otro lado, **mimodulo.ko** carece de alias.

El módulo oficial depende de otro módulo llamado `libdes`, nuestro módulo es autónomo. 

### 2) ¿Qué drivers/módulos están cargados en sus propias pc? Comparar las salidas con las computadoras de cada integrante del grupo. Expliquen las diferencias. Carguen un txt con la salida de cada integrante en el repo y pongan un diff en el informe.

Cada integrante debe ejecutar en su terminal (posicionado en el directorio **`lsmod`**):

```
lsmod > lsmod_apellido.txt
```

Para comprobar las diferencias de a pares:

```
diff -u lsmod_mohammad.txt lsmod_fernandez.txt
diff -u lsmod_mohammad.txt lsmod_laura_surco.txt
diff -u lsmod_fernandez.txt lsmod_laura_surco.txt
```

Para comprobar las diferencias de los tres archivos a la vez:

```
diff3 lsmod_mohammad.txt lsmod_fernandez.txt lsmod_laura_surco.txt
```
Al comparar las salidas de lsmod de los tres integrantes, se observa que los módulos cargados dependen del hardware y de los controladores presentes en cada computadora. Por ejemplo, cambian los módulos de red, audio, gráficos y dispositivos USB, ya que cada equipo tiene una combinación distinta de periféricos y firmware.

Las salidas y comparaciones entre la computadora de cada integrante del grupo estarán en la carpeta `lsmod/` del directorio raiz:

* [**`sudo-make-a-sandwich/lsmod/`**](https://github.com/Sachan-2512/Sudo-Make-Me-a-Sandwich-TP-s-SdC/tree/TP4/lsmod)

### 3) ¿Cuáles no están cargados pero están disponibles? ¿Qué pasa cuando el driver de un dispositivo no está disponible? 

Los controladores o módulos que están disponibles pero no cargados se encuentran almacenados físicamente en el disco duro, específicamente en la ruta `/lib/modules/$(uname -r)/kernel/`. En este estado, el sistema reconoce su existencia pero **no consumen memoria RAM**, ya que no han sido insertados en el núcleo debido a que el hardware asociado **no está conectado o en uso**.

Por otro lado, cuando el controlador de un dispositivo no está disponible, al conectar dicho hardware el kernel solo logra detectar su identificador (`Device ID`). Al no encontrar un módulo con el alias correspondiente para manejarlo, no se crea el nodo del dispositivo en `/dev`, y por lo tanto, el dispositivo queda completamente inutilizable.


### 4) Correr hwinfo en una pc real con hw real y agregar la url de la información de hw en el reporte. 

Para instalar la herramienta y generar el reporte abreviado:

```
sudo apt install hwinfo
hwinfo --short > informe_hardware.txt
```
Los reportes de los integrantes de grupos estarán en la carpeta `informes_hardware/` del directorio raíz:

* [**`sudo-make-a-sandwich/informes_hardware/`**](https://github.com/Sachan-2512/Sudo-Make-Me-a-Sandwich-TP-s-SdC/tree/TP4/informes_hardware)

### 5) ¿Qué diferencia existe entre un módulo y un programa  ? 

Un módulo no inicia en la función `main()`, sino que implementa **funciones de ciclo de vida como `init_module` y `exit_module`**. Asimismo, los módulos suelen ejecutarse en respuesta a **eventos del kernel o llamadas internas del sistema**. Un módulo no corre en espacio de usuario como un programa común, sino en **kernel space**. No usa la libc, sino que **usa APIs internas del kernel**. Un error corrompe la memoria del SO, provocando un `Kernel Panic`.

### 6) ¿Cómo puede ver una lista de las llamadas al sistema que realiza un simple helloworld en c?

Se utiliza la herramienta de rastreo `strace`. Si tienes tu ejecutable compilado, corres en la terminal:

```
strace ./hello_world
```

Esto interceptará y mostrará en pantalla cada interacción del programa con el kernel (como `execve`, `brk`, `openat`, y la llamada `write` para imprimir el texto en la pantalla).

### 7) ¿Qué es un segmentation fault? ¿Cómo lo maneja el kernel y cómo lo hace un programa?

Es una **excepción de hardware** generada por la Unidad de Manejo de Memoria `(MMU)` cuando un proceso intenta acceder a segmentos de memoria a los cuales no tiene permisos (por ejemplo, memoria que pertenece al kernel o a otro proceso).

**Manejo por parte del kernel:**
El procesador interrumpe la ejecución del programa y le cede el control al kernel. Este identifica qué proceso causó la infracción y le envía una señal de terminación, típicamente `SIGSEGV` (señal de violación de segmento).

**Manejo por parte de un programa:**
Por defecto, los programas no están diseñados para defenderse de este tipo de errores; al recibir la señal `SIGSEGV`, el proceso finaliza inmediatamente (crash). Sin embargo, un programador puede preparar el programa para capturar y manejar esta señal de forma controlada utilizando funciones como `signal()` o `sigaction()`.

### 8) ¿Se animan a intentar firmar un módulo de kernel? ¿Y documentar el proceso?  https://askubuntu.com/questions/770205/how-to-sign-kernel-modules-with-sign-file

Se modifica el `mimodulo.c` por: 

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

En primera instancia, se crea la carpeta `Claves`. Posteriormente, dentro de esta carpeta se generó un par de claves `X.509` utilizando `openssl`:

```
openssl req -new -x509 -newkey rsa:2048 \
-keyout MOK.priv \
-outform DER \
-out MOK.der \
-nodes \
-days 36500 \
-subj "/CN=Sudo-Make-A-Sandwich/"
```

Como resultado de esta operación se obtuvieron:

- `MOK.priv`: Clave privada utilizada para firmar el módulo.
- `MOK.der`: Certificado público asociado a dicha firma.

![Claves](https://hackmd.io/_uploads/HkOY93nyze.png)

Finalmente, por medio de la herramienta `sign-file`, que está incluida dentro de los headers del kernel de Linux, e indicando la ruta donde se encuentran las claves `MOK.priv` y `MOK.der` (en este caso `~/Documentos/TPN4-Computacion/Claves`), se procede a aplicar la firma criptográfica sobre `mimodulo.ko`. Esto se logra utilizando las siguientes líneas:

```
sudo /usr/src/linux-headers-$(uname -r)/scripts/sign-file \
sha256 \
~/Documentos/TPN4-Computacion/Claves/MOK.priv \
~/Documentos/TPN4-Computacion/Claves/MOK.der \
mimodulo.ko
```

A continuación, se muestra el antes y después de ejecutar el bloque anterior:

![firma](https://hackmd.io/_uploads/H1vr63nyMx.png)


### 9) Agregar evidencia de la compilación, carga y descarga de su propio módulo imprimiendo el nombre del equipo en los registros del kernel. 

**Compilación**

![Compilacion](https://hackmd.io/_uploads/ryb43on1fe.png)

**Carga y Descarga del módulo**

Para acotar el log del kernel, se utilizará el siguiente comando:

```
sudo dmesg | tail -n 3
```

Luego, se siguen los mismos pasos que para el módulo del repositorio clonado.

![cargadescarga](https://hackmd.io/_uploads/rkFvnjn1ze.png)


### 10) ¿Qué pasa si mi compañero con Secure Boot habilitado intenta cargar un módulo firmado por mi? 

El intento fallará y la terminal devolverá un error de operación no permitida. Aunque el módulo esté firmado, la clave utilizada para firmarlo no pertenece al conjunto de claves confiables registradas en el sistema de la otra computadora. El módulo se puede cargar siempre y cuando mi compañero cargue mi certificado público dentro del gestor MOK de su propia máquina.


### 11) Dada la siguiente nota https://arstechnica.com/security/2024/08/a-patch-microsoft-spent-2-years-preparing-is-making-a-mess-for-some-linux-users/ 

* ¿Cuál fue la consecuencia principal del parche de Microsoft sobre GRUB en sistemas con arranque dual (Linux y Windows)?

> El parche aplicó una actualización de políticas `SBAT` (Secure Boot Advanced Targeting) diseñada para bloquear `versiones antiguas y vulnerables` del cargador de arranque GRUB. Sin embargo, debido a un error de implementación, el parche bloqueó también las `versiones legítimas y actualizadas` de `GRUB` de muchas distribuciones de `Linux` (como Ubuntu, Mint, Debian). Esto causó que los sistemas con Dual Boot no pudieran iniciar Linux, mostrando pantallas de error de violación de seguridad.

* ¿Qué implicancia tiene desactivar Secure Boot como solución al problema descrito en el artículo?

> Desactivar `Secure Boot` permite el arranque inmediato de GRUB y de `Linux` saltándose el `bloqueo de Microsoft`. Sin embargo, el efecto negativo que trajo consigo es que se elimina por completo la barrera de protección del firmware en el inicio. Esto deja expuesta a la computadora ante ataques de malware avanzados que se inyectan antes de que cargue el sistema operativo (como bootkits o rootkits).

* ¿Cuál es el propósito principal del Secure Boot en el proceso de arranque de un sistema?

> Su propósito es garantizar que la computadora inicie utilizando únicamente software confiable. Para ello, verifica las firmas digitales criptográficas de cada pieza de código que se ejecuta en el arranque (el firmware `UEFI`, el cargador de arranque `(GRUB)`, el `kernel` y los `módulos críticos`). Si alguna pieza no está firmada o su firma fue alterada, el sistema detiene el arranque para prevenir la ejecución de código malicioso.

### Conclusiones

El desarrollo de este trabajo permitió comprender el funcionamiento interno de los módulos de kernel y su interacción con el SO Linux. Además de analizar herramientas de compilación y administración de módulos, se investigaron mecanismos modernos de seguridad como Secure Boot y la firma criptográfica, permitiendo entender su importancia para proteger la integridad del kernel frente a software malicioso o módulos no confiables.

Asimismo, la práctica permitió reforzar la diferencia entre el espacio de usuario y el espacio del kernel, y la relevancia de controlar qué código puede ejecutarse con privilegios elevados.