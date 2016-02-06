/*
Main game logic for the Bull-Cow game
Excludes user input/output
Includes:
- Reading text file of words (assumes they are isograms)
- Setting isogram (MyHiddenWord) and difficulty (MaxTries)
- Validating user guesses
- 
*/

#pragma once
#include "FBullCowGame.h"
#include <map>
#include <cstdlib>
#include <ctime>
#include <fstream>

// to make syntax Unreal-friendly
#define TMap std::map

FBullCowGame::FBullCowGame() { Reset(); }; // default constructor

// getters
int32 FBullCowGame::GetCurrentTry() const { return MyCurrentTry; };
int32 FBullCowGame::GetHiddenWordLength() const { return MyHiddenWord.length(); };
bool FBullCowGame::GetIsGameWon() const { return bMyGameWon; };
int32 FBullCowGame::GetMinWordLength() const { return MinNumberOfLetters; };
int32 FBullCowGame::GetMaxWordLength() const { return MaxNumberOfLetters; };
FGameStats FBullCowGame::GetGameStats() const { return GameStats; };


// methods

EFileReadStatus FBullCowGame::LoadWordList(FString Filename)
/*
Load list of words from filename provided (assumes one isogram per line)
Adds each word to a Deque for each word-length of a TMap
Using a TMap for the master structure so I can get a dictionary of appropriate isograms using MasterWordList[NumberOfLetters].WordList
Using a Deque for the WordList so I can add an unknown number of words to the list and count them and return them by index
Returns status of various types if there are problems reading the file or the content is unexpected
*/
{
	FString ReadLine;
	int32 LineLength;
	int32 MinLineLength = 0;
	int32 MaxLineLength = 0;
	try {
		std::ifstream myfile(Filename);
		if (myfile.is_open())
		{
			// file is found and can be opened - start reading
			while (getline(myfile, ReadLine))
			{
				LineLength = ReadLine.length();
				if (LineLength >= ABSOLUTE_MIN_NUMBER_OF_LETTERS && LineLength <= ABSOLUTE_MAX_NUMBER_OF_LETTERS) 
				{
					// Add the word to the appropriate Deque of the appropriate TMap
					// ie. MasterWordList { { 3, {"the","and","but",...}}, { 4, {"plan", "done", ...}}, ... etc } 
					MasterWordList[LineLength].WordList.push_back(ReadLine);					
					if (MinLineLength == 0 || LineLength < MinLineLength) { MinLineLength = LineLength; };
					if (MaxLineLength == 0 || LineLength > MaxLineLength) { MaxLineLength = LineLength; };
				};
			};
			myfile.close();
			if (MinLineLength < ABSOLUTE_MIN_NUMBER_OF_LETTERS || MaxLineLength >> ABSOLUTE_MAX_NUMBER_OF_LETTERS)
			{
				// file has been read but only words are too short or too long to be used so return error
				return EFileReadStatus::Invalid_Content;
			}
			else
			{
				// file has been read and words are just right
				MinNumberOfLetters = MinLineLength;
				MaxNumberOfLetters = MaxLineLength;
				return EFileReadStatus::OK;
			};
		}
		else
		{
			// file is found but cannot be opened so return error
			return EFileReadStatus::File_Not_Opened;
		};
	}
	catch (...) {
		// file cannot be found or some other error
		return EFileReadStatus::File_Not_Found;
	};
}; // LoadWordList


FWordLength FBullCowGame::IsValidWordLength(FString Word) const
/* 
Check if user request for word length is valid
- is a number
- is between minimum and maximum numbers
Returns error status if word length fails any of the checks or returns OK and the word length as an integer
*/
{
	FWordLength ReturnStatus;
	if (Word.length() < 1) 
	{
		// treat Word length of 0 as not valid number so return error
		ReturnStatus.Status = EWordLengthStatus::Not_Number;
	}
	else if (!IsInteger(Word)) 
	{
		// string cannot be converted to number so return error
		ReturnStatus.Status = EWordLengthStatus::Not_Number;
	}
	else 
	{
		// Word is an integer but is it within the range of allowed numbers?
		int32 Number = stoi(Word);
		if (Number<MinNumberOfLetters || Number>MaxNumberOfLetters) 
		{
			// Word is an integer but is outside the valid range of between Min and Max Number of Letters so return error
			ReturnStatus.Status = EWordLengthStatus::Out_Of_Range;
		}
		else 
		{
			// We know the Word is an integer and within the valid range of between Min and Max Number of Letters so return OK
			ReturnStatus.Status = EWordLengthStatus::OK;
			ReturnStatus.Length = Number;
		}
	}
	return ReturnStatus;
}; // IsValidWordLength


void FBullCowGame::SetHiddenWord(int32 NumberOfLetters) 
/* 
Sets the hidden word as a random word from the dictionary of isograms with that word length
*/
{
	int32 Number = GetRandomNumber(MasterWordList[NumberOfLetters].WordList.size());
	MyHiddenWord = MasterWordList[NumberOfLetters].WordList[Number];
	return;
}; // SetHiddenWord


int32 FBullCowGame::GetMaxTries()
/*
Sets the difficulty level based on the word length
*/
{
	TMap<int32, int32> WordLengthToMaxTries{ { 3,5 },{ 4,8 },{ 5,10 },{ 6,15 },{ 7, 18 },{ 8, 22} };
	return WordLengthToMaxTries[MyHiddenWord.length()];
};  // GetMaxTries


EGuessStatus FBullCowGame::CheckGuessValidity(FString ThisGuess) const
/*
Checks the user guess for validity
- checks if an isogram
- checks if all lower-case
- checks if correct length
Returns error code if fails any of those checks
*/
{	
	if (!IsIsogram(ThisGuess)) 
	{ // if the guess is not an isogram
		return EGuessStatus::Not_Isogram;
	} 
	else if (!IsLowercase(ThisGuess)) 
	{ // if the guess is not lower-case
		return EGuessStatus::Not_Lowercase;
	} 
	else if (ThisGuess.length() != GetHiddenWordLength()) 
	{ // if the guess is different length from hidden word then return error
		return EGuessStatus::Wrong_Length;	
	};
	return EGuessStatus::OK; // TODO Later make actual error
}; // CheckGuessValidity


void FBullCowGame::Reset()
{
	MyCurrentTry = 1;
	bMyGameWon = false;
	return;
}; // Reset


FBullCowCount FBullCowGame::SubmitValidGuess(FString ThisGuess)
/*
Receives valid guess, increments turn and returns count of Bulls or Cows
*/
{
	MyCurrentTry++;
	FBullCowCount MyBullCowCount;

	// loop through all letters in the guess
	int32 WordLen = MyHiddenWord.length();
	for (int32 GuessChr = 0; GuessChr < WordLen; GuessChr++)
	{
		// compare letters against the hidden word
		for (int32 HiddenWordChr = 0; HiddenWordChr < WordLen; HiddenWordChr++) 
		{
			// if there is any match then
			if (ThisGuess[HiddenWordChr] == MyHiddenWord[GuessChr]) 
			{
				if (HiddenWordChr == GuessChr) 
				{
					// increment bulls if they're in the same place
					MyBullCowCount.Bulls++;
				}
				else 
				{
					// else increment cows if they're in the word but not in the same place
					MyBullCowCount.Cows++;
				};
			}
			else 
			{
				// else no match; no bulls, no cows
			};
		}; // next hiddenword
	}; // next guess
	// if all bulls then set game as won!
	if (MyBullCowCount.Bulls == WordLen) { bMyGameWon = true; };
	return MyBullCowCount;
}; // SubmitValidGuess


void FBullCowGame::UpdateTotalGames()
{
	GameStats.TotalGames++;
	bool bGameWon = GetIsGameWon();
	if (bGameWon) {
		GameStats.GamesWon++;
	};
	// calculate current and best/worst winning/losing streaks
	if (GameStats.bWonLastGame && bGameWon) 
	{ 
		GameStats.CurrentWinningStreak++;
		GameStats.CurrentLosingStreak = 0;
		if (GameStats.CurrentWinningStreak > GameStats.WinningStreak) 
		{
			GameStats.WinningStreak = GameStats.CurrentWinningStreak;
		};
	};
	if (!GameStats.bWonLastGame && !bGameWon) 
	{
		GameStats.CurrentLosingStreak++;
		GameStats.CurrentWinningStreak = 0;
		if (GameStats.CurrentLosingStreak > GameStats.LosingStreak)
		{
			GameStats.LosingStreak = GameStats.CurrentLosingStreak;
		};
	};
	GameStats.bWonLastGame = bGameWon;
	return;
}; // UpdateTotalGames


bool FBullCowGame::IsIsogram(FString Word) const
/*
private function to check if Word is valid isogram
*/
{
	// treat 0 and 1 length word as isogram
	if (Word.length() <= 1) { return true; };
	// setup map
	TMap<char, bool> LetterSeen;

	// loop through letters within Word
	for (auto letter : Word) {
		letter = tolower(letter); // ensure letter is lower-case
		// check if this letter in map is already present (true)
		if (LetterSeen[letter]) {
			// if already mapped then immediately return that this word isn't an isogram
			return false;
		} else {
			// otherwise update the map for this letter to true
			LetterSeen[letter] = true;
		};
	}
	// when loop finished then we know all letters only mapped once so the word is an isogram
	return true;
};  // isIsogram


bool FBullCowGame::IsLowercase(FString Word) const
/*
private function to check if Word is lower-case
*/
{
	// treat 0 length word as lowercase
	if (Word.length() < 1) { return true; };
	
	// loop through letters within Word
	for (auto letter : Word) {
		// check if this character isn't lowercase
		if (!islower(letter)) {
			return false; // immediately return that word isn't completely lower-case		
		};
	}
	// when loop finished then we know all letters were lowercase 
	// so we can return that the word is in lowercase
	return true;
}; // IsLowercase


bool FBullCowGame::IsInteger(FString Word) const
/*
private function to check if Word is valid integer
*/
{
	int32 Number;
	try {
		Number = stoi(Word);
		return true;
	}
	catch (...)
	{
		return false;
	};
}; // IsInteger


int32 FBullCowGame::GetRandomNumber(int32 DictionarySize) const
/*
private function to return properly random number
Thankyou: Alex Rivera Rivera https://www.facebook.com/alexarmystrong
*/
{
	srand(time(NULL));
	int32 number;
	for (int32 i = 0; i <= DictionarySize; i++)
	{
		number = rand() % DictionarySize;
	}
	return number;
}; // GetRandomNumber


int32 FBullCowGame::GetDictionarySize()
/*
Getter for Dictionary size for current hidden word length that includes check that hidden word has been set
*/
{
	int32 NumberOfLetters = MyHiddenWord.length();
	if (NumberOfLetters >= MinNumberOfLetters && NumberOfLetters <= MaxNumberOfLetters) {
		return MasterWordList[NumberOfLetters].WordList.size();
	}
	else 
	{
		return 0;
	};
}; // GetDictionarySize
