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

/* This version never exceeds the maximum, so no checks for
 * if(transfers < NUM_URLS)
 *      add_transfer(cm, transfers++, &left);
 */
int get_review_pages(char* review_page_link, int review_page_number) {
	CURLM* m_curl = curl_multi_init();

	if (!m_curl) {
		fprintf(stderr, "[!] Failed Initializing Curl. (get_review_pages)\n");
		return 1;
	}

	fprintf(stdout, "[i] Downloading review pages...\n");
	fprintf(stdout, DIVIDER_25);

	CURLMsg* msg;
	unsigned int transfers = 0;
	int msgs_left = -1;
	int left = 0;

	curl_multi_setopt(m_curl, CURLMOPT_MAXCONNECTS, (long)REVIEW_PAGES_AT_ONCE);	// Set maximum parallel count
	// CloudFlare bypass
	curl_multi_setopt(m_curl, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:109.0) Gecko/20100101 Firefox/111.0");


	// PLEASE FREE ME
	webpage_callback* all_callbacks = (webpage_callback*)malloc(sizeof(webpage_callback) * REVIEW_PAGES_AT_ONCE);

	// Generate proper review page link
	for (int i = 0; i < REVIEW_PAGES_AT_ONCE; i++) {
		int link_page_number = ((review_page_number - 1) * 10) + i + 1;

		all_callbacks[i].bytes = 0;

		// e.g. <URL_GLASSDOOR_BASE>/Reviews/Company-E1234.htm is the OG
		// get length, -4 (.htm)
		// add _P<num>.htm to it instead
		char final_review_page_link[UNIVERSAL_LENGTH] = URL_GLASSDOOR_BASE;
		char temp_buf[UNIVERSAL_LENGTH];										// This is what happens when I don't use malloc for 1 second
		strcpy(temp_buf, review_page_link);
		temp_buf[strlen(review_page_link) - 5] = '\0';
		char to_append[16] = "";
		if (link_page_number == 1) {
			snprintf(to_append, 16, ".htm\0");
		}
		else {
			snprintf(to_append, 16, "_P%d.htm\0", link_page_number);
		}

		strcat(temp_buf, to_append);
		strcat(final_review_page_link, temp_buf);
		strcpy(all_callbacks[i].webpage_link, final_review_page_link);

		printf("%s\n", all_callbacks[i].webpage_link);

		// Generate filename_path
		char company_review_file_path[UNIVERSAL_LENGTH];
		snprintf(company_review_file_path, UNIVERSAL_LENGTH, FILENAME_GLASSDOOR_COMPANY_REVIEW, i);
		strncpy(all_callbacks[i].filename_path, company_review_file_path, UNIVERSAL_LENGTH);
	}

	for (transfers = 0; transfers < REVIEW_PAGES_AT_ONCE; transfers++) {
		// pointer arithmetic offset
		review_pages_transfer(m_curl, transfers, all_callbacks + transfers, &left);
	}

	do {
		int still_alive = 1;
		curl_multi_perform(m_curl, &still_alive);

		while ((msg = curl_multi_info_read(m_curl, &msgs_left))) {
			if (msg->msg == CURLMSG_DONE) {
				char* url;
				CURL* e = msg->easy_handle;
				curl_easy_getinfo(msg->easy_handle, CURLINFO_PRIVATE, &url);
				if (msg->data.result) {
					fprintf(stderr, "[!] Could Not Fetch!\n[!] Error : Code %d - %s\n", msg->data.result, curl_easy_strerror(msg->data.result));
					curl_multi_cleanup(m_curl);
					return 2;
				}

				// msg->data.result for exit code (0), curl_easy_strerror(msg->data.result) for exit code meaning (no error)
				fprintf(stdout, "[>] Review page downloaded.\n");
				/*fprintf(stdout, "[>] Review page downloaded: <%s>\n", all_callbacks[url_to_review_page(all_callbacks, url)].webpage_link);*/
				curl_multi_remove_handle(m_curl, e);
				curl_easy_cleanup(e);
				left--;
			}
			else {
				//fprintf(stderr, "[!] Could Not Fetch songs!\n[!] CURL Error Code: %d\n", msg->msg);
				curl_multi_cleanup(m_curl);
				return 3;
			}
		}
		if (left)
			curl_multi_wait(m_curl, NULL, 0, 1000, NULL);

	} while (left);

	free(all_callbacks);
	curl_multi_cleanup(m_curl);

	return 0;
}

void review_pages_transfer(CURLM* m_curl, int transfers, webpage_callback* callback, int* left)
{
	CURL* sdt_curl = curl_easy_init();

	curl_easy_setopt(sdt_curl, CURLOPT_WRITEFUNCTION, store_webpage);
	curl_easy_setopt(sdt_curl, CURLOPT_WRITEDATA, (void*)callback);
	curl_easy_setopt(sdt_curl, CURLOPT_URL, callback->webpage_link);
	curl_easy_setopt(sdt_curl, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:109.0) Gecko/20100101 Firefox/111.0");

	curl_multi_add_handle(m_curl, sdt_curl);

	(*left)++;
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

u_int url_to_review_page(webpage_callback* all_callbacks, char* url) {
	for (int i = 0; i < REVIEW_PAGES_AT_ONCE; i++) {

		if (!strcmp(all_callbacks[i].webpage_link, url)) {
			return i;
		}
	}

	return 0;
}