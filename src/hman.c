/*
 * Name: hman.c
 * Author: justin_
 * Date: January 16, 2013
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
 
#include "hman.h"

#define WORD_L "./res/vcwords.txt"
#define DEFN_L "./res/vcdefns.txt"
#define BUF_SZ 128
#define DEBUG 0
#define GUESS_LIM 12

#define DEAD_FACE "(x_x)"
#define ALIVE_FACE "(o_o)"
#define TORSO " -|- "
#define LEGS " / \\ "

void print_hman(int dead)
{
	printf("%s\n%s\n", dead ? DEAD_FACE : ALIVE_FACE, TORSO "\n" LEGS);
	// printf("\n" TORSO "\n" LEGS "\n");
}

int is_al(char c)
{
	int i;
	for (i = 0; i < sizeof(alphabet); i++)
	{
		if (alphabet[i] == c)
		{
			return true;
		}
	}
	return false;
}

int rand_int(const int min, const int max)
{
	return rand() % (max - min + 1) + min;
}

void gen_rnd_word(char *word, int *ln)
{
	FILE *f;
	int i = 0, ptr = 0, count = 0, rnd = 0;

	/*
	 * Open a stream to read.
	 */
	f = fopen(WORD_L, "r");
	if (f == NULL)
	{
		printf("Wordlist (path: %s) is missing!\n", WORD_L);
		exit(EXIT_FAILURE);
	}

	/*
	 * Read the wordlist line by line
	 * to count the number of lines.
	 */
	while (fgets(word, BUF_SZ, f))
	{
		++count;
	}

	/*
	 * Generate a random number.
	 */
	rnd = rand_int(0, count);
	*ln = rnd;

	// printf("%d\n", rnd);
	// printf("%d\n", *ln);

	// printf("Count: %d\n", count);
	// printf("Rand:  %d\n", rnd);

	/*
	 * Read the wordlist again and choose a
	 * word based on a randomized line number.
	 */
	f = fopen(WORD_L, "r");
	while (fgets(word, BUF_SZ, f))
	{
		++ptr;
		if (ptr != rnd)
		{
			continue;
		}
		for (i = 0; i < strlen(word); i++)
		{
			if (word[i] == '\n')
			{
				word[i] = 0;
			}
		}
		printf("Chosen a word!\n");
		// printf("(%ld) %s\n", strlen(line), line);
		break;
	}
	fclose(f);
}

void find_defn(char *defn, int ln)
{
	FILE *f;
	int i = 0;

	/*
	 * Open a stream to read.
	 */
	f = fopen(DEFN_L, "r");
	if (f == NULL)
	{
		printf("Wordlist definitions (path: %s) is missing!\n", DEFN_L);
		exit(EXIT_FAILURE);
	}

	/*
	 * Finds the definition.
	 */
	while (fgets(defn, BUF_SZ, f))
	{
		++i;
		if (i != ln)
		{
			continue;
		}
		for (i = 0; i < strlen(defn); i++)
		{
			if (defn[i] == '\n')
			{
				defn[i] = 0;
			}
		}
		if (DEBUG)
		{
			printf("Definition: %s.\n", defn);
		}
		break;
	}
	fclose(f);
}

/*
 * Start the program.
 */
int main(int argc, char *argv[])
{
	int i = 0, ptr = 0, bl_ptr = 0, ln = 0;
	char line[BUF_SZ], defn[BUF_SZ], usr_table[BUF_SZ], input[BUF_SZ], bad_letters[sizeof(alphabet)], c;

	c = 0;
	srand(time(NULL));

	printf("Welcome to Terminal Hangman!\nLoading wordlist...\n");

	memset(line, 0, sizeof(line));
	memset(usr_table, 0, sizeof(usr_table));
	memset(input, 0, sizeof(input));
	memset(bad_letters, 0, sizeof(bad_letters));

	/*
	 * Generate the word. :D
	 */
	gen_rnd_word(line, &ln);

	/*
	 * Find the definition.
	 */
	find_defn(defn, ln);

	/*
	 * Incase there's a bug (like srsly wtf).
	 */
	if (strlen(line) < 1)
	{
		printf("Word cannot be less than one character!\n");
		return EXIT_FAILURE;
	}
	else if (DEBUG)
	{
		printf("The word is: %s.\n", line);
	}

	printf("You have a limit of %d bad guesses.\n", GUESS_LIM);
	print_hman(0);

	/*
	 * Create a "blank word" based on the line.
	 */
	for (i = 0; i < strlen(line); i++)
	{
		usr_table[i] = '_';
	}

	/*
	 * Strip the remaining junk (if any).
	 */
	for (i = 0; i < strlen(usr_table); i++)
	{
		if (usr_table[i] != '_')
		{
			usr_table[i] = 0;
		}
	}

	/*
	 * Begin guessing!
	 */
	while (1)
	{
		printf("%s\n", usr_table);
		while ((c = getchar()) != '\n')
		{
			input[ptr++] = (char) c;
		}
		if (strncmp(input, "exit", BUF_SZ) == 0)
		{
			return EXIT_SUCCESS;
		}
		else if (strncmp(input, "help", BUF_SZ) == 0)
		{
			printf("NO YOU DON'T NEED HELP JUST ENTER A LETTER!\n");
		}
		if (strlen(input) == 1)
		{
			char guess = input[0];
			int wrong = 1;
			for (i = 0; i < strlen(line); i++)
			{
				if (line[i] == guess)
				{
					usr_table[i] = guess;
					wrong = 0;
				}
			}
			/*
			 * Was it a bad guess? LOL
			 */
			if (wrong)
			{
				/*
				 * Only add the guess to the bad letters array
				 * and increment the pointer if it is part of
				 * the alphabet (for guessing reasons, etc).
				 */
				if (is_al(guess))
				{
					int already_guessed = 0;
					for (i = 0; i < strlen(bad_letters); i++)
					{
						if (guess == bad_letters[i])
						{
							already_guessed = 1;
							break;
						}
					}
					if (!already_guessed)
					{
						bad_letters[bl_ptr++] = guess;
						printf("Nope! :S (bad guesses: %d)\nBad Letters: ", bl_ptr);
						for (i = 0; i < sizeof(bad_letters); i++)
						{
							if (bad_letters[i] != 0)
							{
								printf("%c, ", bad_letters[i]);
							}
						}
						printf("\n");
						if (bl_ptr >= GUESS_LIM)
						{
							printf("You ran out of bad guesses!\n");
							print_hman(1);
							printf("The word was %s.\n(defn: %s)\n", line, defn);
							printf("You lose the game (and the man's life)! :(\n");
							return EXIT_SUCCESS;
						}
					}
					else
					{
						printf("You already guessed that letter!\n");
					}
				}
			}
			else
			{
				printf("Yay! :D\n");
			}
		}
		/*
		 * LOL okay so you win.
		 */
		if (strncmp(line, usr_table, BUF_SZ) == 0 || strncmp(line, input, BUF_SZ) == 0)
		{
			printf("Congratulations, you win (and saved the man's life)!\n");
			print_hman(0);
			printf("The word was %s.\n", line);
			if (strncmp(line, input, BUF_SZ) == 0)
			{
				printf("You guessed the entire word, impressive...\n");
			}
			printf("(defn: %s)\nTotal bad (incorrect) guesses: %d.\n", defn, bl_ptr);
			break;
		}
		memset(input, 0, sizeof(input));
		ptr = 0;
	}
	return EXIT_SUCCESS;
}
