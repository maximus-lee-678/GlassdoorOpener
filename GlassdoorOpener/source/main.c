#include "../headers/main.h"
#include "../headers/curl_operations.h"
#include "../headers/file_read_writer.h"
#include "../headers/user_interaction.h"

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

	//process_companies_review_pages("./temp/company_review_1.txt");

	welcome();
	int page_start = receive_digits("Page Start");
	int page_count = receive_digits("Page Count");
	fprintf(stdout, "[i] Getting pages: %d - %d\n", page_start, page_start + page_count - 1);

	// force update ascendency file at start
	access_ascendency_file(page_start * COMPANIES_PER_PAGE * REVIEWS_TOTAL_PER_COMPANY);

	// e.g. start->5, len->5, 5 6 7 8 9
	for (int main_page_no = page_start; main_page_no < page_start + page_count; main_page_no++) {
		char main_url[UNIVERSAL_LENGTH];
		snprintf(main_url, UNIVERSAL_LENGTH, URL_GLASSDOOR_HOME, main_page_no);
		
		get_webpage(main_url, FILENAME_GLASSDOOR_COMPANIES);
		get_companies_list();

		// for each company, get 1000 reviews (10 retrieves)
		init_current_id();
		for (int company_no = 0; company_no < COMPANIES_PER_PAGE; company_no++) {
			char* company_link = get_company_base_link(company_no + 1);

			// each rep is 100, x10
			for (int reps = 0; reps < REVIEW_GET_REPETITIONS; reps++) {
				// Any return value not == 0 is a fail, semula
				if (get_review_pages(company_link, reps + 1)) {
					reps--;
					continue;
				}

				if (process_one_block()) {
					fprintf(stdout, "[i] Halting early.\n");
					break;	// cut short
				}
			}

			ascend_file(main_page_no);
			remove_purgatory_file();

			free(company_link);
		}
		remove_company_list();
	}

	remove_temp_review_files();

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