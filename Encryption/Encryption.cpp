#include "framework.h"
#include "Encryption.h"

SPRUZ_ERROR_TYPE Encryption::Encrypt(BYTE* baseptr, BYTE* outptr, size_t size)
{


	return SPRUZ_ERROR_TYPE::SPRUZ_SUCCESS_ERROR;
}

SPRUZ_ERROR_TYPE Encryption::Decrypt(BYTE* baseptr, BYTE* outptr, size_t size)
{
	//Check that the buffer contains a SPRUZ encrypted data blob
	if (std::memcmp(baseptr, SPRUZ_SIG, SPRUZ_SIG_SIZE)) {
		return SPRUZ_ERROR_TYPE::SPRUZ_SIG_ERROR;
	}
	if (std::memcmp(baseptr + size - SPRUZ_SIG_SIZE, SPRUZ_END_SIG, SPRUZ_SIG_SIZE)) {
		return SPRUZ_ERROR_TYPE::SPRUZ_END_SIG_ERROR;
	}

	return SPRUZ_ERROR_TYPE::SPRUZ_SUCCESS_ERROR;
}
