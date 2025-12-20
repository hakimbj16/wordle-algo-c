#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

// Belhadj Abdelhakim - Mekdam mohamed idir - Lamara ayoub


#define MAX_WORDS 10000
#define WORD_LEN 5
#define MAX_GUESSES 6

typedef struct {
    char word[WORD_LEN + 1];
} Word;

Word dictionary[MAX_WORDS];
int num_words = 0;

// load words.txt
int load_dictionary(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("No words.txt ! Put it next to program\n");
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

int is_valid(const char* g) {
    for (int i = 0; i < num_words; i++)
        if (strcmp(dictionary[i].word, g) == 0) return 1;
    return 0;
}

// 0 gray - 1 yellow - 2 green
int* feedback(const char* target, const char* guess) {
    int* f = malloc(5 * sizeof(int));
    int cnt[26] = {0};
    for (int i = 0; i < 5; i++) {
        if (guess[i] == target[i]) f[i] = 2;
        else {
            f[i] = 0;
            cnt[target[i]-'a']++;
        }
    }
    for (int i = 0; i < 5; i++) {
        if (f[i] == 0) {
            int l = guess[i]-'a';
            if (cnt[l] > 0) { f[i] = 1; cnt[l]--; }
        }
    }
    return f;
}

void print_f(int* f) {
    for (int i = 0; i < 5; i++) {
        if (f[i]==2) printf("G ");
        else if (f[i]==1) printf("Y ");
        else printf("- ");
    }
    printf("\n");
}

void play(const char* target) {
    char g[6];
    int att = 1;
    while (att <= 6) {
        printf("Guess %d : ", att);
        scanf("%5s", g);
        for (int i=0; i<5; i++) g[i]=tolower(g[i]);
        if (strlen(g)!=5 || !is_valid(g)) {
            printf("Not good word - try again\n");
            continue;
        }
        int* f = feedback(target, g);
        print_f(f);
        if (strcmp(g, target)==0) {
            printf("You win !! %d tries\n", att);
            free(f); return;
        }
        free(f); att++;
    }
    printf("Lost - word was %s\n", target);
}

int score(const char* w) {
    int u[26]={0}, s=0;
    int fr[26]={8,1,3,4,12,2,2,5,7,1,1,4,2,7,8,2,1,6,6,9,3,1,2,1,2,1};
    for (int i=0; i<5; i++) {
        int l = w[i]-'a';
        if (!u[l]) { s += fr[l]; u[l]=1; }
    }
    return s;
}

void solve(const char* target) {
    Word p[MAX_WORDS];
    for (int i=0; i<num_words; i++) p[i]=dictionary[i];
    int cnt = num_words;

    char guess[6] = "water"; 
    printf("I start with water\n");

    int att=1;
    while (att <= 6) {
        printf("Guess %d : %s\n", att, guess);
        int* f = feedback(target, guess);
        print_f(f);

        if (strcmp(guess, target)==0) {
            printf("Done in %d !! \n", att);
            free(f); return;
        }

        int newc = 0;
        for (int i=0; i<cnt; i++) {
            int* tf = feedback(p[i].word, guess);
            int ok=1;
            for (int j=0; j<5; j++) if (tf[j] != f[j]) ok=0;
            free(tf);
            if (ok) p[newc++] = p[i];
        }
        cnt = newc;
        free(f);

        if (cnt==1) { strcpy(guess, p[0].word); continue; }

        int bs = -1; char best[6];
        for (int i=0; i<cnt; i++) {
            int s = score(p[i].word);
            if (s > bs) { bs=s; strcpy(best, p[i].word); }
        }
        strcpy(guess, best);
        att++;
    }
    printf("Hard one , failed\n");
}

int main() {
    srand(time(0));
    if (!load_dictionary("words.txt")) return 1;

    int r = rand() % num_words;
    char target[6]; strcpy(target, dictionary[r].word);

    printf("1 = You play\n2 = Watch me solve\nChoice ? ");
    int ch; scanf("%d", &ch);

    if (ch==2) solve(target);
    else play(target);

    return 0;
}
