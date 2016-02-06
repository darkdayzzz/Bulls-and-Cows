/*
The game logic (no view code or direct user interaction)

*/

#pragma once
#include <string>
#include <deque>
#include <map>

// to make syntax Unreal-friendly
#define TMap std::map
using FString = std::string;
using int32 = int;


/*
structure to hold dictionary of isograms at each word-length
I use this instead of "TMap<int32, std::deque<FString>> MasterWordList" in the 
class below purely for legibility
*/
struct FWordList
{
	std::deque<FString> WordList;
};


// enum for returning Guess validity
enum class EGuessStatus
{
	Invalid,
	OK,
	Not_Isogram,
	Wrong_Length,
	Not_Lowercase,
	Not_Alpha
};


// enum for returning check on Word Length validity
enum class EWordLengthStatus
{
	Invalid,
	OK,
	Not_Number,
	Out_Of_Range
};


// enum for returning Isogram File Read status
enum class EFileReadStatus
{
	OK,
	File_Not_Found,
	File_Not_Opened,
	Invalid_Content	
};


/* 
structure for returning results from check on whether 
user input is valid number of letters
one string (default to invalid), one integer (zeroed)
*/
struct FWordLength
{
	EWordLengthStatus Status = EWordLengthStatus::Invalid;
	int32 Length = 0;
};


// structure for reporting results of user guess at isogram
// two integers (default zero)
struct FBullCowCount 
{
	int32 Bulls = 0;
	int32 Cows = 0;
};


// structure for recording results of games
// three integers (default zero)
struct FGameStats
{
	int32 TotalGames = 0;
	int32 GamesWon = 0;
	int32 CurrentWinningStreak = 0;
	int32 CurrentLosingStreak = 0;
	int32 WinningStreak = 0;
	int32 LosingStreak = 0;
	bool bWonLastGame = false;
};


class FBullCowGame 
{
public:
	FBullCowGame(); // constructor

	// public getters/setters
	int32 GetCurrentTry() const;
	int32 GetMinWordLength() const;
	int32 GetMaxWordLength() const;
	int32 GetHiddenWordLength() const;
	bool GetIsGameWon() const;
	FGameStats GetGameStats() const;
	void SetHiddenWord(int32 NumberOfLetters);
	int32 GetMaxTries();

	// public methods
	void Reset();
	EFileReadStatus LoadWordList(FString Filename);
	FWordLength IsValidWordLength(FString Word) const;
	EGuessStatus CheckGuessValidity(FString) const;
	FBullCowCount SubmitValidGuess(FString);
	int32 GetDictionarySize();
	void UpdateTotalGames();

private:
	// private constants/variables
	const int32 ABSOLUTE_MIN_NUMBER_OF_LETTERS = 3; // minimum length of Isogram that this program can handle
	const int32 ABSOLUTE_MAX_NUMBER_OF_LETTERS = 8; // maximum length of Isogram that this program can handle
	int32 MinNumberOfLetters = ABSOLUTE_MIN_NUMBER_OF_LETTERS; // may change depending on file being read
	int32 MaxNumberOfLetters = ABSOLUTE_MAX_NUMBER_OF_LETTERS; // may change depending on file being read
	int32 MyCurrentTry;
	FString MyHiddenWord;
	bool bMyGameWon;
	FGameStats GameStats;

	// store list of isograms from 5000 most common English words
	// see https://www.udemy.com/course/657932/activities/?ids=3614612
	TMap<int32, FWordList> MasterWordList;

	// private methods
	int32 GetRandomNumber(int32 DictionarySize) const;
	bool IsIsogram(FString) const;
	bool IsLowercase(FString) const;
	bool IsInteger(FString Word) const;
};
