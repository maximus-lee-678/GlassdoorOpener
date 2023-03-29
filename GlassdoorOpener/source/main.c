#include "../headers/main.h"
#include "../headers/curl_operations.h"
#include "../headers/file_read_writer.h"

int main(void)
{
	if (_mkdir(TEMP_DIRECTORY) && errno != EEXIST) {
		fprintf(stderr, "[x] File creation issue: Code %d\n", errno);
		return -3;
	}

	if (_mkdir(PERM_DIRECTORY) && errno != EEXIST) {
		fprintf(stderr, "[x] File creation issue: Code %d\n", errno);
		return -3;
	}

	//get_webpage("https://www.glassdoor.sg/Reviews/index.htm?overall_rating_low=3.5&page=1&filterType=RATING_OVERALL", FILENAME_GLASSDOOR_COMPANIES);
	//get_companies_list();
	//get_companies_review_pages();
	get_review_pages("/Reviews/Accenture-Reviews-E4138.htm");
	process_one_block();

	curl_global_cleanup();

	return 0;
}

// ensure malloc success, eliminate catastrophic failure.
void mallocChecker(void* ptr)
{
	if (ptr == NULL)
	{
		fprintf(stderr, "[x] Unable to allocate memory!\n[x] Program closed with code %d\n", -1);
		exit(-1);
	}
}