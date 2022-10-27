# Business directory web scrapping

This project scrapes a business directory website and saves in a file personal and professional datas.

Using BeautifulSoup and fake_useragent, I scrapped all pages, executing sessions at random times not to be ejected from the site.

Once the paged is parsed, I collect datas and check for missing values and datas exceptions (multiple first names, punctuation mark).

The final datas are stored in a excel file.