#pragma once

#include "main.h"
#include "curl_operations.h"

#define HTML_GET_REVIEWS "\"cell-Reviews-url\" href=\""

#define REVIEW_COMPANY_NAME "<title data-rh=\"true\">"
#define REVIEW_START "__typename\":\"EmployerReview"
#define REVIEW_DATE "reviewDateTime"
#define REVIEW_RATING_OVERALL "ratingOverall"
#define REVIEW_RATING_CEO "ratingCeo"
#define REVIEW_RATING_BUSI_OUTLOOK "ratingBusinessOutlook"
#define REVIEW_RATING_LIFE_BAL "ratingWorkLifeBalance"
#define REVIEW_RATING_CULT_VAL "ratingCultureAndValues"
#define REVIEW_RATING_DIVR_INCLU "ratingDiversityAndInclusion"
#define REVIEW_RATING_SNR_LEAD "ratingSeniorLeadership"
#define REVIEW_RATING_RECM_FRND "ratingRecommendToFriend"
#define REVIEW_RATING_CAREER "ratingCareerOpportunities"
#define REVIEW_RATING_COMP_BENE "ratingCompensationAndBenefits"
#define REVIEW_IS_CURRENT_JOB "isCurrentJob"
#define REVIEW_LENGTH_OF_EMPLOYMENT "lengthOfEmployment"
#define REVIEW_PROS "\"pros\""
#define REVIEW_CONS "\"cons\""
#define REVIEW_SUMMARY "\"summary\""
#define REVIEW_ADVICE_TO_MGMT "\"advice\""
#define REVIEW_POSITION_LOCATION_START "<span class=\"middle common__EiReviewDetailsStyle__newGrey\">"
#define REVIEW_LOCATION_START "in<!-- --> <span>"

u_int get_companies_list();
u_int process_companies_review_pages(char*);
u_int process_one_block();
