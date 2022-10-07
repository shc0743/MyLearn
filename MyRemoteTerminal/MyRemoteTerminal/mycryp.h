#pragma once
#include <string>

std::string __stdcall rt_sha256(const std::string& str);
bool __stdcall rt_generate_rsakey(FILE* pub, FILE* priv, unsigned int bits = 4096);
bool __stdcall rt_rsa_decrypt_by_privkey(const char* k,
	const unsigned char* in, size_t in_len,
	unsigned char* out, size_t out_len, int* real_len);

