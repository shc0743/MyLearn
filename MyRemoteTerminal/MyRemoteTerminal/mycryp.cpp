#include <openssl/evp.h>
#include <openssl/sha.h>

#include "mycryp.h"

#pragma comment(lib, "crypt32.lib")
#pragma comment(lib, "libcrypto.lib")
#pragma comment(lib, "libssl.lib")



std::string rt_sha256(const std::string& str) {
	//char buf[2];
	//unsigned char hash[SHA256_DIGEST_LENGTH];
	//SHA256_CTX sha256;
	//SHA256_Init(&sha256);
	//SHA256_Update(&sha256, str.c_str(), str.size());
	//SHA256_Final(hash, &sha256);
	//std::string NewString = "";
	//for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
	//{
	//	sprintf_s(buf, "%02x", hash[i]);
	//	NewString = NewString + buf;
	//}
	//return NewString;

	unsigned char md[SHA256_DIGEST_LENGTH + 1]{};
	if (0 == SHA256((const unsigned char*)str.c_str(), str.length(), md)) {
		return ("");
	}
	std::string NewString = "";
	char buf[3]{};
	for (size_t i = 0; i < SHA256_DIGEST_LENGTH; i++) {
		sprintf_s(buf, "%02x", md[i]);
		NewString += buf;
	}
	return NewString;
}


