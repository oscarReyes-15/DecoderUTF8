#include "Decoder.h"


Decoder::Decoder() {
}

bool Decoder::codePointBiggerThanLength (int length, int offset) {
    return offset  >= length;
}

std::vector<uint32_t> Decoder::leerFileEnBufferDeBytes(const std::vector<char>& buffer) {
    int offset = 0;
    int len = buffer.size();

    if (buffer[0] == 0xEF && buffer[1] == 0xBB && buffer[2] == 0xBF){
        offset = 3;  // skip BOM
    }

    while (offset < len){
        // Primer Byte
        uint32_t b1 = buffer[offset];

        if ((b1 & 0x80) == 0x00){  // pattern 0xxx-xxxx (uses 7) - 1 byte - ASCHII 0x20 a 0x7E
            uint32_t codePoint = b1;
            codePoints.push_back(codePoint);
            bytesUsed +=    1;
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
    }
    

    return {};
}

