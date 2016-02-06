Udemy course: Learn to Code in C++ by Developing Your First Game in Unreal
(with Ben Tristem)
https://www.udemy.com/unrealcourse/learn

Section 2 - Bulls and Cows game - Final beta
for comment, suggestions and plagiarising

Additional features beyond the course include:
- loading a list of isograms from isograms.txt and allowing the game to select a random word fitting the user's selection of word-length
- additional multi-game statistics

IMPORTANT:
- Update the ISOGRAM_FILE constant in main.cpp to point to your own copy of the isograms.txt file

Note:
- The isograms.txt file was generated from the list of 5000 most common English words provided for 
free from http://www.wordfrequency.info/intro.asp

To recreate this list yourself:
- Go to http://www.wordfrequency.info/free.asp
- Sign-up (free) and get their list of the 5000 most common words in the English language
- Copy and paste the table into Microsoft Excel
- Delete all columns except the actual words and so these are in column A
- Enter a formula in column B: =LOWER(RIGHT(A1,LEN(A1)-3))
  (This is to remove the 3-leading non-breaking spaces before each word and also convert all words to lowercase)
- In column C add another formula: =LEN(B1)
- Follow the instructions in the link to add another formula in column D which reports whether the word in column B is an isogram.
    http://www.mrexcel.com/forum/excel-questions/546103-isogram-testing.html
    (Use either the in-line formula or the VBA code; I used the VBA code and it worked fine)
- Sort by Column C to sort-by word length, then by Column D to isolate the isograms

I then copied the words that were isograms and length of 3 or more to a new document and saved this as a list (remove the n't entry!)
There you have it: a list of isograms drawn from the most common 5000 words in the English language! :)
