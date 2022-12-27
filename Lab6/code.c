/// hhoangcpascal
/// Ha Van Hoang
/// 21520033

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

int** FIFO(int* input, int size, int frames) {
    int* pages = (int*) calloc(frames, sizeof(int));
    int** table = (int**) calloc(size, sizeof(int*));

    for(int i = 0; i < frames; ++i) pages[i] = -1;

    int size_pages = 0, pos = 0;
    for(int i = 0; i < size; ++i) {
        table[i] = (int*) calloc(frames + 1, sizeof(int));

        bool find = false;
        for(int j = 0; j < size_pages && !find; ++j)
        if (pages[j] == input[i]) find = true;

        if (find) {
            for(int j = 0; j < frames; ++j) table[i][j] = pages[j];
            table[i][frames] = 0;

            continue;
        }

        if (size_pages < frames) pages[size_pages++] = input[i];
        else {
            pages[pos] = input[i];
            pos = (pos + 1) % frames;
        }

        for(int j = 0; j < frames; ++j) table[i][j] = pages[j];
        table[i][frames] = 1;
    }

    free(pages);
    return table;
}

int** OPT(int* input, int size, int frames) {
    int *pages = (int*) calloc(frames, sizeof(int)), *next = (int*) calloc(frames, sizeof(int));
    int** table = (int**) calloc(size, sizeof(int*));

    for(int i = 0; i < frames; ++i) pages[i] = -1;

    int size_pages = 0;
    for(int i = 0; i < size; ++i) {
        table[i] = (int*) calloc(frames + 1, sizeof(int));

        int r = size + 1;
        for(int j = i + 1; j < size; ++j)
        if (input[i] == input[j]) {
            r = j;
            break;
        }

        int find = -1;
        for(int j = 0; j < size_pages && find == -1; ++j)
        if (pages[j] == input[i]) find = j;

        if (find != -1) {
            next[find] = r;

            for(int j = 0; j < frames; ++j) table[i][j] = pages[j];
            table[i][frames] = 0;

            continue;
        }

        if (size_pages < frames) {
            pages[size_pages] = input[i];
            next[size_pages++] = r;
        } else {
            int worst = -1;
            for(int j = 0; j < frames; ++j)
            if (worst == -1 || next[worst] < next[j]) worst = j;

            if (worst != -1) {
                pages[worst] = input[i];
                next[worst] = r;
            }
        }

        for(int j = 0; j < frames; ++j) table[i][j] = pages[j];
        table[i][frames] = 1;
    }

    free(pages); free(next);
    return table;
}

int** LRU(int* input, int size, int frames) {
    int *pages = (int*) calloc(frames, sizeof(int)), *last = (int*) calloc(frames, sizeof(int));
    int** table = (int**) calloc(size, sizeof(int*));

    for(int i = 0; i < frames; ++i) pages[i] = -1;

    int size_pages = 0;
    for(int i = 0; i < size; ++i) {
        table[i] = (int*) calloc(frames + 1, sizeof(int));

        int find = -1;
        for(int j = 0; j < size_pages && find == -1; ++j)
        if (pages[j] == input[i]) find = j;

        if (find != -1) {
            last[find] = i;

            for(int j = 0; j < frames; ++j) table[i][j] = pages[j];
            table[i][frames] = 0;

            continue;
        }

        if (size_pages < frames) {
            pages[size_pages] = input[i];
            last[size_pages++] = i;
        } else {
            int worst = -1;
            for(int j = 0; j < frames; ++j)
            if (worst == -1 || last[worst] > last[j]) worst = j;

            if (worst != -1) {
                pages[worst] = input[i];
                last[worst] = i;
            }
        }

        for(int j = 0; j < frames; ++j) table[i][j] = pages[j];
        table[i][frames] = 1;
    }

    free(pages); free(last);
    return table;
}

void print(int *input, int **table, int size, int frames) {
	printf("\n\t--- Page Replacement algorithm ---\n");
	printf("\t----------------------------------\n\n\t");
	for (int i = 0; i < size; ++i)
		printf("%2d ", input[i]);

	printf("\n\n\t");

	for (int j = 0; j < frames; ++j) {
		for (int i = 0; i < size; i++)
			if (table[i][j] != -1)
				printf("%2d ", table[i][j]);
			else
				printf("   ");
		printf("\n\n\t");
	}

	int cnt = 0;
	for (int i = 0; i < size; ++i) {
		if (table[i][frames] == 1) printf(" * "), ++cnt;
		else printf("   ");
	}

	printf("\n\n\tNumber of Page Fault : %d\n\n", cnt);
}

int main() {
    while (1) {
        printf("\t--- Page Replacement algorithm ---\n");
        printf("\t1. Default referenced sequence: 2, 1, 5, 2, 0, 0, 3, 3, 0, 0, 7\n");
        printf("\t2. Manual input sequence       \t\n");
        printf("\t3. Exit");
        int key = 0;
        while (1 > key || 3 < key) {
            printf("\n\tEnter: ");
            scanf("%d", &key);
        }

        int *input, size;

        if (key == 1) {
            size = 11;
            input = (int*) calloc(size, sizeof(int));

            input[0] = input[3] = 2;
            input[1] = 1;
            input[2] = 5;
            input[4] = input[5] = input[8] = input[9] = 0;
            input[6] = input[7] = 3;
            input[10] = 7;
        } else if (key == 2) {
            printf("\n\t--- Page Replacement algorithm ---\n");
            printf("\tInput length of sequences: ");
            scanf("%d", &size);

            printf("\tInput sequences: ");
            input = (int*) calloc(size, sizeof(int));
            for(int i = 0; i < size; ++i) scanf("%d", &input[i]);
        } else break;

        int frames;
        printf("\n\t--- Page Replacement algorithm ---\n");
        printf("\tInput page frames: ");
        scanf("%d", &frames);

        while (1) {
            int** table;

            printf("\n\t--- Page Replacement algorithm ---\n");
            printf("\t1. FIFO algorithm.\n");
            printf("\t2. OPT algorithm.\n");
            printf("\t3. LRU algorithm.\n");
            printf("\t4. Exit.\n");

            key = 0;
            while (1 > key || 4 < key) {
                printf("\n\tEnter: ");
                scanf("%d", &key);
            }

            if (key == 4) break;
            if (key == 1) table = FIFO(input, size, frames);
            if (key == 2) table = OPT(input, size, frames);
            if (key == 3) table = LRU(input, size, frames);

            print(input, table, size, frames);
        }
    }


    return 0;
}
