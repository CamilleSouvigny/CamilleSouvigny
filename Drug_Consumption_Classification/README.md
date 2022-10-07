# Drug Consumption study

This machine learning academic project is a study of the consumption frequency of 18 drugs with psychological tests for two thousands medical patients. 
The goal is to detect clusters of drugs with similar consumption patterns and to predict drugs clusters most likely to be used by patients. 

For each patient, we have its frequency of consumption for 18 drugs with seven ordered modalities (never, in last 10 years, 1 year ...). We also have personal informations (sex, age, nationality ...) and results of psychological tests.

Project steps : 
- Preprocessing : data cleaning, study of variable distribution and correlation. Transformation of ordered categorical variables to best fit the the distributions.
- Search of similar drugs using clustering methods : use HAC, k-menoids in  Hamming distance and HAC, K-means methods after applying factor analysis to get a euclidean space.

![image](Images_readme/clustering_results.png)


-
