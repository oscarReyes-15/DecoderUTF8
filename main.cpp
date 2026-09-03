#include <cstdint>
#include <fstream>
#include <iostream>
#include <vector>

#include "Decoder.h"

int main(int argc, char* argv[]) {
    // Param check
    if (argc < 2) {
        std::cerr << "Error: falta la ruta del archivo\n";
        return 1;
    }

    // Open at the end to obtain the file size
    std::ifstream txtFile(argv[1], std::ios::binary | std::ios::ate);

    if (!txtFile) {
        std::cerr << "Error: no se pudo abrir el archivo\n";
        return 1;
    }

    //  file size
    std::streamsize size = txtFile.tellg();

    if (size < 0) {
        std::cerr << "Error: no se pudo determinar el tamaño\n";
        return 1;
    }

    // Return to the beginning
    txtFile.seekg(0, std::ios::beg);

    if (!txtFile) {
        std::cerr << "Error: no se pudo posicionar el archivo\n";
        return 1;
    }

    // Raw UTF-8 byte buffer
    std::vector<std::uint8_t> buffer(static_cast<std::size_t>(size));

    // Avoid reading when file is empty
    if (size > 0) {
        if (!txtFile.read(reinterpret_cast<char*>(buffer.data()), size)) {
            std::cerr << "Error: no se pudo leer el archivo\n";
            return 1;
        }
    }

    Decoder decoderUTF8;
    decoderUTF8.leerFileEnBufferDeBytes(buffer);


    return 0;
}