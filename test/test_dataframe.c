#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <check.h>
#include "util.h"
#include "dataframe.h"

START_TEST(test_datfrm_set_fin)
{
	unsigned char frm[] = {0x0};

	frm[0] = 0x0;
	datfrm_set_fin(frm, 1); 
	fail_unless(frm[0] == 0x80, "Failed to set FIN field %#x", frm[0]);

	frm[0] = 0x88;
	datfrm_set_fin(frm, 0); 
	fail_unless(frm[0] == 0x08, "Failed to set FIN field %#x", frm[0]);
}
END_TEST

START_TEST(test_datfrm_create_txt)
{
	unsigned char *frm;
	const unsigned char buff[] = "Hello World";
	const unsigned char pl[] = "Hello"; /* rfc6455 payload example */
	const unsigned char umfrm[] = {
		0x81, 0x05, 0x48, 0x65, 
		0x6c, 0x6c, 0x6f}; /* rfc6455 unmasked frame example */

	const unsigned char mfrm[] = {
		0x81, 0x85, 0x37, 0xfa, 
		0x21, 0x3d, 0x7f, 0x9f, 
		0x4d, 0x51, 0x58}; /* rfc6455 masked frame example */
 
	unsigned char bigbuff[300];
	size_t flen;
	unsigned char mkey[MASK_KEY_LEN];
	size_t plen;

	frm = datfrm_create_txt(buff, strlen(buff), MASK_OFF, &flen); 

	fail_unless(frm != NULL, "Failed to create text data frame");

	fail_unless(datfrm_get_fin(frm) == FIN_ON, "Failed to set FIN bit on");

	fail_unless(datfrm_get_opcode(frm) == OPCODE_TEXT, 
		    "Failed to set TEXT opcode");

	fail_unless(datfrm_get_mask(frm) == MASK_OFF,
		    "Failed to set mask");

	fail_unless(datfrm_get_payload_len(frm) == 11, 
		    "Failed to set payload lenght == 11");

	/* a frame lenght >= 126) */
	memset(bigbuff, 0, sizeof(bigbuff));
	bigbuff[sizeof(bigbuff) - 1] = '\0';
	frm = datfrm_create_txt(bigbuff, 300, MASK_OFF, &flen);
	fail_unless(datfrm_get_payload_len(frm) == 300, 
		    "Failed to set payload lenght == 300");

	/* a single-frame unmasked text message */
	frm = datfrm_create_txt(pl, strlen(pl), MASK_OFF, &flen);
	print_hex(frm, 7);
	print_hex(umfrm, 7);
	fail_unless(memcmp(frm, umfrm, 7) == 0, 
		    "Failed to create text frame"); 

	/* a single-frame masked text message */
	frm = datfrm_create_txt(pl, strlen(pl), MASK_ON, &flen);
	print_hex(frm, 11);
	print_hex(mfrm, 11);
	fail_unless(memcmp(frm, mfrm, 11) == 0, 
		    "Failed to create text frame");

	/* get mask key */
	plen = datfrm_get_payload_len(frm);
	datfrm_get_mask_key(frm, plen, mkey);
	fail_unless(memcmp(mkey, frm + 2, MASK_KEY_LEN) == 0,
		    "Failed to get mask key");
}
END_TEST


Suite *dataframe_suite(void)
{
	Suite *s = suite_create("dataframe");
	TCase *tc;
 
	/* data frame create test case */
	tc = tcase_create("test_datfrm_create_txt");
	tcase_add_test(tc, test_datfrm_create_txt);
	suite_add_tcase(s, tc);

	/* data frame set fin bit test case */
	tc = tcase_create("test_datfrm_set_fin");
	tcase_add_test(tc, test_datfrm_set_fin);
	suite_add_tcase(s, tc);

	return s;
}


int main(void)
{
	int number_failed;
	Suite *s = dataframe_suite();
	SRunner *sr = srunner_create(s);

	/* run the test suite */
	srunner_run_all(sr, CK_NORMAL);
	number_failed = srunner_ntests_failed(sr);
	srunner_free(sr);

	return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;  return 0;
}
