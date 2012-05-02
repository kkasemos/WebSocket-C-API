
#include <stdlib.h>
#include <stdio.h>
#include <check.h>
#include "handshake.h"

#define CLI_HNDSHK_FIX "GET /chat HTTP/1.1\r\n" \
        "Host: localhost\r\n" \
        "Upgrade: websocket\r\n" \
        "Connection: Upgrade\r\n" \
        "Sec-WebSocket-Key: dGhlIHNhbXBsZSBub25jZQ==\r\n" \
        "Origin: http://localhost\r\n" \
        "Sec-WebSocket-Protocol: chat, superchat\r\n" \
        "Sec-WebSocket-Version: 13\r\n\r\n" \

#define SRV_HNDSHK_FIX "HTTP/1.1 101 Switching Protocols\r\n" \
        "Upgrade: websocket\r\n" \
        "Connection: Upgrade\r\n" \
        "Sec-WebSocket-Accept: s3pPLMBiTxaQ9kYGzzhZRbK+xOo=\r\n\r\n" \


START_TEST(test_hndshk_gen_srv)
{
	unsigned char buff[MAX_FLD_LEN];

	struct websck_hndshk exphnd;
	struct websck_hndshk clihnd;
	struct websck_hndshk acthnd;

	/* hndshk_init(&exphnd); */
	/* hndshk_init(&clihnd); */
	/* hndshk_init(&acthnd); */

	hndshk_parse(CLI_HNDSHK_FIX, strlen(CLI_HNDSHK_FIX), &clihnd);
	hndshk_parse(SRV_HNDSHK_FIX, strlen(SRV_HNDSHK_FIX), &exphnd);

	hndshk_gen_srv(&clihnd, &acthnd);

	printf("%s", hndshk_sprintf(buff, &clihnd));
	printf("%s", hndshk_sprintf(buff, &exphnd));

	fail_unless(hndshk_cmp(&acthnd, &exphnd), 
		    "Failed to generate the server handshake");

	hndshk_parse("hello world", strlen("hello world"), &clihnd);
	hndshk_gen_srv(&clihnd, &acthnd);
}
END_TEST

START_TEST(test_hndshk_gen_websck_accept)
{
  const unsigned char expc[] = "s3pPLMBiTxaQ9kYGzzhZRbK+xOo=";
  const unsigned char key[]  = "dGhlIHNhbXBsZSBub25jZQ==";
  unsigned char accept[MAX_FLD_LEN];

  int ksize = strlen(key);
  int esize = strlen(expc);
  int asize = 0;
  int match;
  
  hndshk_gen_websck_accept(key, ksize, accept, &asize);

  match = strncmp(accept, expc, esize) == 0;

  fail_unless(match, "The Sec-WebSocket-Accept did not match %*s", asize, accept);
}
END_TEST 

START_TEST(test_hndshk_validate_cli)
{
  struct websck_hndshk hnd;
  struct hndshk_criteria ctr;
  int valid = 0;

  hndshk_init(&hnd);

  hndshk_add_fld(&hnd, "Host", strlen("Host"), "localhost", strlen("localhost"));

  valid = hndshk_validate_cli(&hnd, &ctr);
  
  fail_unless(valid, "The handshake was invalid");
}
END_TEST

START_TEST(test_hndshk_parse)
{
     char line1[] = "GET /chat HTTP/1.1\r\nHost:localhost\r\nUpgrade:websocket\r\nOrigin:http://localhost\r\n";
     char line2[] = "Host:localhost";

     struct websck_hndshk acthnd; /* actual handshake */
     struct websck_hndshk exphnd; /* expected handshake */

     int same;
     const char *val;

     hndshk_init(&acthnd);
     hndshk_init(&exphnd);

     hndshk_set_startline("GET /chat HTTP/1.1", strlen("GET /chat HTTP/1.1"), &exphnd);

     strcpy(exphnd.fields[0].name, "Host");
     strcpy(exphnd.fields[0].value, "localhost");

     strcpy(exphnd.fields[1].name, "Upgrade");
     strcpy(exphnd.fields[1].value, "websocket");

     strcpy(exphnd.fields[2].name, "Origin");
     strcpy(exphnd.fields[2].value, "http://localhost");

     strcpy(exphnd.fields[3].name, "Sec-WebSocket-Key");
     strcpy(exphnd.fields[3].value, "dGhlIHNhbXBsZSBub25jZQ==");

     exphnd.numfld = 4;

     /* parse valid hand handshake fields */
     hndshk_parse(line1, strlen(line1), &acthnd);

     same = strcmp(exphnd.startline, acthnd.startline) == 0;
     val = acthnd.startline;
     fail_unless(same, "Failed to parse the start line %s", val);

     same = strcmp(exphnd.fields[0].name, acthnd.fields[0].name) == 0;
     val = acthnd.fields[0].name;
     fail_unless(same, "Failed to parse the Host %s", val);
 
     same = strcmp(exphnd.fields[0].value, acthnd.fields[0].value) == 0;
     val = acthnd.fields[0].value;
     fail_unless(same, "Failed to parse the Host value %s", val);

     same = strcmp(exphnd.fields[1].name, acthnd.fields[1].name) == 0;
     val = acthnd.fields[1].name;
     fail_unless(same, "Failed to parse the Upgrade %s", val);

     same = strcmp(exphnd.fields[1].value, acthnd.fields[1].value) == 0;
     val = acthnd.fields[1].value;
     fail_unless(same, "Failed to parse the Upgrade value %s", val);

     same = strcmp(exphnd.fields[2].name, acthnd.fields[2].name) == 0;
     val = acthnd.fields[2].name;
     fail_unless(same, "Failed to parse the Origin %s", val);

     same = strcmp(exphnd.fields[2].value, acthnd.fields[2].value) == 0;
     val = acthnd.fields[2].value;
     fail_unless(same, "Failed to parse the Origin value %s", val);

     /* parse invalid handshake fields */
     hndshk_init(&acthnd);
     hndshk_parse(line2, strlen(line2), &acthnd);

     same = strcmp(exphnd.fields[0].name, acthnd.fields[0].name) == 0;
     val = acthnd.fields[0].name;
     fail_unless(!same, "Succeed to parse the Host %s", val);
}
END_TEST

START_TEST(test_hndshk_add_fld)
{
  struct websck_hndshk hnd;
  const struct hndshk_field *fld;

  hndshk_init(&hnd);

  hndshk_add_fld(&hnd, "Host", 4, "localhost", 9);
  fld = hndshk_get_fld(&hnd, "Host");
  fail_unless(strcmp("localhost", fld->value) == 0,
	      "Failed to get the handshake field 'Host'");

  hndshk_add_fld(&hnd, "Upgrade", 7, "websocket", 9);
  fld = hndshk_get_fld(&hnd, "Upgrade");
  fail_unless(strcmp("websocket", fld->value) == 0,
	      "Failed to get the handshake field 'Upgrade'");
}
END_TEST

START_TEST(test_hndshk_parse_fldval)
{
  char line1[] = "Host: localhost";
  char expc1[] = " localhost";

  char line2[] = ":localhost";
  char expc2[] = "localhost";

  char line3[] = "Upgrade:";
  char expc3[] = "";

  char line4[] = "";
  char expc4[] = "";

  const char *val;
  int size;
  int same;
  
  /* parse field value */
  val = hndshk_parse_fldval(line1, strlen(line1), &size); 
  
  fail_unless(size == strlen(expc1), 
              "The field value size is not the same %d", size);
  fail_unless(strncmp(expc1, val, size) == 0, 
              "Failed to parse the field value %s\n", val);

  val = hndshk_parse_fldval(line2, strlen(line2), &size); 
  fail_unless(size == strlen(expc2), 
              "The field value size is not the same %d", size);
  fail_unless(strncmp(expc2, val, size) == 0, 
              "Failed to parse the field value %s\n", val);

  val = hndshk_parse_fldval(line3, strlen(line3), &size); 
  fail_unless(size == strlen(expc3), 
              "The field value size is not the same %d", size);
  fail_unless(strncmp(expc3, val, size) == 0, 
              "Failed to parse the field value %s\n", val);

  val = hndshk_parse_fldval(line4, strlen(line4), &size); 
  fail_unless(size == strlen(expc4), 
              "The field value size is not the same %d", size);
  fail_unless(strncmp(expc4, val, size) == 0, 
              "Failed to parse the field value %s)\n", val);
}
END_TEST

START_TEST(test_hndshk_parse_fldname)
{
  char line1[] = "Host: localhost";
  char expc1[] = "Host";

  char line2[] = "Host domain"; 
  char expc2[] = "";

  char line3[] = ":"; 
  char expc3[] = "";

  const char *name;
  int size;
  
  name = hndshk_parse_fldname(line1, strlen(line1), &size); 
  fail_unless(strncmp(expc1, name, size) == 0, 
              "Failed to parse the field %s\n", name);

  name = hndshk_parse_fldname(line2, strlen(line2), &size); 
  fail_unless(strncmp(expc2, name, size) == 0, 
              "Failed to parse the field %s\n", name);

  name = hndshk_parse_fldname(line3, strlen(line3), &size); 
  fail_unless(strncmp(expc3, name, size) == 0, 
              "Failed to parse the field %s\n", name);

}
END_TEST

START_TEST(test_hndshk_parse_fld)
{
  char line1[] = "Host: localhost\r\n";
  char expc1[] = "Host: localhost";

  char line2[] = "Host: domain\r\n\r\n"; 
  char expc2[] = "Host: domain";

  char line3[] = "\r\n\r\n"; 
  char expc3[] = "";
 
  char line4[] = "Host: mydomain\r"; 
  char expc4[] = "";

  char line5[] = "Host: mydomain\n"; 
  char expc5[] = "";

  char line6[] = "Host:localhost\r\nUpgrade:websocket\r\n";
  char expc6[] = "Host:localhost\r\n";

  const char *fld;
  int size;
  
  fld = hndshk_parse_fld(line1, strlen(line1), &size);
  fail_unless(strncmp(expc1, fld, size) == 0, 
              "Failed to parse the line %s\n", fld);

  fld = hndshk_parse_fld(line2, strlen(line2), &size);
  fail_unless(strncmp(expc2, fld, size) == 0, 
              "Failed to parse the line %s\n", fld);

  fld = hndshk_parse_fld(line3, strlen(line3), &size);
  fail_unless(strncmp(expc3, fld, size) == 0, 
              "Failed to parse the line %s\n", fld);

  fld = hndshk_parse_fld(line4, strlen(line4), &size);
  fail_unless(strncmp(expc4, fld, size) == 0, 
              "Failed to parse the line %s\n", fld);

  fld = hndshk_parse_fld(line5, strlen(line5), &size);
  fail_unless(strncmp(expc5, fld, size) == 0, 
              "Failed to parse the line %s\n", fld);

  fld = hndshk_parse_fld(line6, strlen(line6), &size);
  fail_unless(strncmp(expc6, fld, size) == 0, 
              "Failed to parse the line %s\n", fld);

}
END_TEST

Suite *hndshk_suite(void)
{
  Suite *s = suite_create("handshake");
  TCase *tc;
 
  /* handshake field parsing test case */
  tc = tcase_create("test_hndshk_parse_fld");
  tcase_add_test(tc, test_hndshk_parse_fld);
  suite_add_tcase(s, tc);

  /* handshake field name parsing test case */
  tc = tcase_create("test_hndshk_parse_fldname");
  tcase_add_test(tc, test_hndshk_parse_fldname);
  suite_add_tcase(s, tc);

  /* handshake field value parsing test case */
  tc = tcase_create("test_hndshk_parse_fldval");
  tcase_add_test(tc, test_hndshk_parse_fldval);
  suite_add_tcase(s, tc);

  /* handshake parsing test case */
  tc = tcase_create("test_hndshk_parse");
  tcase_add_test(tc, test_hndshk_parse);
  suite_add_tcase(s, tc);

  /* add handshake field test case */
  tc = tcase_create("test_hndshk_add_fld");
  tcase_add_test(tc, test_hndshk_add_fld);
  suite_add_tcase(s, tc);

  /* gen Sec-WebSocket-Accept field test case */
  tc = tcase_create("test_hndshk_gen_websck_accept");
  tcase_add_test(tc, test_hndshk_gen_websck_accept);
  suite_add_tcase(s, tc);    

  /* gen server handshake test case */
  tc = tcase_create("test_hndshk_gen_srv");
  tcase_add_test(tc, test_hndshk_gen_srv);
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
