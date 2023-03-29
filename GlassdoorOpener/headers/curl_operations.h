#pragma once

#include "main.h"

// base
int get_webpage(char*, char*);
int store_webpage(char*, int, int, void*);

// replicating menace
int get_review_pages(char*, int);
void review_pages_transfer(CURLM*, int, webpage_callback*, int*);
u_int url_to_review_page(webpage_callback*, char*);
u_int remove_temp_review_files();
