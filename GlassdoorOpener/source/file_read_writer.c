#include "../headers/file_read_writer.h"


static int current_id = 0;

void init_current_id() {
	current_id = access_ascendency_file(0) + 1;
}

// Output e.g.: /Reviews/Company-Reviews-E12345.htm
// Insert _<page-no> before .htm to move pages
u_int get_companies_list() {
	char chunk[UNIVERSAL_LENGTH];
	size_t len = sizeof(chunk);		// Store the chunks of text into a line buffer
	char* line = malloc(len);
	mallocChecker(line);
	line[0] = '\0';					// Zeroise the string
	char* examiner = NULL;
	int iteration = 0;

	FILE* fp_read = fopen(FILENAME_GLASSDOOR_COMPANIES, "r");
	FILE* fp_write = fopen(FILENAME_GLASSDOOR_COMPANIES_LIST, "w");

	while (fgets(chunk, sizeof(chunk), fp_read) != NULL) {
		// Resize the line buffer if necessary, x2 each time
		size_t len_used = strlen(line);
		size_t chunk_used = strlen(chunk);

		if (len - len_used < chunk_used) {
			len *= 2;
			line = realloc(line, len);
			mallocChecker(line);
		}

		// Copy the chunk to the end of the line buffer
		strncpy(line + len_used, chunk, len - len_used);
		len_used += chunk_used;

		// Check if line contains '\n', if yes process the line of text
		if (line[len_used - 1] == '\n') {
			examiner = strstr(line, HTML_GET_REVIEWS);
			while (iteration < COMPANIES_PER_PAGE && examiner != NULL) {
				size_t str_length = 0;
				char temp[UNIVERSAL_LENGTH];

				examiner += strlen(HTML_GET_REVIEWS);			// Seek to start of review URL
				str_length = (int)(strstr(examiner, "\"") - examiner);		// Measure length (exampleURL")
				strncpy(temp, examiner, str_length);						// Copy URL
				temp[str_length] = '\n';									// Provide a newline
				temp[str_length + 1] = '\0';									// Provide a terminator
				fprintf(fp_write, "%s", temp);
				iteration++;
				examiner = strstr(examiner, HTML_GET_REVIEWS);
			}

			// "Empty" the line buffer
			line[0] = '\0';

			if (iteration == COMPANIES_PER_PAGE)
				break;
		}
	}

	fclose(fp_read);
	fclose(fp_write);
	free(line);

	return 0;
}

/*
 * This code could be written better... for shame!
 *
 * Extracts review data from 1 page containing 10 reviews
 * As a result, most operations here loop 10 times
 * Implemented in a way that considers data position in the file to reduce time taken fseeking
 * making it look a bit scuffed
 *
 * 1. Gets company name
 * 2. Gets huge block of text containing most info
 * 3. Gets job position and location
 */
u_int process_companies_review_pages(char* file_path) {
	review_struct reviews[10];

	char chunk[UNIVERSAL_LENGTH];
	size_t len = sizeof(chunk);		// Store the chunks of text into a line buffer
	char* line = malloc(len);
	mallocChecker(line);
	line[0] = '\0';					// Zeroise the string
	char* examiner = NULL;

	size_t str_length = 0;
	char temp_str_buf[UNIVERSAL_LENGTH];
	int temp_int_buf = 0;

	FILE* fp_read = fopen(file_path, "r");

	fprintf(stdout, "[>] Reading: <%s> Stage-", file_path);

	int flag = 0;
	// 1. Gets company name
	while (fgets(chunk, sizeof(chunk), fp_read) != NULL) {
		// Resize the line buffer if necessary, x2 each time
		size_t len_used = strlen(line);
		size_t chunk_used = strlen(chunk);

		if (len - len_used < chunk_used) {
			len *= 2;
			line = realloc(line, len);
			mallocChecker(line);
		}

		// Copy the chunk to the end of the line buffer
		strncpy(line + len_used, chunk, len - len_used);
		len_used += chunk_used;

		// Check if line contains '\n', if yes process the line of text
		if (line[len_used - 1] == '\n') {
			// Look for company name
			examiner = strstr(line, REVIEW_COMPANY_NAME);

			if (examiner) {
				flag = 1;
				line[0] = '\0';				// "Empty" the line buffer

				break;
			}

			line[0] = '\0';				// "Empty" the line buffer
		}
	}

	examiner += strlen(REVIEW_COMPANY_NAME);							// Seek to start of name
	str_length = (int)(strstr(examiner, "Reviews") - examiner);			// Measure length until before Reviews
	str_length -= 1;													// String contains " ", -1 to truncate
	for (int iterator = 0; iterator < REVIEWS_PER_PAGE; iterator++) {	// Copy name for all reviews
		strncpy(reviews[iterator].company, examiner, str_length);
		reviews[iterator].company[str_length] = '\0';					// Provide a terminator
	}

	fprintf(stdout, "1-");
	//////////////////////////////////////////////////////////////////////////////////////////////////

	flag = 0;
	// 2. Gets huge block of text containing most info
	while (fgets(chunk, sizeof(chunk), fp_read) != NULL) {
		// Resize the line buffer if necessary, x2 each time
		size_t len_used = strlen(line);
		size_t chunk_used = strlen(chunk);

		if (len - len_used < chunk_used) {
			len *= 2;
			line = realloc(line, len);
			mallocChecker(line);
		}

		// Copy the chunk to the end of the line buffer
		strncpy(line + len_used, chunk, len - len_used);
		len_used += chunk_used;

		// Check if line contains '\n', if yes process the line of text
		if (line[len_used - 1] == '\n') {
			// Look for start of massive block of text containing REVIEWS_PER_PAGE(10) counts of 
			examiner = strstr(line, REVIEW_START);

			if (examiner) {
				flag = 1;
				line[0] = '\0';				// "Empty" the line buffer
				break;
			}

			line[0] = '\0';				// "Empty" the line buffer
		}
	}

	// Review page is devoid of reviews, stop early
	if (!flag) {
		fprintf(stdout, "!\n[i] 0 reviews on this page. <%s>\n", file_path);

		fclose(fp_read);
		free(line);

		return 1;
	}

	int reviews_per_page = REVIEWS_PER_PAGE;
	for (int iterator = 0; iterator < reviews_per_page; iterator++) {
		str_length = 0;
		temp_int_buf = 0;

		// Get "reviewDateTime"
		examiner = strstr(examiner, REVIEW_DATE);

		// Ensure review exists, if not, cut loop short early
		if (examiner == NULL) {
			reviews_per_page = iterator;
			fclose(fp_read);

			break;
		}

		examiner += strlen(REVIEW_DATE);							// Skip string itself
		examiner += 3;															// Skip ":"
		str_length = (int)(strstr(examiner, "\"") - examiner);					// Measure length until before "
		strncpy(reviews[iterator].review_date_time, examiner, str_length);		// Copy
		reviews[iterator].review_date_time[str_length] = '\0';					// Provide a terminator

		// Get "ratingOverall"
		examiner = strstr(examiner, REVIEW_RATING_OVERALL);
		examiner += strlen(REVIEW_RATING_OVERALL);					// Skip string itself
		examiner += 2;															// Skip ":
		str_length = 1;															// Rating: 1 digit 0-5 OR NULL
		strncpy(temp_str_buf, examiner, str_length);							// Copy 1 character to temp buffer
		if (isdigit(temp_str_buf[0])) {											// Check if digit or 'n'
			temp_str_buf[str_length] = '\0';									// Provide a terminator
			reviews[iterator].rating_overall = atoi(temp_str_buf);				// Copy to real
		}
		else {
			reviews[iterator].rating_overall = 0;
		}

		// Get "ratingCeo"
		examiner = strstr(examiner, REVIEW_RATING_CEO);
		examiner += strlen(REVIEW_RATING_CEO);						// Skip string itself
		examiner += 2;															// Skip ":
		str_length = 1;															// Rating: 1 digit 0-5 OR NULL
		strncpy(temp_str_buf, examiner, str_length);							// Copy 1 character to temp buffer
		if (isdigit(temp_str_buf[0])) {											// Check if digit or 'n'
			temp_str_buf[str_length] = '\0';									// Provide a terminator
			reviews[iterator].rating_ceo = atoi(temp_str_buf);					// Copy to real
		}
		else {
			reviews[iterator].rating_ceo = 0;
		}

		// Get "ratingBusinessOutlook"
		examiner = strstr(examiner, REVIEW_RATING_BUSI_OUTLOOK);
		examiner += strlen(REVIEW_RATING_BUSI_OUTLOOK);					// Skip string itself
		examiner += 2;															// Skip ":
		str_length = 1;															// Rating: 1 digit 0-5 OR NULL
		strncpy(temp_str_buf, examiner, str_length);							// Copy 1 character to temp buffer
		if (isdigit(temp_str_buf[0])) {											// Check if digit or 'n'
			temp_str_buf[str_length] = '\0';									// Provide a terminator
			reviews[iterator].rating_business_outlook = atoi(temp_str_buf);		// Copy to real
		}
		else {
			reviews[iterator].rating_business_outlook = 0;
		}


		// Get "ratingWorkLifeBalance"
		examiner = strstr(examiner, REVIEW_RATING_LIFE_BAL);
		examiner += strlen(REVIEW_RATING_LIFE_BAL);				// Skip string itself
		examiner += 2;															// Skip ":
		str_length = 1;															// Rating: 1 digit 0-5 OR NULL
		strncpy(temp_str_buf, examiner, str_length);							// Copy 1 character to temp buffer
		if (isdigit(temp_str_buf[0])) {											// Check if digit or 'n'
			temp_str_buf[str_length] = '\0';									// Provide a terminator
			reviews[iterator].rating_work_life_balance = atoi(temp_str_buf);	// Copy to real
		}
		else {
			reviews[iterator].rating_work_life_balance = 0;
		}


		// Get "ratingCultureAndValues"
		examiner = strstr(examiner, REVIEW_RATING_CULT_VAL);
		examiner += strlen(REVIEW_RATING_CULT_VAL);				// Skip string itself
		examiner += 2;															// Skip ":
		str_length = 1;															// Rating: 1 digit 0-5 OR NULL
		strncpy(temp_str_buf, examiner, str_length);							// Copy 1 character to temp buffer
		if (isdigit(temp_str_buf[0])) {											// Check if digit or 'n'
			temp_str_buf[str_length] = '\0';									// Provide a terminator
			reviews[iterator].rating_culture_and_values = atoi(temp_str_buf);	// Copy to real
		}
		else {
			reviews[iterator].rating_culture_and_values = 0;
		}


		// Get "ratingDiversityAndInclusion"
		examiner = strstr(examiner, REVIEW_RATING_DIVR_INCLU);
		examiner += strlen(REVIEW_RATING_DIVR_INCLU);				// Skip string itself
		examiner += 2;															// Skip ":
		str_length = 1;															// Rating: 1 digit 0-5 OR NULL
		strncpy(temp_str_buf, examiner, str_length);							// Copy 1 character to temp buffer
		if (isdigit(temp_str_buf[0])) {											// Check if digit or 'n'
			temp_str_buf[str_length] = '\0';									// Provide a terminator
			reviews[iterator].rating_diversity_and_inclusion = atoi(temp_str_buf);	// Copy to real
		}
		else {
			reviews[iterator].rating_diversity_and_inclusion = 0;
		}

		// Get "ratingSeniorLeadership"
		examiner = strstr(examiner, REVIEW_RATING_SNR_LEAD);
		examiner += strlen(REVIEW_RATING_SNR_LEAD);				// Skip string itself
		examiner += 2;															// Skip ":
		str_length = 1;															// Rating: 1 digit 0-5 OR NULL
		strncpy(temp_str_buf, examiner, str_length);							// Copy 1 character to temp buffer
		if (isdigit(temp_str_buf[0])) {											// Check if digit or 'n'
			temp_str_buf[str_length] = '\0';									// Provide a terminator
			reviews[iterator].rating_senior_leadership = atoi(temp_str_buf);	// Copy to real
		}
		else {
			reviews[iterator].rating_senior_leadership = 0;
		}

		// Get "ratingRecommendToFriend"
		examiner = strstr(examiner, REVIEW_RATING_RECM_FRND);
		examiner += strlen(REVIEW_RATING_RECM_FRND);				// Skip string itself
		examiner += 2;															// Skip ":
		str_length = 1;															// Rating: 1 digit 0-5 OR NULL
		strncpy(temp_str_buf, examiner, str_length);							// Copy 1 character to temp buffer
		if (isdigit(temp_str_buf[0])) {											// Check if digit or 'n'
			temp_str_buf[str_length] = '\0';									// Provide a terminator
			reviews[iterator].rating_recommend_to_friend = atoi(temp_str_buf);	// Copy to real
		}
		else {
			reviews[iterator].rating_recommend_to_friend = 0;
		}

		// Get "ratingCareerOpportunities"
		examiner = strstr(examiner, REVIEW_RATING_CAREER);
		examiner += strlen(REVIEW_RATING_CAREER);					// Skip string itself
		examiner += 2;															// Skip ":
		str_length = 1;															// Rating: 1 digit 0-5 OR NULL
		strncpy(temp_str_buf, examiner, str_length);							// Copy 1 character to temp buffer
		if (isdigit(temp_str_buf[0])) {											// Check if digit or 'n'
			temp_str_buf[str_length] = '\0';									// Provide a terminator
			reviews[iterator].rating_career_opportunities = atoi(temp_str_buf);	// Copy to real
		}
		else {
			reviews[iterator].rating_career_opportunities = 0;
		}

		// Get "ratingCompensationAndBenefits"
		examiner = strstr(examiner, REVIEW_RATING_COMP_BENE);
		examiner += strlen(REVIEW_RATING_COMP_BENE);						// Skip string itself
		examiner += 2;																	// Skip ":
		str_length = 1;																	// Rating: 1 digit 0-5 OR NULL
		strncpy(temp_str_buf, examiner, str_length);									// Copy 1 character to temp buffer
		if (isdigit(temp_str_buf[0])) {													// Check if digit or 'n'
			temp_str_buf[str_length] = '\0';											// Provide a terminator
			reviews[iterator].rating_compensation_and_benefits = atoi(temp_str_buf);	// Copy to real
		}
		else {
			reviews[iterator].rating_compensation_and_benefits = 0;
		}

		// Get "isCurrentJob"
		examiner = strstr(examiner, REVIEW_IS_CURRENT_JOB);
		examiner += strlen(REVIEW_IS_CURRENT_JOB);							// Skip string itself
		examiner += 2;																	// Skip ":
		if (examiner[0] == 't') {														// Check if char 1 is 't' or 'f'
			reviews[iterator].is_current_job = 1;	// Copy to real
		}
		else {
			reviews[iterator].is_current_job = 0;
		}

		// Get "lengthOfEmployment"
		examiner = strstr(examiner, REVIEW_LENGTH_OF_EMPLOYMENT);
		examiner += strlen(REVIEW_LENGTH_OF_EMPLOYMENT);					// Skip string itself
		examiner += 2;																	// Skip ":
		str_length = (int)(strstr(examiner, ",") - examiner);							// Measure length until before ,
		strncpy(temp_str_buf, examiner, str_length);									// Copy to temp buffer
		reviews[iterator].length_of_employment = atoi(temp_str_buf);

		// Get "pros"
		examiner = strstr(examiner, REVIEW_PROS);
		examiner += strlen(REVIEW_PROS);							// Skip string itself
		examiner += 1;															// Skip :
		if (examiner[0] == '\"') {												// Check if "review OR null
			examiner += 1;														// Skip "
			str_length = (int)(strstr(examiner, "\"") - examiner);				// Measure length until before "
			strncpy(reviews[iterator].pros, examiner, str_length);				// Copy
			reviews[iterator].pros[str_length] = '\0';							// Provide a terminator
		}
		else {
			str_length = (int)(strstr(examiner, ",") - examiner);				// Measure length until before ,
			strncpy(reviews[iterator].pros, examiner, str_length);				// Copy
			reviews[iterator].pros[str_length] = '\0';							// Provide a terminator					
		}

		// Get "cons"
		examiner = strstr(examiner, REVIEW_CONS);
		examiner += strlen(REVIEW_CONS);							// Skip string itself
		examiner += 1;															// Skip :
		if (examiner[0] == '\"') {												// Check if "review OR null
			examiner += 1;														// Skip "
			str_length = (int)(strstr(examiner, "\"") - examiner);				// Measure length until before "
			strncpy(reviews[iterator].cons, examiner, str_length);				// Copy
			reviews[iterator].cons[str_length] = '\0';							// Provide a terminator
		}
		else {
			str_length = (int)(strstr(examiner, ",") - examiner);				// Measure length until before ,
			strncpy(reviews[iterator].cons, examiner, str_length);				// Copy
			reviews[iterator].cons[str_length] = '\0';							// Provide a terminator					
		}

		// Get "summary"
		examiner = strstr(examiner, REVIEW_SUMMARY);
		examiner += strlen(REVIEW_SUMMARY);										// Skip string itself
		examiner += 1;															// Skip :
		if (examiner[0] == '\"') {												// Check if "review OR null
			examiner += 1;														// Skip "
			str_length = (int)(strstr(examiner, "\"") - examiner);				// Measure length until before "
			strncpy(reviews[iterator].review_summary, examiner, str_length);	// Copy
			reviews[iterator].review_summary[str_length] = '\0';				// Provide a terminator
		}
		else {
			str_length = (int)(strstr(examiner, ",") - examiner);				// Measure length until before ,
			strncpy(reviews[iterator].cons, examiner, str_length);				// Copy
			reviews[iterator].cons[str_length] = '\0';							// Provide a terminator					
		}

		// Get "advice"
		examiner = strstr(examiner, REVIEW_ADVICE_TO_MGMT);
		examiner += strlen(REVIEW_ADVICE_TO_MGMT);					// Skip string itself
		examiner += 1;															// Skip :
		if (examiner[0] == '\"') {												// Check if "review OR null
			examiner += 1;														// Skip "
			str_length = (int)(strstr(examiner, "\"") - examiner);				// Measure length until before "
			strncpy(reviews[iterator].advice_to_management, examiner, str_length);	// Copy
			reviews[iterator].advice_to_management[str_length] = '\0';			// Provide a terminator
		}
		else {
			str_length = (int)(strstr(examiner, ",") - examiner);				// Measure length until before ,
			strncpy(reviews[iterator].advice_to_management, examiner, str_length);	// Copy
			reviews[iterator].advice_to_management[str_length] = '\0';			// Provide a terminator					
		}
	}

	fprintf(stdout, "2-");
	//////////////////////////////////////////////////////////////////////////////////////////////////

	// 3. Gets job position and location
	// Start: <span class="middle common__EiReviewDetailsStyle__newGrey">01 Jan 2023 - Job Title</span>
	// Occasional: 30 chars later will contain in<!-- --> <span>Country</span>
	int iterator_3 = 0;
	while (fgets(chunk, sizeof(chunk), fp_read) != NULL) {
		// Resize the line buffer if necessary, x2 each time
		size_t len_used = strlen(line);
		size_t chunk_used = strlen(chunk);

		if (len - len_used < chunk_used) {
			len *= 2;
			line = realloc(line, len);
			mallocChecker(line);
		}

		// Copy the chunk to the end of the line buffer
		strncpy(line + len_used, chunk, len - len_used);
		len_used += chunk_used;

		int flag = 0;
		// Check if line contains '\n', if yes process the line of text
		if (line[len_used - 1] == '\n') {
			// Look for start of job position and location
			examiner = strstr(line, REVIEW_POSITION_LOCATION_START);

			if (examiner && iterator_3 < reviews_per_page) {
				examiner += strlen(REVIEW_POSITION_LOCATION_START);
				examiner = strstr(examiner, "-");
				examiner += strlen("-");
				examiner += 1;
				str_length = (int)(strstr(examiner, "<") - examiner);				// Measure length until before <
				strncpy(reviews[iterator_3].position, examiner, str_length);	// Copy
				reviews[iterator_3].position[str_length] = '\0';			// Provide a terminator

				examiner = strstr(examiner, REVIEW_LOCATION_START);
				if (examiner != NULL) {
					examiner += strlen(REVIEW_LOCATION_START);
					str_length = (int)(strstr(examiner, "<") - examiner);				// Measure length until before <
					strncpy(reviews[iterator_3].country, examiner, str_length);	// Copy
					reviews[iterator_3].country[str_length] = '\0';			// Provide a terminator
				}
				else {
					strncpy(reviews[iterator_3].country, "null", strlen("null"));	// Copy
					reviews[iterator_3].country[strlen("null")] = '\0';			// Provide a terminator
				}
				iterator_3++;
			}

			if (iterator_3 == reviews_per_page)
				break;

			// "Empty" the line buffer
			line[0] = '\0';
		}
	}

	fprintf(stdout, "3 ~ Done.\n");

	if (reviews_per_page != REVIEWS_PER_PAGE) {
		fprintf(stdout, "[i] Fewer than %d reviews on this page. <%s>\n", REVIEWS_PER_PAGE, file_path);
	}

	fclose(fp_read);

	FILE* fp_write = fopen(FILENAME_GLASSDOOR_PURGATORY_OUTPUT, "a");

	// very cool
	for (int iterator = 0; iterator < reviews_per_page; iterator++) {
		fprintf(fp_write, "%d\t%s\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%s\t%s\t%s\t%s\t%s\t%s\t%s\n",
			current_id++, reviews[iterator].company, reviews[iterator].rating_overall, reviews[iterator].rating_recommend_to_friend,
			reviews[iterator].rating_ceo, reviews[iterator].rating_business_outlook, reviews[iterator].rating_work_life_balance,
			reviews[iterator].rating_culture_and_values, reviews[iterator].rating_diversity_and_inclusion, reviews[iterator].rating_career_opportunities,
			reviews[iterator].rating_compensation_and_benefits, reviews[iterator].rating_senior_leadership, reviews[iterator].is_current_job,
			reviews[iterator].length_of_employment, reviews[iterator].review_date_time, reviews[iterator].position,
			reviews[iterator].country, reviews[iterator].review_summary, reviews[iterator].pros,
			reviews[iterator].cons, reviews[iterator].advice_to_management);
	}

	fclose(fp_write);

	free(line);

	if (reviews_per_page != REVIEWS_PER_PAGE) {
		return 2;
	}
	else {
		return 0;
	}
}

// Loads 10 reviews into process_companies_review_pages.
u_int process_one_block() {
	char temp_buf[UNIVERSAL_LENGTH] = "";

	int flag = 0;
	for (int i = 0; i < REVIEW_PAGES_AT_ONCE; i++) {
		snprintf(temp_buf, UNIVERSAL_LENGTH, FILENAME_GLASSDOOR_COMPANY_REVIEW, i);
		if (process_companies_review_pages(temp_buf)) {		// 0 - normal, 1 - nothing found on a page, 2 - shorter than normal page found
			flag = 1;
			break;
		}
	}

	fprintf(stdout, DIVIDER_50);
	if (flag) {
		return 1;
	}
	else {
		return 0;
	}
}

char* get_company_base_link(int company_line_number) {
	FILE* fp_read = fopen(FILENAME_GLASSDOOR_COMPANIES_LIST, "r");

	char *buffer = (char*)malloc(UNIVERSAL_LENGTH * sizeof(char));
	int current_line = 0;

	for (int i = 0; i < company_line_number; i++) {
		fgets(buffer, UNIVERSAL_LENGTH, fp_read);
	}
	fclose(fp_read);

	return buffer;
}

// Transfer info from purgatory to final, increment final ID
u_int ascend_file(int main_page_no) {
	FILE* fp_source, * fp_dest;
	char final_output_path[UNIVERSAL_LENGTH];
	snprintf(final_output_path, UNIVERSAL_LENGTH, FILENAME_GLASSDOOR_FINAL_OUTPUT, main_page_no);

	fp_dest = fopen(final_output_path, "a");
	if (fp_dest == NULL) {
		fprintf(stderr, "[!] Could not open file %s.\n", final_output_path);
		return -1;
	}

	fp_source = fopen(FILENAME_GLASSDOOR_PURGATORY_OUTPUT, "r");
	if (fp_source == NULL) {
		fprintf(stderr, "[!] Could not open file %s.\n", FILENAME_GLASSDOOR_PURGATORY_OUTPUT);
		return -1;
	}
	else {
		char c;
		while ((c = fgetc(fp_source)) != EOF) {
			fputc(c, fp_dest);
		}

		fclose(fp_source);
		fclose(fp_dest);
	}

	// +1000
	access_ascendency_file(1);

	return 0;
}

// Parameters: 
// 0 - return value in file
// 1 - add 1000 to value. return value before add.
// >1 - override file value to parameter. return value after change.
// 0 and 1 used to matter, but now only >1 is used
u_int access_ascendency_file(int is_increment) {
	char buffer[16];
	int stored_value = 0;

	// >1
	if (is_increment > 1) {
		sprintf(buffer, "%d\0", is_increment);

		FILE* fp = fopen(FINAL_ID_CONTAINING_FILE, "w");
		if (fp == NULL) {
			fprintf(stderr, "[!] Could not open file %s.\n", FINAL_ID_CONTAINING_FILE);
			exit(-1);
		}
		else {
			fwrite(buffer, 1, strlen(buffer), fp);

			fclose(fp);
		}

		return is_increment;
	}

	// 0
	FILE* fp = fopen(FINAL_ID_CONTAINING_FILE, "r");
	if (fp == NULL) {
		fprintf(stderr, "[!] Could not open file %s.\n", FINAL_ID_CONTAINING_FILE);
		exit(-1);
	}
	fgets(buffer, sizeof(buffer), fp);

	stored_value = atoi(buffer);
	fclose(fp);

	// 1
	if (is_increment) {
		stored_value += REVIEWS_TOTAL_PER_COMPANY;
		sprintf(buffer, "%d\0", stored_value);

		FILE* fp = fopen(FINAL_ID_CONTAINING_FILE, "w");
		if (fp == NULL) {
			fprintf(stderr, "[!] Could not open file %s.\n", FINAL_ID_CONTAINING_FILE);
			exit(-1);
		}
		else {
			fwrite(buffer, 1, strlen(buffer), fp);

			fclose(fp);
		}
		
	}

	return stored_value;
}

// Called just before new files to be downloaded (cause the multi is in 'a' mode and i lazy clear there)
// and at end of program
u_int remove_temp_review_files() {
	for (int i = 0; i < REVIEW_PAGES_AT_ONCE; i++) {
		char remove_target[UNIVERSAL_LENGTH];
		snprintf(remove_target, UNIVERSAL_LENGTH, FILENAME_GLASSDOOR_COMPANY_REVIEW, i);
		remove(remove_target);
	}

	return 0;
}

u_int remove_purgatory_file() {
	remove(FILENAME_GLASSDOOR_PURGATORY_OUTPUT);

	return 0;
}

u_int remove_company_list() {
	remove(FILENAME_GLASSDOOR_COMPANIES);
	remove(FILENAME_GLASSDOOR_COMPANIES_LIST);

	return 0;
}