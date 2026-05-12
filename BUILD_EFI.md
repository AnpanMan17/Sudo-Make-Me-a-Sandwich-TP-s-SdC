## 2.2 Compilación a Formato PE/COFF

UEFI utiliza el formato PE/COFF (típico de Windows), incluso si compilamos desde Linux.

**Comandos:**

```bash
# 1. Compilar a código objeto
gcc -I/usr/include/efi -I/usr/include/efi/x86_64 -I/usr/include/efi/protocol -fpic -ffreestanding -fno-stack-protector -fno-strict-aliasing -fshort-wchar -mno-red-zone -maccumulate-outgoing-args -Wall -c -o aplicacion.o aplicacion.c

# 2. Linkear (generar .so intermedio)
ld -shared -Bsymbolic -L/usr/lib -L/usr/lib/efi -T /usr/lib/elf_x86_64_efi.lds /usr/lib/crt0-efi-x86_64.o aplicacion.o -o aplicacion.so -lefi -lgnuefi

# 3. Convertir a ejecutable EFI (PE/COFF)
objcopy -j .text -j .sdata -j .data -j .dynamic -j .dynsym -j .rel -j .rela -j .rel.* -j .rela.* -j .reloc --target=efi-app-x86_64 aplicacion.so aplicacion.efi
```

## 2.3 Análisis de Metadatos y Decompilación

**Comandos:**

```bash
file aplicacion.efi
readelf -h aplicacion.efi
ghidra
```

*(Importa aplicacion.efi en Ghidra y analiza la función efi_main).*