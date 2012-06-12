#include <stdlib.h>
#include <stdio.h>
#include <check.h>
#include "util.h"

START_TEST(test_linkiter)
{
	const char name[2][30] = {"udom kasemosoth", "krit kasemosoth"};
	int i = 0;

	struct linklist *lnk = linklist_create();
	struct linkiter *iter = linkiter_create(lnk);
	
	linklist_add(lnk, (void*)name[0]);
	linklist_add(lnk, (void*)name[1]);

	while(!linkiter_eol(iter))
	{
	        char *value = (char*)linkiter_next(iter);
		
		fail_unless(strcmp(value, name[i]) == 0,
			    "Fail to get next item %s in the list",
			    name[i]);
		i++;
	}
}
END_TEST

START_TEST(test_linklist_add)
{
	const char name[] = "krit kasemosoth";
  	const char title[] = "senior software engineer";

	struct linklist *lnk = linklist_create();

	linklist_add(lnk, (void*)name);
	fail_unless(lnk->size == 1,
		    "Failed to add a new item %s to the list\n",
		    name);
	
	linklist_add(lnk, (void*)title);
	fail_unless(lnk->size == 2,
		    "Failed to add a new item %s to the list\n",
		    name);	
	
}
END_TEST

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

	/* test adding a new item to the link list */
	tc = tcase_create("test_linklist_add");
	tcase_add_test(tc, test_linklist_add);
	suite_add_tcase(s, tc);
    
	/* test iterating items in the link list */
	tc = tcase_create("test_linkiter");
	tcase_add_test(tc, test_linkiter);
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
