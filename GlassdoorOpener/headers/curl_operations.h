#pragma once

#include "main.h"

// base
int get_webpage(char*, char*);
int store_webpage(char*, int, int, void*);

// replicating menace
u_int get_companies_review_pages();
int get_review_pages(char*);
void review_pages_transfer(CURLM*, int, webpage_callback*, int*);
int store_webpage(char*, int, int, void*);
u_int url_to_review_page(webpage_callback*, char*);