#include <efi.h>
#include <efilib.h>

EFI_STATUS efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable) {
    
    InitializeLib(ImageHandle, SystemTable);
    
    // Print() de efilib.h maneja internamente la traduccion de la ABI (Linux a Windows)
    Print(L"Iniciando analisis de seguridad...\r\n");

    // Inyección de un software breakpoint (INT3)
    unsigned char code[] = { 0xCC };

    if (code[0] == 0xCC) {
        Print(L"Breakpoint estatico alcanzado.\r\n");
    }
    
    // Y le agregamos la gran idea del retardo para que la pantalla no se limpie al instante
    uefi_call_wrapper(SystemTable->BootServices->Stall, 1, 3000000);

    return EFI_SUCCESS;
}