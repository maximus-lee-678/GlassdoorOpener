#include "../headers/file_read_writer.h"

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

u_int process_companies_review_pages() {
	review_struct reviews[10];

	char chunk[UNIVERSAL_LENGTH];
	size_t len = sizeof(chunk);		// Store the chunks of text into a line buffer
	char* line = malloc(len);
	mallocChecker(line);
	line[0] = '\0';					// Zeroise the string
	char* examiner = NULL;

	for (int iterator = 0; iterator < COMPANIES_PER_PAGE; iterator++) {
		char temp_buf[UNIVERSAL_LENGTH] = "";
		snprintf(temp_buf, UNIVERSAL_LENGTH, FILENAME_GLASSDOOR_COMPANY_REVIEW, iterator);

		FILE* fp_read = fopen(temp_buf, "r");
		FILE* fp_write = fopen(FILENAME_GLASSDOOR_OUTPUT, "a");

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
				// Look for start of massive block of text containing REVIEWS_PER_PAGE(10) counts of 
				examiner = strstr(line, HTML_GET_REVIEWBODY_START);

				if (examiner) {
					flag = 1;
					break;
				}

				// "Empty" the line buffer
				line[0] = '\0';
			}
		}

		size_t str_length = 0;
		char temp_str_buf[64];
		int temp_int_buf = 0;

		// Get "reviewDateTime"
		examiner = strstr(line, HTML_GET_REVIEWBODY_DATE);
		examiner += strlen(HTML_GET_REVIEWBODY_DATE);							// Skip string itself
		examiner += 3;															// Skip ":"
		str_length = (int)(strstr(examiner, "\"") - examiner);					// Measure length until before "
		strncpy(reviews[iterator].review_date_time, examiner, str_length);		// Copy
		reviews[iterator].review_date_time[str_length] = '\0';					// Provide a terminator

		// Get "reviewDateTime"
		examiner = strstr(line, HTML_GET_REVIEWBODY_RATING_OVERALL);
		examiner += strlen(HTML_GET_REVIEWBODY_RATING_OVERALL);					// Skip string itself
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
		examiner = strstr(line, HTML_GET_REVIEWBODY_RATING_CEO);
		examiner += strlen(HTML_GET_REVIEWBODY_RATING_CEO);						// Skip string itself
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
		examiner = strstr(line, HTML_GET_REVIEWBODY_RATING_OUTLOOK);
		examiner += strlen(HTML_GET_REVIEWBODY_RATING_OUTLOOK);					// Skip string itself
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
		examiner = strstr(line, HTML_GET_REVIEWBODY_RATING_LIFE_BAL);
		examiner += strlen(HTML_GET_REVIEWBODY_RATING_LIFE_BAL);				// Skip string itself
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
		examiner = strstr(line, HTML_GET_REVIEWBODY_RATING_CULT_VAL);
		examiner += strlen(HTML_GET_REVIEWBODY_RATING_CULT_VAL);				// Skip string itself
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
		examiner = strstr(line, HTML_GET_REVIEWBODY_RATING_DIVR_INCLU);
		examiner += strlen(HTML_GET_REVIEWBODY_RATING_DIVR_INCLU);				// Skip string itself
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
		examiner = strstr(line, HTML_GET_REVIEWBODY_RATING_SNR_LEAD);
		examiner += strlen(HTML_GET_REVIEWBODY_RATING_SNR_LEAD);				// Skip string itself
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
		examiner = strstr(line, HTML_GET_REVIEWBODY_RATING_RECM_FRND);
		examiner += strlen(HTML_GET_REVIEWBODY_RATING_RECM_FRND);				// Skip string itself
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
		examiner = strstr(line, HTML_GET_REVIEWBODY_RATING_CAREER);
		examiner += strlen(HTML_GET_REVIEWBODY_RATING_CAREER);					// Skip string itself
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
		examiner = strstr(line, HTML_GET_REVIEWBODY_RATING_COMP_BENE);
		examiner += strlen(HTML_GET_REVIEWBODY_RATING_COMP_BENE);					// Skip string itself
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

		

		fclose(fp_read);
		fclose(fp_write);
	}
		
	free(line);

	return 0;
}