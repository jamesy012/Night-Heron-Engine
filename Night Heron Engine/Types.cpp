#include "Types.h"

#include <Windows.h>
#include <wincrypt.h>

#include "Util.h"

CMString CMString::ToLower() {
	CMString newString;
	for (int i = 0; i < Length(); i++) {
		char letter = this->at(i);
		if (letter >= 'A' && letter <= 'Z') {
			letter -= 32;
		}
		newString += letter;
	}
	return newString;
}

void CMString::Hash(uchar* a_Output) const {
	HCRYPTPROV hProv;
	HCRYPTHASH hHash;
	CryptAcquireContext(&hProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT);
	CryptCreateHash(hProv, CALG_MD5, 0, 0, &hHash);
	CryptHashData(hHash, (BYTE*)Get(), Size(), 0);
	DWORD cbHash = HASH_LENGTH;
	BYTE rgbHash[HASH_LENGTH];
	CryptGetHashParam(hHash, HP_HASHVAL, rgbHash, &cbHash, 0);
	CryptDestroyHash(hHash);
	CryptReleaseContext(hProv, 0);
	
	memcpy(a_Output, rgbHash, HASH_LENGTH);
}

uchar * CMString::HashAlloc() const {
	uchar* output = new uchar[HASH_LENGTH];
	Hash(output);
	return output;
}


