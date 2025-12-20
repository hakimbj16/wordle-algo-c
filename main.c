#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

// Belhadj Abdelhakim - Mekdam Mohamed Idir - Lamara Ayoub

#define MAX_WORDS 10000
#define WORD_LEN 5
#define MAX_GUESSES 6

typedef struct {
    char word[WORD_LEN + 1];
} Word;

Word dictionary[MAX_WORDS];
int num_words = 0;

// Load the list of 5-letter words from words.txt
int load_dictionary(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Error: words.txt not found. Please put it next to the program.\n");
        return 0;
    }

    char buffer[20];
    while (fgets(buffer, sizeof(buffer), file) && num_words < MAX_WORDS) {
        buffer[strcspn(buffer, "\n")] = 0; 
        if (strlen(buffer) == WORD_LEN) {
            strcpy(dictionary[num_words++].word, buffer);
        }
    }
    fclose(file);
    return num_words;
}

// Check if a guessed word exists in the dictionary
int is_valid(const char* g) {
    for (int i = 0; i < num_words; i++) {
        if (strcmp(dictionary[i].word, g) == 0)
            return 1;
    }
    return 0;
}

// 0 = gray, 1 = yellow, 2 = green
int* feedback(const char* target, const char* guess) {
    int* f = malloc(5 * sizeof(int));
    int count[26] = {0};

    // Mark green letters first
    for (int i = 0; i < 5; i++) {
        if (guess[i] == target[i])
            f[i] = 2;
        else {
            f[i] = 0;
            count[target[i] - 'a']++;
        }
    }

    // Mark yellow letters
    for (int i = 0; i < 5; i++) {
        if (f[i] == 0) {
            int letter = guess[i] - 'a';
            if (count[letter] > 0) {
                f[i] = 1;
                count[letter]--;
            }
        }
    }

    return f;
}

// feedback (G = green, Y = yellow, - = gray)
void print_feedback(int* f) {
    for (int i = 0; i < 5; i++) {
        if (f[i] == 2)
            printf("G ");
        else if (f[i] == 1)
            printf("Y ");
        else
            printf("- ");
    }
    printf("\n");
}

// Player mode
void play(const char* target) {
    char guess[6];
    int attempt = 1;

    while (attempt <= MAX_GUESSES) {
        printf("Guess %d: ", attempt);
        scanf("%5s", guess);

        for (int i = 0; i < 5; i++)
            guess[i] = tolower(guess[i]);

        if (strlen(guess) != 5 || !is_valid(guess)) {
            printf("Invalid word. Try again.\n");
            continue;
        }

        int* f = feedback(target, guess);
        print_feedback(f);

        if (strcmp(guess, target) == 0) {
            printf("You win! You found the word in %d tries.\n", attempt);
            free(f);
            return;
        }

        free(f);
        attempt++;
    }

    printf("You lost. The word was: %s\n", target);
}

// Simple scoring system to choose better guesses
int score(const char* w) {
    int used[26] = {0}, s = 0;
    int freq[26] = {8,1,3,4,12,2,2,5,7,1,1,4,2,7,8,2,1,6,6,9,3,1,2,1,2,1};
    for (int i = 0; i < 5; i++) {
        int l = w[i] - 'a';
        if (!used[l]) {
            s += freq[l];
            used[l] = 1;
        }
    }
    return s;
}

// Solver mode
void solve(const char* target) {
    Word possible[MAX_WORDS];
    for (int i = 0; i < num_words; i++)
        possible[i] = dictionary[i];

    int count = num_words;
    char guess[6] = "water";
    printf("Computer starts with: %s\n", guess);

    int attempt = 1;
    while (attempt <= MAX_GUESSES) {
        printf("Guess %d: %s\n", attempt, guess);
        int* f = feedback(target, guess);
        print_feedback(f);

        if (strcmp(guess, target) == 0) {
            printf("Computer found the word in %d tries!\n", attempt);
            free(f);
            return;
        }

        // Keep only words that match the same feedback pattern
        int new_count = 0;
        for (int i = 0; i < count; i++) {
            int* test_f = feedback(possible[i].word, guess);
            int ok = 1;
            for (int j = 0; j < 5; j++)
                if (test_f[j] != f[j]) ok = 0;
            free(test_f);
            if (ok)
                possible[new_count++] = possible[i];
        }

        count = new_count;
        free(f);

        // Choose the next guess with the highest score
        int best_score = -1;
        char best_word[6];
        for (int i = 0; i < count; i++) {
            int s = score(possible[i].word);
            if (s > best_score) {
                best_score = s;
                strcpy(best_word, possible[i].word);
            }
        }

        strcpy(guess, best_word);
        attempt++;
    }

    printf("Computer failed to find the word.\n");
}

int main() {
    srand(time(0));

    if (!load_dictionary("words.txt"))
        return 1;

    int r = rand() % num_words;
    char target[6];
    strcpy(target, dictionary[r].word);

    printf("WORDLE in C\n");
    printf("1 - Play yourself\n");
    printf("2 - Let the computer solve\n");
    printf("Your choice: ");
    int choice;
    scanf("%d", &choice);

    if (choice == 2)
        solve(target);
    else
        play(target);

    return 0;
}
