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

	get_webpage("https://www.glassdoor.sg/Reviews/index.htm?overall_rating_low=3.5&page=1&filterType=RATING_OVERALL", FILENAME_GLASSDOOR_COMPANIES);
	get_companies_list();

	// for each company, get 1000 reviews (10 retrieves)
	for (int j = 0; j < COMPANIES_PER_PAGE; j++) {
		char* company_link = get_company_base_link(j + 1);
		printf("%s\n", company_link);

		for (int i = 0; i < REVIEW_PAGES_AT_ONCE; i++) {
			get_review_pages(company_link, i + 1);
			process_one_block();
			remove_temp_review_files();
		}

		free(company_link);
	}

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