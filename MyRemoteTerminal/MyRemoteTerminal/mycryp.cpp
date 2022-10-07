/*
My Crypt
*/

#define OSSL_DEPRECATED(x) 

#include <openssl/evp.h>
#include <openssl/sha.h>
#include <openssl/rsa.h>
#include <openssl/encoder.h>
#include <openssl/err.h>
#include <openssl/core_names.h>
#include <openssl/pem.h>
#include <openssl/x509.h>

#include "mycryp.h"

#pragma comment(lib, "crypt32.lib")
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "libcrypto.lib")
#pragma comment(lib, "libssl.lib")



std::string __stdcall rt_sha256(const std::string& str) {
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





/* Line 56 - 278
 * Copyright 2022 The OpenSSL Project Authors. All Rights Reserved.
 *
 * Licensed under the Apache License 2.0 (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://www.openssl.org/source/license.html
*/

/* A property query used for selecting algorithm implementations. */
static const char* propq = NULL;

/*
 * Generates an RSA public-private key pair and returns it.
 * The number of bits is specified by the bits argument.
 *
 * This uses the long way of generating an RSA key.
 */
static EVP_PKEY* __stdcall generate_rsa_key_long(OSSL_LIB_CTX* libctx, unsigned int bits)
{
	EVP_PKEY_CTX* genctx = NULL;
	EVP_PKEY* pkey = NULL;
	unsigned int primes = 2;

	/* Create context using RSA algorithm. "RSA-PSS" could also be used here. */
	genctx = EVP_PKEY_CTX_new_from_name(libctx, "RSA", propq);
	if (genctx == NULL) {
		fprintf(stderr, "EVP_PKEY_CTX_new_from_name() failed\n");
		goto cleanup;
	}

	/* Initialize context for key generation purposes. */
	if (EVP_PKEY_keygen_init(genctx) <= 0) {
		fprintf(stderr, "EVP_PKEY_keygen_init() failed\n");
		goto cleanup;
	}

	/*
	 * Here we set the number of bits to use in the RSA key.
	 * See comment at top of file for information on appropriate values.
	 */
	if (EVP_PKEY_CTX_set_rsa_keygen_bits(genctx, bits) <= 0) {
		fprintf(stderr, "EVP_PKEY_CTX_set_rsa_keygen_bits() failed\n");
		goto cleanup;
	}

	/*
	 * It is possible to create an RSA key using more than two primes.
	 * Do not do this unless you know why you need this.
	 * You ordinarily do not need to specify this, as the default is two.
	 *
	 * Both of these parameters can also be set via EVP_PKEY_CTX_set_params, but
	 * these functions provide a more concise way to do so.
	 */
	if (EVP_PKEY_CTX_set_rsa_keygen_primes(genctx, primes) <= 0) {
		fprintf(stderr, "EVP_PKEY_CTX_set_rsa_keygen_primes() failed\n");
		goto cleanup;
	}

	/*
	 * Generating an RSA key with a number of bits large enough to be secure for
	 * modern applications can take a fairly substantial amount of time (e.g.
	 * one second). If you require fast key generation, consider using an EC key
	 * instead.
	 *
	 * If you require progress information during the key generation process,
	 * you can set a progress callback using EVP_PKEY_set_cb; see the example in
	 * EVP_PKEY_generate(3).
	 */
	fprintf(stderr, "Generating RSA key, this may take some time...\n");
	if (EVP_PKEY_generate(genctx, &pkey) <= 0) {
		fprintf(stderr, "EVP_PKEY_generate() failed\n");
		goto cleanup;
	}

	/* pkey is now set to an object representing the generated key pair. */

cleanup:
	EVP_PKEY_CTX_free(genctx);
	return pkey;
}

/*
 * Generates an RSA public-private key pair and returns it.
 * The number of bits is specified by the bits argument.
 *
 * This uses a more concise way of generating an RSA key, which is suitable for
 * simple cases. It is used if -s is passed on the command line, otherwise the
 * long method above is used. The ability to choose between these two methods is
 * shown here only for demonstration; the results are equivalent.
 */
static EVP_PKEY* __stdcall generate_rsa_key_short(OSSL_LIB_CTX* libctx, unsigned int bits)
{
	EVP_PKEY* pkey = NULL;

	fprintf(stderr, "Generating RSA key, this may take some time...\n");
	pkey = EVP_PKEY_Q_keygen(libctx, propq, "RSA", (size_t)bits);

	if (pkey == NULL)
		fprintf(stderr, "EVP_PKEY_Q_keygen() failed\n");

	return pkey;
}

/*
 * Prints information on an EVP_PKEY object representing an RSA key pair.
 */
static int __stdcall dump_key(const EVP_PKEY* pkey, FILE* pub, FILE* priv)
{
	int rv = 0;
	int bits = 0;
	BIGNUM* n = NULL, * e = NULL, * d = NULL, * p = NULL, * q = NULL;

	/*
	 * Retrieve value of n. This value is not secret and forms part of the
	 * public key.
	 *
	 * Calling EVP_PKEY_get_bn_param with a NULL BIGNUM pointer causes
	 * a new BIGNUM to be allocated, so these must be freed subsequently.
	 */
	if (EVP_PKEY_get_bn_param(pkey, OSSL_PKEY_PARAM_RSA_N, &n) == 0) {
		//fprintf(stderr, "Failed to retrieve n\n");
		goto cleanup;
	}

	/*
	 * Retrieve value of e. This value is not secret and forms part of the
	 * public key. It is typically 65537 and need not be changed.
	 */
	if (EVP_PKEY_get_bn_param(pkey, OSSL_PKEY_PARAM_RSA_E, &e) == 0) {
		//fprintf(stderr, "Failed to retrieve e\n");
		goto cleanup;
	}

	/*
	 * Retrieve value of d. This value is secret and forms part of the private
	 * key. It must not be published.
	 */
	if (EVP_PKEY_get_bn_param(pkey, OSSL_PKEY_PARAM_RSA_D, &d) == 0) {
		//fprintf(stderr, "Failed to retrieve d\n");
		goto cleanup;
	}

	/*
	 * Retrieve value of the first prime factor, commonly known as p. This value
	 * is secret and forms part of the private key. It must not be published.
	 */
	if (EVP_PKEY_get_bn_param(pkey, OSSL_PKEY_PARAM_RSA_FACTOR1, &p) == 0) {
		//fprintf(stderr, "Failed to retrieve p\n");
		goto cleanup;
	}

	/*
	 * Retrieve value of the second prime factor, commonly known as q. This value
	 * is secret and forms part of the private key. It must not be published.
	 *
	 * If you are creating an RSA key with more than two primes for special
	 * applications, you can retrieve these primes with
	 * OSSL_PKEY_PARAM_RSA_FACTOR3, etc.
	 */
	if (EVP_PKEY_get_bn_param(pkey, OSSL_PKEY_PARAM_RSA_FACTOR2, &q) == 0) {
		//fprintf(stderr, "Failed to retrieve q\n");
		goto cleanup;
	}

	/*
	 * We can also retrieve the key size in bits for informational purposes.
	 */
	if (EVP_PKEY_get_int_param(pkey, OSSL_PKEY_PARAM_BITS, &bits) == 0) {
		//fprintf(stderr, "Failed to retrieve bits\n");
		goto cleanup;
	}

	/* Output hexadecimal representations of the BIGNUM objects. */
	//fprintf(stdout, "\nNumber of bits: %d\n\n", bits);
	//fprintf(stderr, "Public values:\n");
	//fprintf(stdout, "  n = 0x");
	//BN_print_fp(stdout, n);
	//fprintf(stdout, "\n");

	//fprintf(stdout, "  e = 0x");
	//BN_print_fp(stdout, e);
	//fprintf(stdout, "\n\n");

	//fprintf(stdout, "Private values:\n");
	//fprintf(stdout, "  d = 0x");
	//BN_print_fp(stdout, d);
	//fprintf(stdout, "\n");

	//fprintf(stdout, "  p = 0x");
	//BN_print_fp(stdout, p);
	//fprintf(stdout, "\n");

	//fprintf(stdout, "  q = 0x");
	//BN_print_fp(stdout, q);
	//fprintf(stdout, "\n\n");

	/* Output a PEM encoding of the public key. */
	if (PEM_write_PUBKEY(pub, pkey) == 0) {
		//fprintf(stderr, "Failed to output PEM-encoded public key\n");
		goto cleanup;
	}

	/*
	 * Output a PEM encoding of the private key. Please note that this output is
	 * not encrypted. You may wish to use the arguments to specify encryption of
	 * the key if you are storing it on disk. See PEM_write_PrivateKey(3).
	 */
	if (PEM_write_PrivateKey(priv, pkey, NULL, NULL, 0, NULL, NULL) == 0) {
		//fprintf(stderr, "Failed to output PEM-encoded private key\n");
		goto cleanup;
	}

	rv = 1;
cleanup:
	BN_free(n); /* not secret */
	BN_free(e); /* not secret */
	BN_clear_free(d); /* secret - scrub before freeing */
	BN_clear_free(p); /* secret - scrub before freeing */
	BN_clear_free(q); /* secret - scrub before freeing */
	return rv;
}

bool __stdcall rt_generate_rsakey(FILE* pub, FILE* priv, unsigned int bits) {

#if 0

	EVP_PKEY_CTX* ctx;
	EVP_PKEY* pkey = NULL;

	ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, NULL);
	if (!ctx) { return false; }
	if (EVP_PKEY_keygen_init(ctx) <= 0) { return false; }
	if (EVP_PKEY_CTX_set_rsa_keygen_bits(ctx, 2048) <= 0) { return false; }
	/* Generate key */
	if (EVP_PKEY_keygen(ctx, &pkey) <= 0) { return false; }

	EVP_PKEY_CTX_free(ctx);

	//size_t nSize = 0;
	//EVP_PKEY_get_raw_private_key(pkey, NULL, &nSize);
	//unsigned char* pBuffer = new unsigned char[nSize];
	//EVP_PKEY_get_raw_private_key(pkey, pBuffer, &nSize);
	//delete[] pBuffer;
	///**  -- EVP FUCK YOU --  **/
	
	FILE* fp = NULL;
	fopen_s(&fp, "ftest.txt", "w");
	OSSL_ENCODER_CTX* enc = OSSL_ENCODER_CTX_new_for_pkey(pkey, 0, NULL, NULL, NULL);
	OSSL_ENCODER_to_fp(enc, fp);
	fclose(fp);


	EVP_PKEY_free(pkey);

	return false;
#elif 0
	RSA* pRSA = RSA_new();
	BIGNUM* pBNe = BN_new();
	BN_set_word(pBNe, RSA_3);
	int ret = RSA_generate_key_ex(pRSA, 512, pBNe, NULL);
	BN_free(pBNe);

	BIO* pBIOpri = BIO_new_file("rsa.key", "w");
	BIO* pBIOpub = BIO_new_file("rsa.pub", "w");
	ret = i2d_RSAPrivateKey_bio(pBIOpri, pRSA);
	ret = i2d_RSAPublicKey_bio(pBIOpub, pRSA);
	BIO_free(pBIOpri);
	BIO_free(pBIOpub);

	RSA* pRSApri = RSA_new();
	RSA* pRSApub = RSA_new();
	BIO* pBIOpri2 = BIO_new_file("rsa.key", "r");
	BIO* pBIOpub2 = BIO_new_file("rsa.pub", "r");
	d2i_RSAPrivateKey_bio(pBIOpri2, &pRSApri);
	d2i_RSAPublicKey_bio(pBIOpub2, &pRSApub);
	BIO_free(pBIOpri2);
	BIO_free(pBIOpub2);

	int bytes = RSA_size(pRSApri);
	int bytes2 = RSA_size(pRSApub);

	char sText[] = "1234567890";

	unsigned char* pCipher = (unsigned char*)malloc(bytes);
	ret = RSA_public_encrypt(strlen(sText), (const unsigned char*)sText, pCipher, pRSApub, RSA_PKCS1_PADDING);
	printf("RSA_public_encrypt() ret:%d \n", ret);
	if (ret <= 0)
	{
		unsigned long err = ERR_get_error();
		printf("err:[%ld] \n", err);
		char sBuf[128] = { 0 };
	}

	unsigned char* pText = (unsigned char*)malloc(bytes);
	ret = RSA_private_decrypt(ret, (const unsigned char*)pCipher, pText, pRSApri, RSA_PKCS1_PADDING);
	printf("RSA_private_decrypt() ret:%d \n", ret);
	if (ret > 0)
	{
		MessageBoxA(0, (LPCSTR)pText,0,0);
	}
	free(pCipher);
	free(pText);

	RSA_free(pRSApri);
	RSA_free(pRSApub);
	RSA_free(pRSA);
#endif

	bool bSuccess = true;

	OSSL_LIB_CTX *libctx = NULL;
	EVP_PKEY *pkey = NULL;

	pkey = generate_rsa_key_long(libctx, bits);
	if (!pkey) {
		bSuccess = false;
		goto clean;
	}

	if (!dump_key(pkey, pub, priv)) {
		bSuccess = false;
		goto clean;
	}


clean:
	EVP_PKEY_free(pkey);
	OSSL_LIB_CTX_free(libctx);
	return bSuccess;
}

//bool __stdcall rt_rsa_encrypt = 0;
bool __stdcall rt_rsa_decrypt_by_privkey(const char* k,
	const unsigned char* in, size_t in_len,
	unsigned char* out, size_t out_len, int* real_len) {

	//EVP_PKEY* pkey = NULL;
	//pem_password_cb* cb = [](char* buf, int size, int rwflag,
	//  void* userdata)->int {
	//	return 0;
	//};
	//PEM_read_PrivateKey(k,&pkey,)

	RSA* rsa = NULL;
	BIO* keybio;

	keybio = BIO_new_mem_buf(k, -1);
	if (!keybio) return false;

	rsa = PEM_read_bio_RSAPrivateKey(keybio, &rsa, NULL, NULL);
	if (!rsa) {
		BIO_free_all(keybio);
		return false;
	}

	int decrypted_length = RSA_private_decrypt((int)in_len, in, out, rsa, RSA_PKCS1_OAEP_PADDING);
	*real_len = decrypted_length;

	BIO_free_all(keybio);
	RSA_free(rsa);

	if (decrypted_length < 0) return false;
	return true;
}



