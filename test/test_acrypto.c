#include <stdio.h>
#include <stdlib.h>
#include <check.h>
#include <openssl/sha.h>
#include "acrypto.h"

/* test cases for crypto functions require for WebSocket */
START_TEST(test_crypt_sha1)
{
  char key[] = "dGhlIHNhbXBsZSBub25jZQ==258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
  unsigned char expc[] = {0xb3, 0x7a, 0x4f, 0x2c, 0xc0, 0x62, 0x4f, 0x16, 0x90, 0xf6, 0x46, 0x6, 0xcf, 0x38, 0x59, 0x45, 0xb2, 0xbe, 0xc4, 0xea};
  int ksize = strlen(key);
  unsigned char sha1[SHA_DIGEST_LENGTH];
  int match;

  crypt_sha1(key, ksize, sha1); 
  match = strncmp(sha1, expc, SHA_DIGEST_LENGTH) == 0;

  fail_unless(match, "The sha1 did not match");
}
END_TEST

START_TEST(test_crypt_base64)
{
  unsigned char expc[] = "s3pPLMBiTxaQ9kYGzzhZRbK+xOo=";
  unsigned char sha1str[] = {0xb3, 0x7a, 0x4f, 0x2c, 0xc0, 0x62, 0x4f, 0x16, 0x90, 0xf6, 0x46, 0x6, 0xcf, 0x38, 0x59, 0x45, 0xb2, 0xbe, 0xc4, 0xea};

  unsigned char *b64str;
  int b64len;

  b64str = (unsigned char *)crypt_base64(sha1str, SHA_DIGEST_LENGTH, &b64len);

  fail_unless(b64len == strlen(expc), "The base64 string length did not match %d", b64len);
  fail_unless(strncmp(b64str, expc, b64len) == 0, "The base64 string did not match");
}
END_TEST

Suite *hndshk_suite(void)
{
  Suite *s = suite_create("acrypto");
  TCase *tc;
 
  /* base64 encode test case */
  tc = tcase_create("test_crypt_base64");
  tcase_add_test(tc, test_crypt_base64);
  suite_add_tcase(s, tc);

  /* sha1 test case */
  tc = tcase_create("test_crypt_sha1");
  tcase_add_test(tc, test_crypt_sha1);
  suite_add_tcase(s, tc);

  return s;
}


int main(void)
{
  int number_failed;
  Suite *s = hndshk_suite();
  SRunner *sr = srunner_create(s);

  /* run the test suite */
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);

  return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;  return 0;
}
