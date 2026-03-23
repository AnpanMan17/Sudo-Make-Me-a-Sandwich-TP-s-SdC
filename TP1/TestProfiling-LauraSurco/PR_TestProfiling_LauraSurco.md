# 🚀 PR: Profiling del benchmark en C con gprof y perf (LauraSurco)

## 🧩 Resumen
Esta PR consolida el trabajo de medición de rendimiento del benchmark en C del TP1, incorporando evidencia reproducible con `gprof` y `perf`.
El análisis confirma que la mayor carga computacional está concentrada en `func1` y `func2`, y deja trazabilidad completa de compilación, ejecución y reporte.

## 🎯 Objetivo
- Medir el costo por función del binario `test_gprof`.
- Validar consistencia de resultados entre `gprof` (instrumentación) y `perf` (sampling por ciclos).
- Documentar evidencia para revisión académica y técnica.

## 📦 Alcance de cambios
- Estructura de benchmark separada en dos unidades de compilación:
  - `test_gprof.c`
  - `test_gprof_new.c`
- Artefactos de profiling y ejecución:
  - `analysis.txt`
  - `TerminalOutput.log`
  - `gmon.out`
  - `perf.data`
  - `perf.data.old`
  - `test_gprof`

## 📊 Resultados de rendimiento

### 1) gprof (flat profile)
Fuente: `TerminalOutput.log`

- `func1`: **55.98%** (8.05s self, 8.55s total)
- `func2`: **37.00%** (5.32s self)
- `main`: **3.55%** (0.51s self)
- `new_func1`: **3.48%** (0.50s self)
- Tiempo total muestreado: **14.38s**

### 2) perf (cycles)
Fuente: `TerminalOutput.log`

- Samples: **66K** del evento `cycles:P`
- Event count aproximado: **51.225.981.329**
- `func1`: **52.09%**
- `func2`: **41.58%**
- `main`: **2.93%**
- `new_func1`: **2.72%**
- Overhead restante: kernel y ruido de sistema en porcentaje marginal

### 3) Conclusión técnica
Ambas herramientas coinciden en el patrón de consumo: el cuello de botella está en `func1` y `func2`.
La correlación entre `gprof` y `perf` respalda que la distribución del costo es estable y que las funciones de soporte (`main` y `new_func1`) tienen impacto bajo en el tiempo total.

## 🧪 Reproducción local
1. Compilación instrumentada:

   ```bash
   gcc -Wall -pg test_gprof.c test_gprof_new.c -o test_gprof
   ```

2. Ejecución:

   ```bash
   ./test_gprof
   ```

3. Reporte gprof:

   ```bash
   gprof test_gprof gmon.out > analysis.txt
   ```

4. Captura perf:

   ```bash
   sudo perf record ./test_gprof
   ```

5. Reporte perf:

   ```bash
   perf report
   ```

## ⚠️ Nota importante sobre perf (requisito de entorno)
Antes de ejecutar `perf`, verificar si está instalado en la máquina.
Si no está disponible, instalarlo con:

```bash
sudo apt update && apt install linux-tools-common
```

## ✅ Checklist de validación
- [x] Compila con flag `-pg`
- [x] Ejecuta binario sin errores
- [x] Genera `gmon.out` y `analysis.txt`
- [x] Captura profiling con `perf record`
- [x] Obtiene reporte de `perf` con funciones del binario
- [x] Evidencia completa registrada en `TerminalOutput.log`
