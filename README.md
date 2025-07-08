# Búsqueda de Patrones en Múltiples Documentos

- Este proyecto implementa varias soluciones para **búsqueda de patrones en textos**, adaptadas al escenario de **búsqueda en múltiples documentos** concatenados en un solo texto.

### Descripción del proyecto

- El sistema permite buscar patrones en un conjunto de documentos de texto y obtener las posiciones o documentos donde aparecen.
Para ello, concatena todos los documentos en un solo texto (separados por `$`), construye las estructuras necesarias y aplica los algoritmos sobre esa cadena.

### Soluciones algorítmicas
- **Boyer Moore**
- **Knuth-Morris-Pratt (KMP)**
- **Rabin-Karp**

### Solucion basada en estructuras de datos
- **Suffix Array**

### Archivos a buscar
- Los **archivos de texto** a analizar deben estar dentro de la carpeta `documentos/`.
- El programa recorrerá todos los archivos dentro de `documentos/` y buscará el patrón exacto ingresado.

### Requisitos
- Sistema operativo: Linux/Windows
- Compilador: g++ con soporte C++17
- Recomendado utilizar MSYS2

### Para compilar

- Llegar a "cd /Semestral_ED/" y hacer:

```bash
g++ -std=c++17 -g main.cpp -o main.exe
```
```bash
./main.exe
```

### Referencias
Boyer Moore: https://www.youtube.com/watch?v=3Ft3HMizsCk - https://www.youtube.com/watch?v=Tbj8iH9UkSA

Knuth-Morris-Pratt: https://www.youtube.com/watch?v=ynv7bbcSLKE

Rabin-Karp: https://www.youtube.com/watch?v=yFHV7weZ_as