/// Ha Van Hoang, MSSV: 21520033
/// hhoangcpascal
/// cun_thich_lap_trinh

#define __USE_MINGW_ANSI_STDIO 1
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#define llong long long

struct process{
    int id;
    int remaining_time, arrival_time, burst_time;
    llong wait_time, response_time, turnaround_time, last_time;
    bool processing;
    char name[17];
};

int cmp_arrival_time(const void* A, const void* B) {
    if ((*(struct process*)A).arrival_time != (*(struct process*) B).arrival_time) {
        int a = (*(struct process*)A).arrival_time;
        int b = (*(struct process*)B).arrival_time;

        return (a > b) - (a < b);
    }

    return (*(struct process*)A).id - (*(struct process*) B).id;
}

bool cmp_burst_time(const struct process A, const struct process B) {
    if (A.burst_time != B.burst_time)
        return A.burst_time < B.burst_time;
    if (A.arrival_time != B.arrival_time) return A.arrival_time < B.arrival_time;
    return A.id < B.id;
}

bool cmp_remainning_time(const struct process A, const struct process B) {
    if (A.remaining_time != B.remaining_time)
        return A.remaining_time < B.remaining_time;
    if (A.processing != B.processing) return A.processing;
    if (A.arrival_time != B.arrival_time) return A.arrival_time < B.arrival_time;
    return A.id < B.id;
}

void swap(struct process* A, struct process* B) {
    struct process C = *A;
    *A = *B;
    *B = C;
}

struct process *P, *ans;
int num_process;

typedef struct node_q {
    struct process key;
    struct node_q *left, *right;
} *nodeq;

struct queue {
    nodeq head, tail;
    int sz;
} Q;

bool empty_queue(struct queue Q) {
    return Q.sz == 0;
}

struct process front_queue(struct queue Q) {
    return Q.head->key;
}

int size_queue(struct queue Q) {
    return Q.sz;
}

void push_queue(struct queue* Q, struct process p) {
    nodeq curr = (nodeq) calloc(1, sizeof(struct node_q));
    curr->key = p;
    curr->left = curr->right = NULL;

    if (++Q->sz == 1) {
        Q->tail = Q->head = curr;
        return;
    }

    curr->left = Q->tail;
    Q->tail->right = curr;
    Q->tail = curr;
};

void pop_queue(struct queue* Q) {
    nodeq curr = Q->head; Q->head = Q->head->right;
    free(curr);

    if (--Q->sz == 0) {
        Q->head = Q->tail = NULL;
        return;
    }

    Q->head->left = NULL;
}

typedef struct node_pq {
    struct process key;
    struct node_pq *parent, *left, *right;
} *nodepq;

struct priority_queue {
    nodepq root;
    int sz;
};

bool empty_priority_queue(struct priority_queue PQ) {
    return PQ.sz == 0;
}

struct process top_priority_queue(struct priority_queue PQ) {
    return PQ.root->key;
}

int size_priority_queue(struct priority_queue PQ) {
    return PQ.sz;
}

void push_priority_queue(struct priority_queue* PQ, struct process p, const bool (*T)(const struct process, const struct process)) {
    ++PQ->sz;
    int len = 30 - __builtin_clz(PQ->sz);
    nodepq curr = PQ->root, parent = NULL;

    for(int i = len; i >= 0; --i) {
        if ((*T)(p, curr->key)) swap(&p, &curr->key);

        parent = curr;
        if ((PQ->sz >> i) & 1) curr = curr->right;
        else curr = curr->left;
    }

    curr = (nodepq) calloc(1, sizeof(struct node_pq));
    curr->parent = parent;
    curr->key = p;
    curr->left = curr->right = NULL;

    if (PQ->sz == 1) PQ->root = curr;
    else if (PQ->sz & 1) parent->right = curr;
    else parent->left = curr;
};

void pop_priority_queue(struct priority_queue* PQ, const bool (*T)(const struct process, const struct process)) {
    int len = 32 - __builtin_clz(PQ->sz) - 2;
    nodepq curr = PQ->root, parent = NULL;

    for(int i = len; i >= 0; --i) {
        parent = curr;
        if ((PQ->sz >> i) & 1) curr = curr->right; else curr = curr->left;
    }

    swap(&PQ->root->key, &curr->key);
    free(curr); curr = NULL;

    if (--PQ->sz == 0) {
        PQ->root = NULL;
        return;
    }

    if ((PQ->sz + 1) & 1) parent->right = NULL;
    else parent->left = NULL;

    curr = PQ->root;
    nodepq child = curr->left;
    while (child != NULL) {
        if (curr->right != NULL && (*T)(curr->right->key, child->key)) child = curr->right;

        if ((*T)(child->key, curr->key)) {
            swap(&child->key, &curr->key);
            curr = child; child = curr->left;
        } else break;
    }
}

struct queue Gantt = {NULL, NULL, 0};

/// First Come First Serve
void FCFS() {
    ans = (struct process*) calloc(num_process, sizeof(struct process));

    qsort(P, num_process, sizeof(struct process), cmp_arrival_time);
    struct queue Q = {NULL, NULL, 0};

    llong time_elapsed = 0;
    for(int s = 0, i = 0; s < num_process; ++s) {
        if (i < num_process && P[i].arrival_time > time_elapsed) {
            time_elapsed = P[i].arrival_time;
            push_queue(&Gantt, (struct process) {-1, 0, 0, 0, 0, 0, 0, time_elapsed, false, ""});
        }

        while (i < num_process && P[i].arrival_time <= time_elapsed) push_queue(&Q, P[i++]);

        struct process first = front_queue(Q); pop_queue(&Q);
        ans[s] = first;
        ans[s].turnaround_time = time_elapsed + first.burst_time - first.arrival_time;
        ans[s].response_time = time_elapsed - first.arrival_time;
        ans[s].wait_time = ans[s].response_time;
        time_elapsed += first.burst_time;
        ans[s].last_time = time_elapsed;
        push_queue(&Gantt, ans[s]);
    }

    for(int i = 0; i < num_process; ++i) P[ans[i].id] = ans[i];
    while (!empty_queue(Q)) pop_queue(&Q);
}

/// Shortest Job First
void SJF() {
    ans = (struct process*) calloc(num_process, sizeof(struct process));

    qsort(P, num_process, sizeof(struct process), cmp_arrival_time);
    struct priority_queue PQ = {NULL, 0};

    llong time_elapsed = 0;
    for(int s = 0, i = 0; s < num_process; ++s) {
        if (i < num_process && P[i].arrival_time > time_elapsed) {
            time_elapsed = P[i].arrival_time;
            push_queue(&Gantt, (struct process) {-1, 0, 0, 0, 0, 0, 0, time_elapsed, false, ""});
        }

        while (i < num_process && P[i].arrival_time <= time_elapsed) push_priority_queue(&PQ, P[i++], cmp_burst_time);

        struct process best = top_priority_queue(PQ); pop_priority_queue(&PQ, cmp_burst_time);
        ans[s] = best;
        ans[s].turnaround_time = time_elapsed + best.burst_time - best.arrival_time;
        ans[s].response_time = time_elapsed - best.arrival_time;
        ans[s].wait_time = ans[s].response_time;
        time_elapsed += best.burst_time;
        ans[s].last_time = time_elapsed;
        push_queue(&Gantt, ans[s]);
    }

    for(int i = 0; i < num_process; ++i) P[ans[i].id] = ans[i];
    while (!empty_priority_queue(PQ)) pop_priority_queue(&PQ, cmp_burst_time);
}

/// Shortest Remaining Time First
void SRTF() {
    ans = (struct process*) calloc(num_process, sizeof(struct process));

    qsort(P, num_process, sizeof(struct process), cmp_arrival_time);
    struct priority_queue PQ = {NULL, 0};

    llong time_elapsed = 0;
    for(int i = 0; i < num_process || !empty_priority_queue(PQ);) {
        if (i < num_process && empty_priority_queue(PQ) && time_elapsed < P[i].arrival_time) {
            time_elapsed = P[i].arrival_time;
            push_queue(&Gantt, (struct process) {-1, 0, 0, 0, 0, 0, 0, time_elapsed, false, ""});
        }

        bool flag = false;
        struct process curr;

        while (!empty_priority_queue(PQ)) {
            curr = top_priority_queue(PQ); pop_priority_queue(&PQ, cmp_remainning_time);
            curr.wait_time += time_elapsed - curr.last_time;
            curr.processing = true;

            if (i < num_process && time_elapsed + curr.remaining_time > P[i].arrival_time) {
                if (curr.remaining_time == curr.burst_time && time_elapsed < P[i].arrival_time) curr.response_time += time_elapsed;
                curr.remaining_time -= P[i].arrival_time - time_elapsed;
                time_elapsed = P[i].arrival_time;
                curr.last_time = time_elapsed;
                flag = true;
                push_queue(&Gantt, curr);
                break;
            }

            if (curr.remaining_time == curr.burst_time) curr.response_time += time_elapsed;

            time_elapsed += curr.remaining_time;
            curr.turnaround_time += time_elapsed;
            curr.last_time = time_elapsed;
            curr.remaining_time = 0;
            curr.processing = false;
            ans[curr.id] = curr;
            push_queue(&Gantt, curr);
        }

        while (i < num_process && P[i].arrival_time == time_elapsed) {
            struct process tmp = P[i++];
            tmp.wait_time = -tmp.arrival_time;
            tmp.turnaround_time = -tmp.arrival_time;
            tmp.response_time = -tmp.arrival_time;
            tmp.remaining_time = tmp.burst_time;
            tmp.last_time = 0;
            tmp.processing = false;

            push_priority_queue(&PQ, tmp, cmp_remainning_time);
        }

        if (flag) {
            if (!empty_priority_queue(PQ)) {
                struct process chk = top_priority_queue(PQ);
                if (chk.remaining_time < curr.remaining_time) {
                    pop_priority_queue(&PQ, cmp_remainning_time);
                    chk.processing = true;
                    curr.processing = false;
                    push_priority_queue(&PQ, chk, cmp_remainning_time);
                }
            }

            push_priority_queue(&PQ, curr, cmp_remainning_time);
        }
    }

    for(int i = 0; i < num_process; ++i) P[i] = ans[i];
    while (!empty_priority_queue(PQ)) pop_priority_queue(&PQ, cmp_remainning_time);
}

/// Round-Robin
void RR() {
    int Quantum_time;
	  printf("Enter Quantum time: "); scanf("%d",&Quantum_time);
    ans = (struct process*) calloc(num_process, sizeof(struct process));

    qsort(P, num_process, sizeof(struct process), cmp_arrival_time);
    struct queue Q = {NULL, NULL, 0};

    llong time_elapsed = 0;
    for(int i = 0; i < num_process || !empty_queue(Q);) {
        if (i < num_process && empty_queue(Q) && time_elapsed < P[i].arrival_time) {
            time_elapsed = P[i].arrival_time;
            push_queue(&Gantt, (struct process) {-1, 0, 0, 0, 0, 0, 0, time_elapsed, false, ""});
        }

        bool flag = false;
        struct process curr;

        if (!empty_queue(Q)) {
            curr = front_queue(Q); pop_queue(&Q);
            if (curr.remaining_time == curr.burst_time) curr.response_time += time_elapsed;
            curr.wait_time += time_elapsed - curr.last_time;

            if (curr.remaining_time <= Quantum_time) {
                time_elapsed += curr.remaining_time;
                curr.remaining_time = 0;
                curr.turnaround_time += time_elapsed;
                curr.last_time = time_elapsed;
                ans[curr.id] = curr;
            } else {
                time_elapsed += Quantum_time;
                curr.remaining_time -= Quantum_time;
                curr.last_time = time_elapsed;
                flag = true;
            }

            push_queue(&Gantt, curr);
        }

        while (i < num_process && P[i].arrival_time <= time_elapsed) {
            struct process tmp = P[i++];
            tmp.remaining_time = tmp.burst_time;
            tmp.wait_time = tmp.response_time = tmp.turnaround_time = -tmp.arrival_time;
            tmp.last_time = 0;
            push_queue(&Q, tmp);
        }

        if (flag) push_queue(&Q, curr);
    }

    for(int i = 0; i < num_process; ++i) P[i] = ans[i];
    while (!empty_queue(Q)) pop_queue(&Q);
}

int num_digits(llong number) {
    int len = 0;
    if (number == 0) return 1;
    while (number > 0) ++len, number /= 10;
    return len;
}

struct process *out_gantt;
int Max_len;

void output() {
	  printf("\n\nGANTT CHART:\n");
    if (!empty_queue(Gantt)) {
        int len = size_queue(Gantt);
        out_gantt= (struct process*) calloc(len, sizeof(struct process));

        for(int i = 0; i < len; ++i) {
            out_gantt[i] = front_queue(Gantt);
            pop_queue(&Gantt);
            if (Max_len < num_digits(out_gantt[i].last_time) + 1) Max_len = num_digits(out_gantt[i].last_time) + 1;
        }

        for(int i = 0, j = 0; i < len; ++i)
        if (i == len - 1 || out_gantt[i].id != out_gantt[i + 1].id) {
            out_gantt[j++] = out_gantt[i];
            if (i == len - 1) {
                len = j;
                break;
            }
        }

        llong start = out_gantt[0].arrival_time;
        int s = 0;
        if (out_gantt[0].id == -1) start = out_gantt[0].last_time, ++s;
        int slen = num_digits(start);

        for(int i = 1; i < slen; ++i) printf(" ");
        printf("+");
        for(int i = s; i < len; ++i) {
            for(int j = 0; j < Max_len; ++j) printf("-");
            printf("+");
        }

        printf("\n");
        for(int i = 1; i < slen; ++i) printf(" ");
        printf("|");
        for(int i = s; i < len; ++i) {
            int tot = Max_len - strlen(out_gantt[i].name);

            for(int j = 1; j <= tot / 2; ++j) printf(" ");
            printf("%s", out_gantt[i].name);
            for(int j = 1; j <= tot - tot / 2; ++j) printf(" ");

            printf("|");
        }

        printf("\n");
        for(int i = 1; i < slen; ++i) printf(" ");
        printf("+");
        for(int i = s; i < len; ++i) {
            for(int j = 0; j < Max_len; ++j) printf("-");
            printf("+");
        }

        printf("\n%lld", start);
        for(int i = s; i < len; ++i) {
            int tot = Max_len + 1 - num_digits(out_gantt[i].last_time);

            for(int j = 1; j <= tot; ++j) printf(" ");
            printf("%lld", out_gantt[i].last_time);
        }
    }

    printf("\n\nTABLE:\n");
    printf("+----------------+--------------+------------+-----------------+-----------------+--------------+---------------+\n");
	  printf("|  Process Name  | Arrival Time | Burst Time | Completion Time | Turnaround Time | Waiting Time | Response Time |\n");
	  printf("|----------------|--------------|------------|-----------------|-----------------|--------------|---------------|\n");

	  double avg_waiting_time = 0;
	  double avg_turnaround = 0;
	  double avg_respone_time = 0;
	  for(int i = 0; i < num_process; ++i){
		    printf("|%-16s|%-14d|%-12d|%-17lld|%-17lld|%-14lld|%-15lld|\n", P[i].name, P[i].arrival_time, P[i].burst_time, P[i].turnaround_time + P[i].arrival_time, P[i].turnaround_time, P[i].wait_time, P[i].response_time);

		    avg_waiting_time += P[i].wait_time;
		    avg_turnaround += P[i].turnaround_time;
		    avg_respone_time += P[i].response_time;

		    if (i < num_process - 1)
		    printf("|----------------|--------------|------------|-----------------|-----------------|--------------|---------------|\n");
	  }

	  printf("+----------------+--------------+------------+-----------------+-----------------+--------------+---------------+\n");
	  avg_waiting_time /= num_process;
	  avg_turnaround /= num_process;
	  avg_respone_time /= num_process;

	  printf("\n");
	  printf("Average waiting time : %.2lf\n", avg_waiting_time);
	  printf("Average turnaround time: %.2lf\n", avg_turnaround);
	  printf("Average respone time: %.2lf\n",avg_respone_time);
}

void reset(){
	for (int i = 0; i < num_process; i++){
		P[i].last_time = P[i].turnaround_time = P[i].wait_time = P[i].response_time = 0;
		P[i].remaining_time = P[i].burst_time;
		P[i].processing = false;
	}

    while (!empty_queue(Gantt)) pop_queue(&Gantt);
}

int main() {
    printf("Enter the number of processes: "); scanf("%d", &num_process);
    P = (struct process*) calloc(num_process, sizeof(struct process));

    Max_len = 0;
    for(int i = 0; i < num_process; ++i) {
        printf("Enter the Process Name, Arrival Time and Burst Time: ");
        scanf("%s%d%d", P[i].name, &P[i].arrival_time, &P[i].burst_time);
        P[i].id = i;
        if (Max_len < strlen(P[i].name)) Max_len = strlen(P[i].name);
    }

    Max_len += 2;
    if (Max_len > 17) Max_len = 17;

    char name_algo[10];
    char pname4[] = "RR";
    char pname3[] = "SRTF";
    char pname2[] = "SJF";
    char pname1[] = "FCFS";

    while (2 * 2 - 4 * 4 + 6 * 6 - 6 * 4 == 0){
    printf("%s","Enter algorithm (FCFS, SJF, SRTF, RR): ");
	scanf("%s",name_algo);

	reset(P);

        if (strcmp(name_algo, pname1) == 0) FCFS(), output();
	else if (strcmp(name_algo, pname2) == 0) SJF(), output();
	else if (strcmp(name_algo, pname3) == 0) SRTF(), output();
	else if (strcmp(name_algo, pname4) == 0) RR(), output();
	else break;

	printf("\n");
    }

    return 0;
}
