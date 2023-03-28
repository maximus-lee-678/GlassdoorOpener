#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <curl/curl.h>
#include <direct.h>
#include <conio.h>
#include <errno.h>

#define COMPANIES_PER_PAGE 10
#define REVIEWS_PER_PAGE 10

#define UNIVERSAL_LENGTH 512
#define REVIEW_LENGTH 2048
#define URL_GLASSDOOR_BASE "https://www.glassdoor.sg"

#define TEMP_DIRECTORY "./temp/"
#define PERM_DIRECTORY "./out/"
#define FILENAME_GLASSDOOR_COMPANIES "./temp/companies_page.txt"
#define FILENAME_GLASSDOOR_COMPANIES_LIST "./temp/companies_list.txt"
#define FILENAME_GLASSDOOR_COMPANY_REVIEW "./temp/company_review_%d.txt"
#define FILENAME_GLASSDOOR_OUTPUT "./out/reviews.tsv"


#define DIVIDER_25 "=========================\n"
#define DIVIDER_50 "==================================================\n"

typedef struct {
	int bytes;
	char filename_path[UNIVERSAL_LENGTH];
}webpage_callback;

typedef struct {
	char company[UNIVERSAL_LENGTH];
	int rating_overall;						// stars given
	// POSITIVE, NEGATIVE, NO_OPINION, null
	int rating_recommend_to_friend;			// Recommend
	int rating_ceo;							// CEO Approval
	int rating_business_outlook;			// Business Outlook
	// optional fields
	int rating_work_life_balance;			// Work/Life Balance
	int rating_culture_and_values;			// Culture & Values
	int rating_diversity_and_inclusion;		// Diversity and Inclusion
	int rating_career_opportunities;		// Career Opportunities
	int rating_compensation_and_benefits;	// Compensation and Benefits
	int rating_senior_leadership;			// Senior Management

	int is_current_job;
	int length_of_employment;				// 0: no info, 1: <1 year, 2: >1 year, 3: >2 years, ...
	char review_date_time[64];
	char position[UNIVERSAL_LENGTH];		
	char country[UNIVERSAL_LENGTH];
	
	char review_summary[UNIVERSAL_LENGTH];	// Review Title
	char pros[REVIEW_LENGTH];
	char cons[REVIEW_LENGTH];
	char advice[REVIEW_LENGTH];				// Advice to Management
}review_struct;

void mallocChecker(void*);