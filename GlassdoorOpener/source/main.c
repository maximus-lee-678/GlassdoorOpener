#include "../headers/main.h"
#include "../headers/curl_operations.h"
#include "../headers/file_read_writer.h"
#include "../headers/user_interaction.h"

// Warning: unintended behaviour may occur for companies with <1000 reviews. (i.e. error -2314623978462)
// Fix later maybe
int main(void)
{
	// Make "temp"
	if (_mkdir(TEMP_DIRECTORY) && errno != EEXIST) {
		fprintf(stderr, "[x] File creation issue: Code %d\n", errno);
		return -3;
	}

	// Make "out"
	if (_mkdir(PERM_DIRECTORY) && errno != EEXIST) {
		fprintf(stderr, "[x] File creation issue: Code %d\n", errno);
		return -3;
	}

	// Make txt file
	FILE* fp;
	if ((fp = fopen(FINAL_ID_CONTAINING_FILE, "r")))
	{
		// File exists, close it
		fclose(fp);
	}
	else {
		fp = fopen(FINAL_ID_CONTAINING_FILE, "w");
		if (fp == NULL) {
			fprintf(stderr, "[x] Could not create file %s. Shutting down.\n", FINAL_ID_CONTAINING_FILE);
			return -3;
		}
		else {
			fwrite("0", 1, 1, fp);
			fclose(fp);
		}
	}

	welcome();
	int main_page_number = receive_digits();
	printf("%d\n", main_page_number);

	get_webpage("https://www.glassdoor.sg/Reviews/index.htm?overall_rating_low=3.5&page=1&filterType=RATING_OVERALL", FILENAME_GLASSDOOR_COMPANIES);
	get_companies_list();

	// for each company, get 1000 reviews (10 retrieves)
	init_current_id();
	for (int j = 0; j < COMPANIES_PER_PAGE; j++) {
		char* company_link = get_company_base_link(j + 1);

		for (int i = 0; i < REVIEW_GET_REPITIONS; i++) {
			// Any return value not = 0 is a fail, semula
			if (get_review_pages(company_link, i + 1)) {
				i--;
				remove_temp_review_files();
				continue;
			}

			process_one_block();
			remove_temp_review_files();
		}

		ascend_file();
		remove_purgatory_file();

		free(company_link);
	}

	curl_global_cleanup();
	goodbye();

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