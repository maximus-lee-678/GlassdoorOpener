import pandas as pd
import glob
import html

dfList = []
names = ['company', 'rating_overall', 'rating_recommend_to_friend', 'rating_ceo', 'rating_business_outlook', 'rating_work_life_balance', 'rating_culture_and_values', 'rating_diversity_and_inclusion', 'rating_career_opportunities', 'rating_compensation_and_benefits', 'rating_senior_leadership', 'is_current_job', 'length_of_employment', 'review_date_time', 'position', 'country', 'review_summary', 'pros', 'cons', 'advice_to_management']
for file_name in glob.glob('./*.tsv'):
    print(file_name)
    df = pd.read_table(file_name, header=None, on_bad_lines='warn', names=names)
    df['company'] = df['company'].map(lambda x: html.unescape(x))
    df['position'] = df['position'].map(lambda x: html.unescape(x) if pd.notna(x) else x)
    df['review_summary'] = df['review_summary'].map(lambda x: html.unescape(x) if pd.notna(x) else x)
    df['pros'] = df['pros'].map(lambda x: html.unescape(x) if pd.notna(x) else x)
    df['cons'] = df['cons'].map(lambda x: html.unescape(x) if pd.notna(x) else x)
    df['advice_to_management'] = df['advice_to_management'].map(lambda x: html.unescape(x) if pd.notna(x) else x)
    df = df.replace(r"\\r\\n", ". ", regex=True)
    df = df.replace(r"\\n", ". ", regex=True)
    df = df.replace(r"\\t", " ", regex=True)
    df = df.replace(r"\\u002F", "/", regex=True)
    df = df.replace(r"\\u003C", "<", regex=True)
    df = df.replace(r"\\u003E", ">", regex=True)
    df = df.replace(r"\\u2028", "", regex=True)
    df = df.replace(r"\\u0010", "", regex=True)
    df = df.replace(r"\\\"", "'", regex=True)
    df = df.fillna("null")
    dfList.append(df)

result = pd.concat(dfList).drop_duplicates().reset_index(drop=True)
result.to_csv("combined.tsv", sep = "\t", header=None)

