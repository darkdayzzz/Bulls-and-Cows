/*
This is the console executable that makes use of the FBullCowGame class
This acts as the View in MVC.

*/

#pragma once
#include <iostream>
#include <string>
#include "FBullCowGame.h"

// to make syntax Unreal-friendly
using FText = std::string;
using int32 = int;

FString ISOGRAM_FILE = "E:\\Documents\\Unreal Projects\\Udemy-UnrealCourse\\Section_02\\Bulls and Cows\\Debug\\isograms.txt";


// enum for returning Play Again result
enum class EGameReplayStatus
{
	Play_With_Same_Word,
	Play_With_Different_Word,
	Select_Different_Word_Length,
	Quit
};


// function prototypes
void PrintIntro();
void PlayTheGame();
bool LoadWordList();
int32 GetNumberOfLetters();
FText GetValidGuess();
EGameReplayStatus AskToPlayAgain();
void PrintThisGameSummary();
void PrintEndGameSummary();

// the game which we re-use
FBullCowGame BCGame;


// The entry-point for game
int main()
{
	int32 NumberOfGames = 0;
	int32 NumberOfLetters;
	EGameReplayStatus PlayAgainStatus;
	PrintIntro();
	if (LoadWordList()) {
		do 
		{ // loop from here if we want to replay with different word length
			NumberOfLetters = GetNumberOfLetters();
			do
			{ // loop from here if we want to replay with different word but same word length
				BCGame.SetHiddenWord(NumberOfLetters);
				do
				{ // loop from here if we want to replay with the same word
					NumberOfGames++;
					PlayTheGame();
					PrintThisGameSummary();
					PlayAgainStatus = AskToPlayAgain();
				} while (PlayAgainStatus == EGameReplayStatus::Play_With_Same_Word);

			} while (PlayAgainStatus == EGameReplayStatus::Play_With_Different_Word);

		} while (PlayAgainStatus == EGameReplayStatus::Select_Different_Word_Length);
		// quitting
		PrintEndGameSummary();
	};
	return 0;
}; // main


// functions

void PrintIntro()
/*
Some ASCII art and intro text for the user including simple definition and example of an Isogram
*/
{
	std::cout << "MM                   MM                                     \n";
	std::cout << "MMM+                8MM                                     \n";
	std::cout << "MMMMMM            :MMM                        MM,     ZMM   \n";
	std::cout << "ZMMM+MMMM+       MMMM                         MMMO  MMMMM   \n";
	std::cout << "   MMM  MMMMMMMMMMMN                            MMMMMMMZ    \n";
	std::cout << "    MMMMM   MMMMMMMMM                        ZMMMZMZ, MMMMMM\n";
	std::cout << "      $MMD  MMM    MMMMMMM         MMMMMMMMMMMMM         MMM\n";
	std::cout << "777MMMMDI               I7MMM~  7MMMIIIIIII+                \n";
	std::cout << "MMM?                       MMM +MM                          \n";
	std::cout << "                           ZMM +M+                          \n";
	std::cout << "        B U L L S          MMM  MM            C O W S       \n";
	std::cout << "                     MMMMMMMO    MMMMMMM                    \n";
	std::cout << "               ZMMMMMMMM               MMMMM                \n";
	std::cout << "             MMMM                         MMMMM             \n";
	std::cout << "            MMM                               MMMMM         \n";
	std::cout << "           MMO                                   MM,        \n";
	std::cout << "\nWelcome to Bulls & Cows\n";
	std::cout << "Isogram: a word that does not have any repeating letters in it.\n";
	std::cout << "  \"planet\" is an isogram, while \"pollen\" is not.\n";
	std::cout << "  Note: for our purposes, isograms have been selected from\n  the 5000 most common English words." << std::endl;
	return;
}; // PrintIntro


bool LoadWordList() 
/*
Tries to load the ISOGRAM file from the folder specified and reports on any errors
*/
{
	std::cout << "\nLoading list of isograms...";
	EFileReadStatus FileStatus = BCGame.LoadWordList(ISOGRAM_FILE);
	switch (FileStatus) 
	{
		case EFileReadStatus::File_Not_Found : 
			std::cout << "\nERROR: Unable to locate Isogram file:\n" << ISOGRAM_FILE << std::endl;
			return false;
		case EFileReadStatus::File_Not_Opened:
			std::cout << "\nERROR: Found Isogram file but unable to open:\n" << ISOGRAM_FILE << std::endl;
			return false;
		case EFileReadStatus::Invalid_Content: 
			std::cout << "\nERROR: Found Isogram file but content does not meet requirements as words are too short or too long:\n" << ISOGRAM_FILE << std::endl;
			return false;
		default:
			std::cout << "Successfully loaded file.\n\n";
			return true;
	};
}; // LoadWordList


int32 GetNumberOfLetters()
/* 
Ask the user for the number of letters in the isogram to try and guess
loop until a valid integer has been entered, reporting any errors
*/
{
	FText NumberOfLetters;
	FWordLength Status;
	do {
		std::cout << "Enter the length of the isogram to try and guess (between " << BCGame.GetMinWordLength() << " and " << BCGame.GetMaxWordLength() << ") : ";
		std::getline(std::cin, NumberOfLetters);
		Status = BCGame.IsValidWordLength(NumberOfLetters);
		switch (Status.Status)
		{
		case EWordLengthStatus::Not_Number:
		case EWordLengthStatus::Invalid:
			std::cout << "Please enter a number.\n\n";
			break;
		case EWordLengthStatus::Out_Of_Range:
			std::cout << "Please enter a number between " << BCGame.GetMinWordLength() << " and " << BCGame.GetMaxWordLength() << ".\n\n";
			break;
		default: break;
		};
	} while (Status.Status != EWordLengthStatus::OK);
	return Status.Length;
}; // GetNumberOfLetters


void PlayTheGame()
/*
Now that we have the difficulty, play the game until the user correctly guesses 
the word or they have run out of tries
*/
{
	BCGame.Reset();
	std::cout << "\nCan you guess the isogram I'm thinking of?\n";
	std::cout << "  Number of letters: " << BCGame.GetHiddenWordLength() << std::endl;
	std::cout << "  Maximum number of tries: " << BCGame.GetMaxTries() << std::endl;
	std::cout << "  Dictionary size: " << BCGame.GetDictionarySize() << std::endl;
	std::cout << std::endl;
	FText Guess = "";
	int32 MaxTries = BCGame.GetMaxTries();

	// loop and get guesses to maximum number of guesses 
	while (!BCGame.GetIsGameWon() && BCGame.GetCurrentTry() <= MaxTries)
	{
		Guess = GetValidGuess();
		// submit valid guess to the game and receive counts
		FBullCowCount BullCowCount = BCGame.SubmitValidGuess(Guess);

		std::cout << "Bulls = " << BullCowCount.Bulls << "  ";
		std::cout << "Cows = " << BullCowCount.Cows << "\n\n";
	};
	BCGame.UpdateTotalGames();
	return;
}; // PlayTheGame


FText GetValidGuess()
/*
Get the guess from the user and report if it is invalid in any way
*/
{
	// Get the guess from the player
	FText Guess = "";
	int32 CurrentTry = BCGame.GetCurrentTry();
	EGuessStatus Status = EGuessStatus::Invalid;
	do {
		std::cout << "Try " << CurrentTry << " out of " << BCGame.GetMaxTries() << ". Enter your guess: ";
		std::getline(std::cin, Guess);

		Status = BCGame.CheckGuessValidity(Guess);
		switch (Status)
		{
		case EGuessStatus::Wrong_Length:
			std::cout << "Please enter a " << BCGame.GetHiddenWordLength() << " length word.\n\n";
			break;
		case EGuessStatus::Not_Alpha:
			std::cout << "Please only use letters; no numbers, spaces or punctuation.\n\n";
			break;
		case EGuessStatus::Not_Isogram:
			std::cout << "Please enter an isogram; a word with no duplicate letters.\n\n";
			break;
		case EGuessStatus::Not_Lowercase:
			std::cout << "Please enter all lowercase letters.\n\n";
			break;
		default: break;
		}; // switch
	} while (Status != EGuessStatus::OK);
	return Guess;
} // GetGuess


void PrintThisGameSummary()
/*
Report single and multi-game statistics
*/
{
	FGameStats GameStats = BCGame.GetGameStats();
	std::cout << "*****************************************************\n";
	// report results of this game
	if (BCGame.GetIsGameWon()) 
	{
		std::cout << "  CONGRATULATIONS, you won!\n";
		int32 UnderPar = BCGame.GetMaxTries() - BCGame.GetCurrentTry() + 1;
		if (UnderPar == 0)
		{
			std::cout << "  You won this game right on par\n";
		}
		else
		{	
			std::cout << "  Won this game with " << UnderPar << " turn" << ((UnderPar==1) ? "" : "s") << " under par\n";
		}
	}
	else 
	{
		std::cout << "  Sorry, you lost.\n";
	};

	if (GameStats.TotalGames>1) 
	{
		// report multi-game stats
		std::cout << "  Games Lost  : " << (GameStats.TotalGames - GameStats.GamesWon) << std::endl;
		std::cout << "  Games Won : " << GameStats.GamesWon << std::endl;
		std::cout << "  Total Games: " << GameStats.TotalGames << " (" << (100 * GameStats.GamesWon / GameStats.TotalGames) << "%)\n";
		// report winning/losing streaks
		if (GameStats.CurrentLosingStreak > 1) { std::cout << "  Current Losing Streak : " << GameStats.CurrentLosingStreak << std::endl; };
		if (GameStats.CurrentWinningStreak > 1) { std::cout << "  Current Winning Streak: " << GameStats.CurrentWinningStreak << std::endl; };		
		if (GameStats.LosingStreak > GameStats.CurrentLosingStreak) { std::cout << "  Worst Losing Streak   : " << GameStats.LosingStreak << std::endl; };
		if (GameStats.WinningStreak > GameStats.CurrentWinningStreak) { std::cout << "  Best Winning Streak   : " << GameStats.WinningStreak << std::endl; };
	};
	std::cout << "*****************************************************\n";
	return;
} // PrintThisGameSummary


EGameReplayStatus AskToPlayAgain()
/*
Request a choice from the user about whether they want to 
play again in some form or not and return result
Note: you can't play the same game again with the same word if you've won!
*/
{
	FText Response = "";
	do
	{
		std::cout << "\nDo you want to play again?\n";
		std::cout << "  1 - Play again with a different word of the same length\n";
		std::cout << "  2 - Play again with a word of a different length\n";
		if (!BCGame.GetIsGameWon()) { std::cout << "  3 - Play again with the same word\n"; };
		std::cout << "  Q - Quit\n";
		std::cout << "Please enter a choice from above: ";
		std::getline(std::cin, Response);
		switch (Response[0])
		{
		case '1': return EGameReplayStatus::Play_With_Different_Word;
		case '2': return EGameReplayStatus::Select_Different_Word_Length;
		case '3': if (!BCGame.GetIsGameWon())
		{
			return EGameReplayStatus::Play_With_Same_Word;
		}
				  else
				  {
					  break;
				  }
		// no default as we're either quitting or an invalid letter has been entered...
		};
	} while (Response[0] != 'Q' && Response[0] != 'q');
	return EGameReplayStatus::Quit;
}; // AskToPlayAgain


void PrintEndGameSummary()
{
	std::cout << "\nThanks for playing!\n";
	return;
} // PrintEndGameSummary
