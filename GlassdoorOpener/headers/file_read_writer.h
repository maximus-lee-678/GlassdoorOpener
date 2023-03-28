#pragma once

#include "main.h"
#include "curl_operations.h"

#define HTML_GET_REVIEWS "\"cell-Reviews-url\" href=\""

#define HTML_GET_REVIEWBODY_START "__typename\":\"EmployerReview"
#define HTML_GET_REVIEWBODY_DATE "reviewDateTime"
#define HTML_GET_REVIEWBODY_RATING_OVERALL "ratingOverall"
#define HTML_GET_REVIEWBODY_RATING_CEO "ratingCeo"
#define HTML_GET_REVIEWBODY_RATING_OUTLOOK "ratingBusinessOutlook"
#define HTML_GET_REVIEWBODY_RATING_LIFE_BAL "ratingWorkLifeBalance"
#define HTML_GET_REVIEWBODY_RATING_CULT_VAL "ratingCultureAndValues"
#define HTML_GET_REVIEWBODY_RATING_DIVR_INCLU "ratingDiversityAndInclusion"
#define HTML_GET_REVIEWBODY_RATING_SNR_LEAD "ratingSeniorLeadership"
#define HTML_GET_REVIEWBODY_RATING_RECM_FRND "ratingRecommendToFriend"
#define HTML_GET_REVIEWBODY_RATING_CAREER "ratingCareerOpportunities"
#define HTML_GET_REVIEWBODY_RATING_COMP_BENE "ratingCompensationAndBenefits"

u_int get_companies_list();
u_int process_companies_review_pages();