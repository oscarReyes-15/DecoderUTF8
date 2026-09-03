#include <iostream>
#include <format>
#include <vector>
#include <string>

enum ErrorType {
    ContinuacionNoEncontrada,
    ContinuacionInesperada,
    SecuenciaIncompleta,
    LiderInvalido,  // Bono :p
    SobreLarga        // Bono :p
};

struct Error {
    ErrorType errorType;
    size_t offset;

    Error (ErrorType errorParam, size_t offsetParam): errorType(errorParam), offset(offsetParam) {}
};

class Decoder {
    public:
    Decoder ();
    
    std::vector<uint32_t> leerFileEnBufferDeBytes (const std::vector<char>& buffer);

    private:
    bool codePointBiggerThanLength (int length, int offset);


    std::vector<uint32_t> codePoints;
    std::vector<Error> Errors; 
    int bytesUsed;
};