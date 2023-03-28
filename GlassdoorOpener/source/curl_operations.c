#include "../headers/curl_operations.h"

int get_webpage(char* webpage_link, char* filename_path) {
	CURL* curl = curl_easy_init();

	if (!curl) {
		fprintf(stderr, "[!] Failed Initializing Curl. (get_webpage)\n");
		curl_easy_cleanup(curl);
		return 1;
	}

	CURLcode result;
	curl_easy_setopt(curl, CURLOPT_URL, webpage_link);	// Set webpage URL

	FILE* initial_file;								// Create file
	initial_file = fopen(filename_path, "w");
	if (initial_file == NULL) {
		fprintf(stderr, "[!] Could not create file %s.\n", filename_path);
		curl_easy_cleanup(curl);
		return 2;
	}
	else {
		fclose(initial_file);
	}

	fprintf(stdout, DIVIDER_50);
	fprintf(stdout, "[i] Downloading webpage from <%s>\n", webpage_link);

	webpage_callback store_callback = { 0 , "" };
	strncpy(store_callback.filename_path, filename_path, UNIVERSAL_LENGTH);

	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, store_webpage);			// Callback function for storing webpage
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&store_callback);		// Callback argument

	// CloudFlare bypass
	struct curl_slist* list = NULL;
	list = curl_slist_append(list, "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:109.0) Gecko/20100101 Firefox/111.0");
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);

	result = curl_easy_perform(curl);
	
	if (result != CURLE_OK) {
		fprintf(stderr, "[!] Could Not Fetch Webpage %s\n[!] Error : %s\n", webpage_link, curl_easy_strerror(result));
		curl_easy_cleanup(curl);
		return 3;
	}
	else {
		fprintf(stdout, "[>] Webpage Size: %.2f KB\n", (float)store_callback.bytes / 1024);
	}

	curl_slist_free_all(list);	// free the list
	curl_easy_cleanup(curl);

	return 0;
}

int store_webpage(char* buffer, int itemsize, int n_items, void* userp) {
	webpage_callback* callback = (webpage_callback*)userp;

	int bytes = itemsize * n_items;

	callback->bytes += bytes;
	FILE* create_file;

	create_file = fopen(callback->filename_path, "a");
	if (create_file == NULL) {
		fprintf(stderr, "[!] Could not open file %s.\n", callback->filename_path);
		return -1;
	}
	else {
		for (int i = 0; i < bytes; i++) {
			fprintf(create_file, "%c", buffer[i]);
		}
		fclose(create_file);
	}

	return bytes;
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