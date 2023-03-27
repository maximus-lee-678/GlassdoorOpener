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
			examiner = strstr(line, "\"cell-Reviews-url\" href=\"");
			while (iteration < 10 && examiner != NULL) {
				size_t str_length = 0;
				char temp[UNIVERSAL_LENGTH];

				examiner += strlen("\"cell-Reviews-url\" href=\"");			// Seek to start of review URL
				str_length = (int)(strstr(examiner, "\"") - examiner);		// Measure length (exampleURL")
				strncpy(temp, examiner, str_length);						// Copy URL
				temp[str_length] = '\n';									// Provide a newline
				temp[str_length + 1] = '\0';									// Provide a terminator
				fprintf(fp_write, "%s", temp);
				iteration++;
				examiner = strstr(examiner, "\"cell-Reviews-url\" href=\"");
			}

			// "Empty" the line buffer
			line[0] = '\0';

			if (iteration == 10)
				break;
		}
	}

	fclose(fp_read);
	fclose(fp_write);
	free(line);

	return 0;
}

u_int get_companies_review_pages() {
	FILE* fp_read = fopen(FILENAME_GLASSDOOR_COMPANIES_LIST, "r");
	char chunk[UNIVERSAL_LENGTH];

	int iterator = 0;
	while (fgets(chunk, sizeof(chunk), fp_read) != NULL) {
		char webpage_url[UNIVERSAL_LENGTH] = URL_GLASSDOOR_BASE;
		chunk[strlen(chunk) - 1] = '\0';	// Remove \n read from file
		strcat(webpage_url, chunk);			// Combine with glassdoor URL

		char temp_buf[UNIVERSAL_LENGTH] = FILENAME_GLASSDOOR_COMPANY_REVIEW;
		char temp_iter[16];
		snprintf(temp_iter, 16, "_%d.txt", iterator);
		strcat(temp_buf, temp_iter);
		get_webpage(webpage_url, temp_buf);
		iterator++;
	}

	fclose(fp_read);

	return 0;
}