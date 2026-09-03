Decodificador manual de UTF-8

Programa de línea de comandos escrito en C++20 que lee un archivo en modo binario y decodifica manualmente secuencias UTF-8. El programa reporta los code points válidos, los errores encontrados y un resumen por longitud de codificación.

Compilación

Desde la carpeta que contiene main.cpp, Decoder.cpp y Decoder.h:

Windows con PowerShell

g++ -std=c++20 main.cpp Decoder.cpp -o decoder_utf8.exe

Linux o macOS

g++ -std=c++20 main.cpp Decoder.cpp -o decoder_utf8

Linux

./decoder_utf8 Test.txt

También se puede proporcionar una ruta completa o una ruta que contenga espacios:

.\decoder_utf8.exe "C:\ruta con espacios\archivo.txt"