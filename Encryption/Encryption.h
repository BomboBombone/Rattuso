#include <Windows.h>
#include <iostream>

#define SPRUZ_SIG		"SPR\x00"
#define SPRUZ_END_SIG	"UZ\x00\x00"
#define SPRUZ_SIG_SIZE	4

enum class SPRUZ_ERROR_TYPE {
	SPRUZ_SUCCESS_ERROR,
	SPRUZ_SIG_ERROR,
	SPRUZ_END_SIG_ERROR
};

class Encryption {
private:
	uintptr_t _factor;
public:
	Encryption(uintptr_t factor) : _factor(factor) {};
	SPRUZ_ERROR_TYPE Encrypt(BYTE* baseptr, BYTE* outptr, size_t size);
	SPRUZ_ERROR_TYPE Decrypt(BYTE* baseptr, BYTE* outptr, size_t size);
};