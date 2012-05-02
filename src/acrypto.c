#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/buffer.h>
#include "acrypto.h"

void test(char *message, int mlen)
{
        BIO *bio, *b64;
	//        char message[] = "Hello World \n";

        b64 = BIO_new(BIO_f_base64());
        bio = BIO_new_fp(stdout, BIO_NOCLOSE);
        bio = BIO_push(b64, bio);
        BIO_write(bio, message, mlen);
        BIO_flush(bio);

        BIO_free_all(bio);
}

unsigned char *crypt_sha1(const unsigned char *key, unsigned long ksize, char *sha1)
{
  return (unsigned char*)SHA1(key, ksize, sha1);
}

unsigned char *crypt_base64(const unsigned char *src, int slen, int *b64len)
{
  unsigned char *buff;
  BIO *b64, *bio;
  int c;
  BUF_MEM *bmp;

  /* create base64 function and encode base64 */
  b64 = BIO_new(BIO_f_base64());
  bio = BIO_new(BIO_s_mem()); //BIO_new_mem_buf(src, slen);
  bio = BIO_push(b64, bio);

  BIO_write(bio, src, slen);
  BIO_flush(bio);

  BIO_get_mem_ptr(b64, &bmp);
  
  if((*b64len = bmp->length - 1) > 0) {
    buff = (unsigned char *)malloc(bmp->length - 1);
    strncpy(buff, bmp->data, bmp->length - 1);
  } else {
    buff = (void*)0;
  }

  BIO_free_all(bio);

  //test(src, slen);

  return buff;
}
