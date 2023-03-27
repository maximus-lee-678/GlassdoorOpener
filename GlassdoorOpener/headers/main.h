#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <direct.h>
#include <conio.h>
#include <errno.h>

#define UNIVERSAL_LENGTH 512
#define URL_GLASSDOOR_BASE "https://www.glassdoor.sg"

#define TEMP_DIRECTORY "./temp/"
#define PERM_DIRECTORY "./out/"
#define FILENAME_GLASSDOOR_COMPANIES "./temp/companies_page.txt"
#define FILENAME_GLASSDOOR_COMPANIES_LIST "./temp/companies_list.txt"
#define FILENAME_GLASSDOOR_COMPANY_REVIEW "./temp/company_review"


#define DIVIDER_25 "=========================\n"
#define DIVIDER_50 "==================================================\n"

typedef struct {
	int bytes;
	char filename_path[UNIVERSAL_LENGTH];
}webpage_callback;

void mallocChecker(void*);