#include <stdlib.h>
#include <stdio.h>
#include <check.h>
#include "util.h"

START_TEST(test_print_hex)
{
  const char name[] = "krit kasemosoth";
  
  print_hex(name, strlen(name));
}
END_TEST

START_TEST(test_trim_space)
{
  char line1[] = "  Host: localhost  ";
  char expc1[] = "Host: localhost";

  char line2[] = "  Host: localhost";
  char expc2[] = "Host: localhost";

  char line3[] = "  ";
  char expc3[] = "";
  
  trim_space(line1);
  fail_unless(strcmp(expc1, line1) == 0, "Failed to parse the line %s]\n", line1);

  trim_space(line2);
  fail_unless(strcmp(expc2, line2) == 0, "Failed to parse the line %s]\n", line2);

  trim_space(line3);
  fail_unless(strcmp(expc3, line3) == 0, "Failed to parse the line %s]\n", line3);
}
END_TEST

Suite *util_suite(void)
{
  Suite *s = suite_create("util");
  TCase *tc;
 
 /* trim spaces at the front and end of string  test case */
  tc = tcase_create("test_trim_space");
  tcase_add_test(tc, test_trim_space);
  suite_add_tcase(s, tc);

 /* trim spaces at the front and end of string  test case */
  tc = tcase_create("test_print_hex");
  tcase_add_test(tc, test_print_hex);
  suite_add_tcase(s, tc);
    
  return s;
}


int main(void)
{
  int number_failed;
  Suite *s = util_suite();
  SRunner *sr = srunner_create(s);

  /* run the test suite */
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);

  return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;  return 0;
}
