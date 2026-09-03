#include <iostream>
#include <format>
#include <vector>
#include <string>

enum ErrorType {
    ContinuacionNoEncontrada, // Había otro byte, pero no era 10xxxxxx
    ContinuacionInesperada,   // 10xxxxxx apareció sin líder
    SecuenciaIncompleta,      // Se alcanzó EOF
    LiderInvalido,            // Bono
    SobreLarga                // Bono :p
};

struct Error {
    ErrorType errorType;
    size_t offset;

    Error (ErrorType errorParam, size_t offsetParam): errorType(errorParam), offset(offsetParam) {}
};

class Decoder {
    public:
    Decoder ();
    
    void leerFileEnBufferDeBytes (const std::vector<uint8_t>& buffer);
    void getReport() ;


    private:
    bool codePointBiggerThanLength (int length, int offset);

    void printFile ();
    void printErrors ();
    void printReport ();

    std::vector<uint32_t> codePoints;
    std::vector<Error> Errors; 
    
    size_t bytesUsed = 0;
    size_t totalBytes = 0;
    size_t validUTFBytes = 0;
    size_t oneByteCount = 0;
    size_t twoByteCount = 0;
    size_t threeByteCount = 0;
    size_t fourByteCount = 0;
};