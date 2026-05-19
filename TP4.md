



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


modinfo bluetooth
filename:       /lib/modules/6.8.0-117-generic/kernel/net/bluetooth/bluetooth.ko.zst
alias:          net-pf-31
license:        GPL
version:        2.22
description:    Bluetooth Core ver 2.22
author:         Marcel Holtmann <marcel@holtmann.org>
srcversion:     FB30BC48CE4BEBDBDC22082
depends:        ecdh_generic
retpoline:      Y
intree:         Y
name:           bluetooth
vermagic:       6.8.0-117-generic SMP preempt mod_unload modversions 
sig_id:         PKCS#7
signer:         Build time autogenerated kernel key
sig_key:        6D:04:C4:BF:F4:02:05:5D:BD:2A:6A:72:72:2B:44:11:FF:31:09:80
sig_hashalgo:   sha512
signature:      35:34:C2:26:1E:41:AA:F4:D1:BD:2F:11:13:3C:5D:21:18:6B:7F:C0:
                E4:AE:A1:4B:A7:80:3E:C7:49:9B:26:81:7C:05:FF:7C:9F:D3:75:11:
                7C:1F:98:B1:43:74:4E:B7:4E:BF:DB:E3:7C:AF:8F:38:69:BD:9A:0A:
                9D:E0:EF:85:0B:09:02:78:29:93:B3:3D:23:0B:ED:0A:24:98:05:D6:
                1D:2B:02:D7:13:29:04:00:1A:E5:5B:A3:E9:11:66:E0:B4:7A:91:73:
                84:CD:B9:D9:C8:E0:E0:86:B3:7C:03:1A:77:71:95:BB:C8:B2:85:31:
                44:27:7C:42:5F:02:11:DA:D1:F8:4B:CA:A4:45:7F:4C:D4:B5:E7:1F:
                7F:2B:CE:4E:53:AD:8D:37:6F:80:61:0E:6C:68:3E:2F:07:D0:E2:AD:
                71:57:E2:F9:7F:80:A1:5C:82:2C:56:CB:22:E8:A8:25:B2:1F:B7:C9:
                2A:DE:13:F2:B2:41:2A:F4:D2:3C:1B:B4:70:A5:CF:42:E8:CB:49:CB:
                EA:EF:A9:95:38:1D:22:BD:8F:B8:97:21:9A:57:AA:14:DB:E6:6E:DD:
                DB:AB:F6:0A:F5:56:09:18:63:56:60:2F:F4:43:EB:58:75:2D:8E:4B:
                A9:26:97:98:1F:6F:AC:C7:D9:7B:5B:D8:AD:9A:39:65:F6:FC:5A:79:
                17:88:F5:04:81:DF:5F:FF:21:9C:B6:DB:21:DA:21:2F:9D:9D:C2:7B:
                C1:D9:64:EC:83:83:AD:37:72:DD:5E:01:AE:93:01:6B:53:8D:39:A7:
                A3:31:1B:52:DF:EB:33:1F:78:B0:12:38:81:A8:7E:B6:58:57:EB:C0:
                F4:ED:1A:9D:4A:2A:61:F8:52:BF:5C:50:72:DD:5C:B5:E6:F0:E2:9C:
                6C:81:C6:30:76:6D:4E:AC:39:15:53:A2:A6:96:F6:BF:B4:CA:B0:A1:
                4E:A6:1D:CD:2E:CA:FC:DB:A8:EA:FB:D0:52:34:19:46:48:BF:1B:1E:
                E5:40:32:F8:16:C9:EB:F6:C2:40:1B:BD:3E:44:C4:C0:68:4D:A6:07:
                12:74:CE:50:49:77:4E:67:3E:67:7A:E0:E4:2C:4D:C1:89:03:AA:8E:
                FE:DE:82:C1:72:6A:22:80:8E:C6:C8:53:96:D1:32:72:47:CC:79:D4:
                F3:9B:7F:B8:4B:D3:86:C7:D2:50:A5:8B:4F:5B:5D:72:2B:2A:49:EE:
                E8:52:C6:26:F5:9A:B1:12:1D:16:3A:E3:40:63:57:37:E0:C0:B2:92:
                32:12:89:B9:F9:65:02:32:89:23:AD:A1:4D:B7:0E:62:D2:18:34:F1:
                39:5C:9C:C4:E8:39:5D:1E:BD:1B:D9:44
parm:           disable_esco:Disable eSCO connection creation (bool)
parm:           disable_ertm:Disable enhanced retransmission mode (bool)
parm:           enable_ecred:Enable enhanced credit flow control mode (bool)
saqib@saqibdaniel:~/Desktop/Sudo-Make-Me-a-Sandwich-TP-s-SdC/kenel-modules/part1/module$ modinfo e1000e
filename:       /lib/modules/6.8.0-117-generic/kernel/drivers/net/ethernet/intel/e1000e/e1000e.ko.zst
license:        GPL v2
description:    Intel(R) PRO/1000 Network Driver
author:         Intel Corporation, <linux.nics@intel.com>
srcversion:     7C7459544428F81F9F12C1D
alias:          pci:v00008086d000057BAsv*sd*bc*sc*i*
alias:          pci:v00008086d000057B9sv*sd*bc*sc*i*
alias:          pci:v00008086d000057B8sv*sd*bc*sc*i*
alias:          pci:v00008086d000057B7sv*sd*bc*sc*i*
alias:          pci:v00008086d000057B6sv*sd*bc*sc*i*
alias:          pci:v00008086d000057B5sv*sd*bc*sc*i*
alias:          pci:v00008086d000057B4sv*sd*bc*sc*i*
alias:          pci:v00008086d000057B3sv*sd*bc*sc*i*
alias:          pci:v00008086d000057A1sv*sd*bc*sc*i*
alias:          pci:v00008086d000057A0sv*sd*bc*sc*i*
alias:          pci:v00008086d00005511sv*sd*bc*sc*i*
alias:          pci:v00008086d00005510sv*sd*bc*sc*i*
alias:          pci:v00008086d0000550Fsv*sd*bc*sc*i*
alias:          pci:v00008086d0000550Esv*sd*bc*sc*i*
alias:          pci:v00008086d0000550Bsv*sd*bc*sc*i*
alias:          pci:v00008086d0000550Asv*sd*bc*sc*i*
alias:          pci:v00008086d0000550Dsv*sd*bc*sc*i*
alias:          pci:v00008086d0000550Csv*sd*bc*sc*i*
alias:          pci:v00008086d00000DC8sv*sd*bc*sc*i*
alias:          pci:v00008086d00000DC7sv*sd*bc*sc*i*
alias:          pci:v00008086d00001A1Dsv*sd*bc*sc*i*
alias:          pci:v00008086d00001A1Csv*sd*bc*sc*i*
alias:          pci:v00008086d00001A1Fsv*sd*bc*sc*i*
alias:          pci:v00008086d00001A1Esv*sd*bc*sc*i*
alias:          pci:v00008086d00000DC6sv*sd*bc*sc*i*
alias:          pci:v00008086d00000DC5sv*sd*bc*sc*i*
alias:          pci:v00008086d000015F5sv*sd*bc*sc*i*
alias:          pci:v00008086d000015F4sv*sd*bc*sc*i*
alias:          pci:v00008086d000015FAsv*sd*bc*sc*i*
alias:          pci:v00008086d000015F9sv*sd*bc*sc*i*
alias:          pci:v00008086d000015FCsv*sd*bc*sc*i*
alias:          pci:v00008086d000015FBsv*sd*bc*sc*i*
alias:          pci:v00008086d00000D55sv*sd*bc*sc*i*
alias:          pci:v00008086d00000D53sv*sd*bc*sc*i*
alias:          pci:v00008086d00000D4Dsv*sd*bc*sc*i*
alias:          pci:v00008086d00000D4Csv*sd*bc*sc*i*
alias:          pci:v00008086d00000D4Fsv*sd*bc*sc*i*
alias:          pci:v00008086d00000D4Esv*sd*bc*sc*i*
alias:          pci:v00008086d000015E2sv*sd*bc*sc*i*
alias:          pci:v00008086d000015E1sv*sd*bc*sc*i*
alias:          pci:v00008086d000015E0sv*sd*bc*sc*i*
alias:          pci:v00008086d000015DFsv*sd*bc*sc*i*
alias:          pci:v00008086d000015BCsv*sd*bc*sc*i*
alias:          pci:v00008086d000015BBsv*sd*bc*sc*i*
alias:          pci:v00008086d000015BEsv*sd*bc*sc*i*
alias:          pci:v00008086d000015BDsv*sd*bc*sc*i*
alias:          pci:v00008086d000015D6sv*sd*bc*sc*i*
alias:          pci:v00008086d000015E3sv*sd*bc*sc*i*
alias:          pci:v00008086d000015D8sv*sd*bc*sc*i*
alias:          pci:v00008086d000015D7sv*sd*bc*sc*i*
alias:          pci:v00008086d000015B9sv*sd*bc*sc*i*
alias:          pci:v00008086d000015B8sv*sd*bc*sc*i*
alias:          pci:v00008086d000015B7sv*sd*bc*sc*i*
alias:          pci:v00008086d00001570sv*sd*bc*sc*i*
alias:          pci:v00008086d0000156Fsv*sd*bc*sc*i*
alias:          pci:v00008086d000015A3sv*sd*bc*sc*i*
alias:          pci:v00008086d000015A2sv*sd*bc*sc*i*
alias:          pci:v00008086d000015A1sv*sd*bc*sc*i*
alias:          pci:v00008086d000015A0sv*sd*bc*sc*i*
alias:          pci:v00008086d00001559sv*sd*bc*sc*i*
alias:          pci:v00008086d0000155Asv*sd*bc*sc*i*
alias:          pci:v00008086d0000153Bsv*sd*bc*sc*i*
alias:          pci:v00008086d0000153Asv*sd*bc*sc*i*
alias:          pci:v00008086d00001503sv*sd*bc*sc*i*
alias:          pci:v00008086d00001502sv*sd*bc*sc*i*
alias:          pci:v00008086d000010F0sv*sd*bc*sc*i*
alias:          pci:v00008086d000010EFsv*sd*bc*sc*i*
alias:          pci:v00008086d000010EBsv*sd*bc*sc*i*
alias:          pci:v00008086d000010EAsv*sd*bc*sc*i*
alias:          pci:v00008086d00001525sv*sd*bc*sc*i*
alias:          pci:v00008086d000010DFsv*sd*bc*sc*i*
alias:          pci:v00008086d000010DEsv*sd*bc*sc*i*
alias:          pci:v00008086d000010CEsv*sd*bc*sc*i*
alias:          pci:v00008086d000010CDsv*sd*bc*sc*i*
alias:          pci:v00008086d000010CCsv*sd*bc*sc*i*
alias:          pci:v00008086d000010CBsv*sd*bc*sc*i*
alias:          pci:v00008086d000010F5sv*sd*bc*sc*i*
alias:          pci:v00008086d000010BFsv*sd*bc*sc*i*
alias:          pci:v00008086d000010E5sv*sd*bc*sc*i*
alias:          pci:v00008086d0000294Csv*sd*bc*sc*i*
alias:          pci:v00008086d000010BDsv*sd*bc*sc*i*
alias:          pci:v00008086d000010C3sv*sd*bc*sc*i*
alias:          pci:v00008086d000010C2sv*sd*bc*sc*i*
alias:          pci:v00008086d000010C0sv*sd*bc*sc*i*
alias:          pci:v00008086d00001501sv*sd*bc*sc*i*
alias:          pci:v00008086d00001049sv*sd*bc*sc*i*
alias:          pci:v00008086d0000104Dsv*sd*bc*sc*i*
alias:          pci:v00008086d0000104Bsv*sd*bc*sc*i*
alias:          pci:v00008086d0000104Asv*sd*bc*sc*i*
alias:          pci:v00008086d000010C4sv*sd*bc*sc*i*
alias:          pci:v00008086d000010C5sv*sd*bc*sc*i*
alias:          pci:v00008086d0000104Csv*sd*bc*sc*i*
alias:          pci:v00008086d000010BBsv*sd*bc*sc*i*
alias:          pci:v00008086d00001098sv*sd*bc*sc*i*
alias:          pci:v00008086d000010BAsv*sd*bc*sc*i*
alias:          pci:v00008086d00001096sv*sd*bc*sc*i*
alias:          pci:v00008086d0000150Csv*sd*bc*sc*i*
alias:          pci:v00008086d000010F6sv*sd*bc*sc*i*
alias:          pci:v00008086d000010D3sv*sd*bc*sc*i*
alias:          pci:v00008086d0000109Asv*sd*bc*sc*i*
alias:          pci:v00008086d0000108Csv*sd*bc*sc*i*
alias:          pci:v00008086d0000108Bsv*sd*bc*sc*i*
alias:          pci:v00008086d0000107Fsv*sd*bc*sc*i*
alias:          pci:v00008086d0000107Esv*sd*bc*sc*i*
alias:          pci:v00008086d0000107Dsv*sd*bc*sc*i*
alias:          pci:v00008086d000010B9sv*sd*bc*sc*i*
alias:          pci:v00008086d000010D5sv*sd*bc*sc*i*
alias:          pci:v00008086d000010DAsv*sd*bc*sc*i*
alias:          pci:v00008086d000010D9sv*sd*bc*sc*i*
alias:          pci:v00008086d00001060sv*sd*bc*sc*i*
alias:          pci:v00008086d000010A5sv*sd*bc*sc*i*
alias:          pci:v00008086d000010BCsv*sd*bc*sc*i*
alias:          pci:v00008086d000010A4sv*sd*bc*sc*i*
alias:          pci:v00008086d0000105Fsv*sd*bc*sc*i*
alias:          pci:v00008086d0000105Esv*sd*bc*sc*i*
depends:        
retpoline:      Y
intree:         Y
name:           e1000e
vermagic:       6.8.0-117-generic SMP preempt mod_unload modversions 
sig_id:         PKCS#7
signer:         Build time autogenerated kernel key
sig_key:        6D:04:C4:BF:F4:02:05:5D:BD:2A:6A:72:72:2B:44:11:FF:31:09:80
sig_hashalgo:   sha512
signature:      40:20:D8:A2:78:88:53:40:5E:A0:E1:75:77:98:C8:6B:46:5E:A2:41:
                2C:A8:86:23:0E:9C:5B:EB:62:22:AA:75:BB:BC:73:06:E1:87:13:B2:
                86:32:98:0F:A4:F8:5D:64:73:50:14:B3:2D:80:EC:E3:45:CB:57:21:
                BF:D6:04:5B:70:97:FF:EB:7B:28:DF:48:E3:CD:B1:17:34:74:A7:64:
                CB:0E:E7:97:FC:D9:DD:BA:39:AB:17:FB:42:A6:4E:45:24:1D:84:BA:
                80:A6:F0:E4:E8:86:B5:0C:9B:3E:FD:4C:FC:29:93:AF:17:2C:04:9A:
                33:8B:8C:A1:40:FA:B1:36:7D:C3:F2:A7:16:E8:D7:FB:9B:2D:81:E4:
                82:00:13:02:5D:95:8F:16:46:2F:B8:7B:79:43:41:E4:F6:09:82:2F:
                52:6C:33:EF:6A:1D:51:62:0D:9B:58:E4:35:A5:A0:02:A9:DC:5E:1A:
                50:6E:68:46:D3:66:84:9C:26:13:E3:50:3E:1F:07:FA:64:1C:FB:84:
                A3:70:19:31:73:6E:90:F2:48:6F:0D:B1:F3:77:F0:CC:4A:14:9C:8A:
                34:70:25:AD:99:C3:72:60:8F:37:E1:63:AE:D5:84:D1:88:C0:FA:FE:
                B9:C4:11:22:F2:CB:F0:A9:F3:81:A8:8B:1F:99:92:A6:85:D7:C2:07:
                65:BA:6D:71:EF:0E:18:CE:36:CA:DD:D8:51:2B:28:3C:21:A1:03:AF:
                02:E6:46:CB:67:1C:10:89:79:2E:B9:CE:30:58:67:0D:96:BF:C8:C1:
                F2:D4:6F:07:DC:3D:79:11:20:FC:79:85:1C:0A:01:A0:08:28:D7:E4:
                27:5E:FF:00:94:65:1B:3D:61:77:C5:FE:25:87:1A:CC:9A:E9:C3:28:
                94:CB:53:56:A9:68:78:9A:B3:3B:21:68:FB:28:F9:D0:9B:66:53:E8:
                80:50:FC:F3:B1:C4:B2:05:42:81:78:12:6C:DB:4B:AB:A7:34:3C:55:
                CB:73:FB:02:73:C9:F6:9E:5E:EB:D5:6D:E8:D2:81:00:27:53:32:27:
                F8:2F:F4:C2:AE:08:B2:DF:D3:F9:C4:37:41:66:1D:66:87:4A:CB:CC:
                54:7E:A6:A7:22:A9:2B:DB:C8:D8:ED:76:E7:E0:79:22:24:59:8D:1F:
                B1:70:F2:8B:FB:A8:60:C9:90:E7:BA:EB:DD:44:08:F6:1A:3E:51:11:
                C0:FB:E8:3F:64:8B:E5:A6:4B:D0:03:04:E6:86:4F:DB:96:9E:47:CD:
                FC:E9:98:2A:B0:6D:05:8F:54:9E:C8:86:0A:C5:AE:F2:ED:80:B2:7E:
                5B:51:90:D6:E3:32:D9:F2:C7:53:CA:B2
parm:           debug:Debug level (0=none,...,16=all) (int)
parm:           copybreak:Maximum size of packet that is copied to a new buffer on receive (uint)
parm:           TxIntDelay:Transmit Interrupt Delay (array of int)
parm:           TxAbsIntDelay:Transmit Absolute Interrupt Delay (array of int)
parm:           RxIntDelay:Receive Interrupt Delay (array of int)
parm:           RxAbsIntDelay:Receive Absolute Interrupt Delay (array of int)
parm:           InterruptThrottleRate:Interrupt Throttling Rate (array of int)
parm:           IntMode:Interrupt Mode (array of int)
parm:           SmartPowerDownEnable:Enable PHY smart power down (array of int)
parm:           KumeranLockLoss:Enable Kumeran lock loss workaround (array of int)
parm:           WriteProtectNVM:Write-protect NVM [WARNING: disabling this can lead to corrupted NVM] (array of int)
parm:           CrcStripping:Enable CRC Stripping, disable if your BMC needs the CRC (array of int)




