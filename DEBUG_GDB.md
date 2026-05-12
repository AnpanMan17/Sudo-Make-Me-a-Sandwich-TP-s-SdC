# 4 Depuración con gdb

## 1. Depuración con GDB + QEMU

### Iniciar QEMU con el servidor GDB habilitado
```bash
qemu-system-x86_64 -m 512 \
-bios /usr/share/ovmf/OVMF.fd \
-net none \
-drive file=fat:rw:$HOME/uefi_security_lab,format=raw \
-s \
-S
```

> `-s` es shorthand de `-gdb tcp::1234` (abre el servidor GDB en localhost:1234) \
> `-S` hace que QEMU no arranque hasta que GDB le diga continue (pausa la CPU al inicio y espera que GDB conecte antes de ejecutar)

### Conectar GDB
En otra terminal:
```bash
# Necesitás gdb con soporte para EFI, el normal sirve
gdb
```

Dentro de GDB:
```gdb
# Conectar al servidor de QEMU
(gdb) target remote localhost:1234

# El firmware arranca en modo 16-bit real, decirle a GDB la arquitectura
(gdb) set architecture i8086

# Dejar que el firmware inicialice hasta llegar a la UEFI Shell
(gdb) continue
```

### El problema: encontrar la dirección base de tu .efi
UEFI carga las imágenes en direcciones dinámicas (no fijas), entonces necesitás saber dónde en memoria cargó tu `aplicacion.efi`.

Desde la UEFI Shell, antes de ejecutar tu app, corré:
```text
# Esto muestra las imágenes cargadas con sus direcciones base
FS0:\> loadedimage
```
O podés usar `dh -d` sobre el handle de tu aplicación después de cargarla.

**Alternativa más práctica**: agregar un `for (;;) {}` al inicio de tu app para que se quede esperando, y mientras tanto leer la dirección desde GDB.
```c
EFI_STATUS EFIAPI efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable) {
    volatile int waiting = 1;
    while (waiting) {} // GDB conecta acá y cambia waiting a 0
    // ... resto del código
}
```

### Cargar los símbolos en GDB
Una vez que tenés la dirección base (por ejemplo `0x6500000`):
```bash
# En tu máquina, generá el .so con símbolos (ya lo tenés del paso de compilación)
# Necesitás calcular el offset de la sección .text
objdump -h aplicacion.so | grep .text
# Fijate el offset, por ejemplo: 0x240
```

En GDB, cargá los símbolos con la dirección base + offset de `.text`:
```gdb
(gdb) add-symbol-file /home/tuusuario/uefi_security_lab/aplicacion.so 0x6500240

# Ahora podés poner breakpoints por nombre de función
(gdb) break efi_main

# Ver registros
(gdb) info registers

# Paso a paso
(gdb) stepi # una instrucción de assembly
(gdb) nexti # siguiente instrucción (sin entrar a funciones)

# Ver el 0xCC en memoria
(gdb) x/bx &code
```

### Ver el breakpoint 0xCC con GDB
Cuando la ejecución llegue al `if (code[0] == 0xCC)`:
```gdb
# Examinar memoria donde está el byte
(gdb) x/1bx $rbp-0x1
# Debería mostrar:
# 0x...: 0xcc

# Ver el estado de los registros en ese momento
(gdb) info registers rax rbp rsp

# Ver el assembly alrededor del PC actual
(gdb) x/10i $rip
```

---

## 2. Conectar Ghidra a GDB (análisis híbrido)
Ghidra tiene un debugger integrado desde la versión 10+ que puede conectarse a QEMU vía GDB remote.

### Configurar en Ghidra
1. Abrí tu proyecto con `aplicacion.efi` ya importado y analizado
2. Menú: `Debugger → Debug aplicacion.efi`
3. En el diálogo que aparece, elegí:
   `"Remote GDB"` (o `"gdb via ssh"` según versión)
4. Configuración:
   - Host: `localhost`
   - Port: `1234`
5. Click en `"Connect"`

### Lo que podés hacer con el Debugger de Ghidra
Una vez conectado, Ghidra sincroniza el análisis estático con la ejecución en vivo:
- El cursor en el listing sigue el RIP (instruction pointer) en tiempo real
- Podés poner breakpoints haciendo click en el margen izquierdo del listing
- Panel **Registers**: ves todos los registros actualizados en vivo
- Panel **Memory**: podés inspeccionar cualquier dirección de memoria
- Panel **Stack**: ver el stack frame actual

### Flujo recomendado
1. Iniciá QEMU con `-s -S` → queda pausado
2. Abrí Ghidra con el `.efi` importado
3. Conectá Ghidra al GDB de QEMU
4. En la UEFI Shell ejecutá `aplicacion.efi`
   (QEMU la carga pero Ghidra la para en el entry point)
5. En Ghidra, navegá a `efi_main` en el listing y ponés un breakpoint en la línea del `0xCC`
6. Le das Continue → la ejecución para justo ahí
7. Inspeccionás registros, memoria y stack mientras el pseudocódigo de la derecha te muestra dónde estás parado a nivel C

**Lo más interesante** sería poner un breakpoint justo en la instrucción `CMP AL, 0xCC` y mostrar en un screenshot que GDB/Ghidra para la ejecución ahí, con el registro `AL` mostrando `0xCC`.