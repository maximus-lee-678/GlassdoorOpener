#pragma once

#include "main.h"

#define USER_AGENT "Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:109.0) Gecko/20100101 Firefox/111.0"

// base
int get_webpage(char*, char*);
int store_webpage(char*, int, int, void*);

// replicating menace
int get_review_pages(char*, int);
void review_pages_transfer(CURLM*, int, webpage_callback*, int*);
