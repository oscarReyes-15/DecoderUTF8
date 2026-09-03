#include "Decoder.h"
#include <format>


Decoder::Decoder() {
}

bool Decoder::codePointBiggerThanLength (int length, int offset) {
    return offset  >= length;
}

void Decoder::leerFileEnBufferDeBytes(const std::vector<uint8_t>& buffer) {
    int offset = 0;
    int len = buffer.size();

    if (len >= 3 && buffer[0] == 0xEF && buffer[1] == 0xBB && buffer[2] == 0xBF){
        offset = 3;  // skip BOM
    }

    while (offset < len){
        // Primer Byte
        uint32_t b1 = buffer[offset];

        if ((b1 & 0x80) == 0x00){  // pattern 0xxx-xxxx (uses 7) - 1 byte - ASCHII 0x20 a 0x7E
            uint32_t codePoint = b1;
            codePoints.push_back(codePoint);
            bytesUsed += 1;
            oneByteCount ++;
        } 
        
        else if ((b1 & 0xE0) == 0xC0) {// pattern 110x-xxxx (uses 5) - 2 bytes 
            bytesUsed = 2;

            // EOF case
            if (codePointBiggerThanLength(len, offset +1)) {
                Errors.push_back(Error(ErrorType::SecuenciaIncompleta, offset));
                offset++;
                continue;
            }

            // Checking byte 2
            uint32_t b2 = buffer[offset + 1];
            
            // Not continuation case
            if ((b2 & 0xC0) != 0x80) {  // not 10xxxxxx
                Errors.push_back(Error(ErrorType::ContinuacionNoEncontrada, offset));
                offset++;
                continue;
            }
            
            // Code point valid, se mete en vector
            uint32_t codePoint = (b1 & 0x1f) << 6 | (b2 & 0x3f);

            // Over long case 2 bytes
            if (codePoint < 0x80) {
                Errors.push_back(Error(ErrorType::SobreLarga, offset));
                offset++;
                continue;
            }  

            codePoints.push_back(codePoint);
            twoByteCount ++;
        }  

        else if ((b1 & 0xF0) == 0xE0) {  // pattern 1110-xxxx (uses 4)- 3 bytes
            bytesUsed = 3;

            // EOF case
            if (codePointBiggerThanLength(len, offset + 2)) {
                Errors.push_back(Error(ErrorType::SecuenciaIncompleta, offset));
                offset++;
                continue;
            }

            // Checking byte 2 & 3
            uint32_t b2 = buffer[offset + 1];
            uint32_t b3 = buffer[offset + 2];
            
            // Not continuation case
            if ((b2 & 0xC0) != 0x80 || (b2 & 0xc0) != 0x80) {  // not 10xxxxxx
                Errors.push_back(Error(ErrorType::ContinuacionNoEncontrada, offset));
                offset++;
                continue;
            }
            
            // Code point valid, se mete en vector
            uint32_t codePoint = (b1 & 0x0f) << 12 | (b2 & 0x3f) << 6 | (b3 & 0x3f);

            //Over long case 3 bytes
            if (codePoint < 0x800) {
                Errors.push_back(Error(ErrorType::SobreLarga, offset));
                offset++;
                continue;
            }  

            codePoints.push_back(codePoint);
            threeByteCount++;
        }

        else if ((b1 & 0xF8) == 0xF0) { // pattern 1111-0xxx (uses 3) - 4 bytes
            bytesUsed = 4;

            // EOF case
            if (codePointBiggerThanLength(len, offset + 3)) {
                Errors.push_back(Error(ErrorType::SecuenciaIncompleta, offset));
                offset++;
                continue;
            }

            // Checking byte 2 & 3
            uint32_t b2 = buffer[offset + 1];
            uint32_t b3 = buffer[offset + 2];
            uint32_t b4 = buffer[offset + 3];
            
            // Not continuation case
            if ((b2 & 0xC0) != 0x80 || (b2 & 0xc0) != 0x80 || (b3 & 0xc0) != 0x80) {  // not 10xxxxxx
                Errors.push_back(Error(ErrorType::ContinuacionNoEncontrada, offset));
                offset++;
                continue;
            }
            
            // Code point valid, se mete en vector
            uint32_t codePoint = (b1 & 0x07) << 18 | (b2 & 0x3f) << 12 | (b3 & 0x3f) << 6 | (b4 & 0x3f);
            
            // Overlong case - 4 bytes
            if (codePoint < 0x1000) {
                Errors.push_back(Error(ErrorType::SobreLarga, offset));
                offset++;
                continue;
            } 
            
            codePoints.push_back(codePoint);
            fourByteCount++;
        }

        else if ((b1 & 0xC0) == 0x80) { // 10xx-xxx - Byte Huerfano (tiene codigo de continuacion)
            Errors.push_back(Error(ErrorType::ContinuacionInesperada, offset));
            offset++;
            continue;
        }
        
        else { // Byte 1 se encuentre entre 0xF8-0xFF, byte lider invalido no puede ser 1111-1xxx ni mayor
            Errors.push_back(Error(ErrorType::LiderInvalido, offset));
            offset++;
            continue;
        }

        totalBytes += bytesUsed;
        validUTFBytes += bytesUsed;
        offset += bytesUsed;

    }
    

}

void Decoder::printFile () {
    std::cout << "=== Contenido decodificado ===\n \n";

    for (auto codePoint : codePoints) {
        if (codePoint >= 0x20 && codePoint <= 0x7e) {
            std::cout << static_cast<char>(codePoint) << std::endl;
        }
        else {
            std::cout << std::format("U+{04x}", codePoint) << std::endl;
        }
    }

}

void Decoder::printErrors () {
    std::cout << "=== Errores detectados ===\n \n";

    for (auto error : Errors) {
        std::cout << "[ offset " << error.offset << " ] ";
        if (error.errorType == ErrorType::ContinuacionInesperada) {
            std::cout << "Byte de continuación inesperado sin byte líder previo.\n";
        } else if (error.errorType == ErrorType::ContinuacionNoEncontrada) {
            std::cout << "Continuación no encontrada: el siguiente byte no tiene el patrón 10xxxxxx.\n";
        } else if (error.errorType == ErrorType::LiderInvalido) {
            std::cout << "Líder inválido: el byte no puede iniciar una secuencia UTF-8.\n";
        } else if (error.errorType == ErrorType::SecuenciaIncompleta) {
            std::cout<< "Secuencia incompleta: se esperaban bytes de continuación, EOF alcanzado.\n";
        } else if (error.errorType == ErrorType::SobreLarga) {
            std::cout << "Codificación sobrelarga: el code point utiliza más bytes de los necesarios.\n";
        } 
    }
    std::cout << std::endl;
}

void Decoder::printReport () {
    std::cout << "=== Resumen ===\n ";
    std::cout << std::format (
        "Bytes totales: {}\n"
        "Code points válidos: {}\n"
        " - 1 byte: {}\n"
        " - 2 bytes: {}\n"
        " - 3 bytes: {}\n"
        " - 4 bytes: {}\n"
        "Errores detectados: {}\n",
        totalBytes,
        codePoints.size(),
        oneByteCount,
        twoByteCount,
        threeByteCount,
        fourByteCount,
        Errors.size()
    );

}

void Decoder::getReport () {
    printFile();
    printErrors();
    printReport();
}