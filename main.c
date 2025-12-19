#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

// Wordle Project ALGO3 - Group: Mohammed Alami, Sara Benali, Karim El Idrissi
// Simple game + solver in one file

#define MAX_WORDS 15000
#define WORD_LEN 5
#define MAX_GUESSES 6

typedef struct {
    char word[WORD_LEN + 1];
} Word;

Word dictionary[MAX_WORDS];
int num_words = 0;

// Load words from file
int load_dictionary(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Error: Can't open %s! Put words.txt in folder.\n", filename);
        return 0;
    }
    char buffer[20];
    while (fgets(buffer, sizeof(buffer), file) && num_words < MAX_WORDS) {
        buffer[strcspn(buffer, "\n")] = 0;
        if (strlen(buffer) == WORD_LEN) {
            for (int i = 0; i < WORD_LEN; i++) buffer[i] = tolower(buffer[i]);
            strcpy(dictionary[num_words++].word, buffer);
        }
    }
    fclose(file);
    printf("Loaded %d words.\n", num_words);
    return num_words;
}

int is_valid_word(const char* guess) {
    for (int i = 0; i < num_words; i++) {
        if (strcmp(dictionary[i].word, guess) == 0) return 1;
    }
    return 0;
}

// Feedback: 0=gray, 1=yellow, 2=green
int* get_feedback(const char* target, const char* guess) {
    int* fb = malloc(WORD_LEN * sizeof(int));
    int count[26] = {0};
    for (int i = 0; i < WORD_LEN; i++) {
        if (guess[i] == target[i]) {
            fb[i] = 2;
        } else {
            fb[i] = 0;
            count[target[i] - 'a']++;
        }
    }
    for (int i = 0; i < WORD_LEN; i++) {
        if (fb[i] == 0) {
            int let = guess[i] - 'a';
            if (count[let] > 0) {
                fb[i] = 1;
                count[let]--;
            }
        }
    }
    return fb;
}

void print_feedback(int* fb) {
    for (int i = 0; i < WORD_LEN; i++) {
        if (fb[i] == 2) printf("G ");
        else if (fb[i] == 1) printf("Y ");
        else printf("- ");
    }
    printf("\n");
}

// Human play
void play_game(const char* target) {
    char guess[WORD_LEN + 1];
    for (int att = 1; att <= MAX_GUESSES; att++) {
        printf("\nGuess %d/6: ", att);
        if (!scanf("%5s", guess)) continue;
        for (int i = 0; i < WORD_LEN; i++) guess[i] = tolower(guess[i]);
        guess[WORD_LEN] = 0;

        if (strlen(guess) != WORD_LEN || !is_valid_word(guess)) {
            printf("Invalid! Must be 5 letters from dictionary.\n");
            att--;
            continue;
        }

        int* fb = get_feedback(target, guess);
        print_feedback(fb);
        if (strcmp(guess, target) == 0) {
            printf("You won in %d guesses!\n", att);
            free(fb);
            return;
        }
        free(fb);
    }
    printf("Lost! Word was %s\n", target);
}

// Rough letter freq scores (higher = more common)
int freq[26] = {8,1,3,4,12,2,2,5,7,1,1,4,2,7,8,2,1,6,6,9,3,1,2,1,2,1}; // approx

int score_word(const char* w) {
    int used[26] = {0};
    int sc = 0;
    for (int i = 0; i < WORD_LEN; i++) {
        int l = w[i] - 'a';
        if (!used[l]) {
            sc += freq[l];
            used[l] = 1;
        }
    }
    return sc;
}

// Solver
void solve_game(const char* target) {
    Word possibles[MAX_WORDS];
    memcpy(possibles, dictionary, num_words * sizeof(Word));
    int count = num_words;

    char guess[WORD_LEN + 1] = "raise"; // good starter: common letters
    printf("Solver starting with: %s\n", guess);

    for (int att = 1; att <= MAX_GUESSES; att++) {
        printf("\nGuess %d: %s\n", att, guess);
        int* fb = get_feedback(target, guess);
        print_feedback(fb);

        if (strcmp(guess, target) == 0) {
            printf("Solved in %d guesses!\n", att);
            free(fb);
            return;
        }

        // Filter
        int new_count = 0;
        for (int i = 0; i < count; i++) {
            int* test = get_feedback(possibles[i].word, guess);
            int match = 1;
            for (int j = 0; j < WORD_LEN; j++) if (test[j] != fb[j]) { match = 0; break; }
            free(test);
            if (match) strcpy(possibles[new_count++].word, possibles[i].word);
        }
        count = new_count;
        free(fb);
        printf("Remaining words: %d\n", count);

        if (count <= 1) {
            strcpy(guess, possibles[0].word);
            continue;
        }

        // Pick best score
        int best_sc = -1;
        char best[WORD_LEN + 1];
        for (int i = 0; i < count; i++) {
            int sc = score_word(possibles[i].word);
            if (sc > best_sc) {
                best_sc = sc;
                strcpy(best, possibles[i].word);
            }
        }
        strcpy(guess, best);
    }
    printf("Failed! (rare with good dict)\n");
}

int main(int argc, char** argv) {
    srand(time(NULL));
    if (!load_dictionary("words.txt")) return 1;

    int idx = rand() % num_words;
    char target[WORD_LEN + 1];
    strcpy(target, dictionary[idx].word);

    printf("Wordle ALGO3 Project\n");
    printf("1. Play as human\n");
    printf("2. Watch solver\n");
    printf("Choose: ");
    int mode;
    scanf("%d", &mode);

    if (mode == 2) solve_game(target);
    else play_game(target);

    return 0;
}
