#include <stdio.h>
#include <string.h>
#define STATES 50

struct DFA_state {
    char name;
    char StateString[STATES + 1];
    char trans[10];
    int is_final;
};

struct DFA_state dfa_states[50];

struct tran {
    char sym;
    int toStates[50];
    int numberOfTransitions;
};

struct state {
    int no;
    struct tran transitions[50];
};

int stackA[100], stackB[100], stackA_top = -1, stackB_top = -1;
struct state nfa_states[STATES];
char temp[STATES+1], nfa_alphabets[10];

int nfa_stateCount, nfa_alphabetCount, nfa_finalStateCount, j, dfa_stateCount = -1, nfa_finalStates[20], nfa_startState;

void pushB(int z) {
    stackB[++stackB_top] = z;
}

int popB() {
    return stackB[stackB_top--];
}

int peekB() {
    return stackA[stackB_top];
}

void pushA(int z) {
    stackA[++stackA_top] = z;
}

int popA() {
    return stackA[stackA_top--];
}

int peekA() {
    return stackA[stackA_top];
}

void copy(int i) {
    char temp[STATES + 1] = " ";
    int k = 0;
    stackB_top = -1;
    strcpy(temp, dfa_states[i].StateString);

    while(temp[k] != '\0') {
        pushB(temp[k] - '0');
        k++;
    }
}

int seek(int arr[], int ptr, int s) {
    int i;
    for(i = 0; i <= ptr; i++) {
        if(s == arr[i]) {
            return 1;
        }
    }
    return 0;
}

void sortStackB() {
    int i, j, temp;
    for(i = 0; i < stackB_top; i++) {
        for(j = 0; j < (stackB_top - i); j++) {
            if(stackB[j] > stackB[j + 1]) {
                temp = stackB[j];
                stackB[j] = stackB[j + 1];
                stackB[j + 1] = temp;
            }
        }
    }
}

void toString() {
    int i = 0;
    sortStackB();
    for(i = 0; i <= stackB_top; i++) {
        temp[i] = stackB[i] + '0';
    }

    // нуль-символ
    temp[i] = '\0';
}

void display_DFA() {
    int i, j;
    printf("\nStates\tString\tInputs\n\t\t");
    for(i = 0; i < nfa_alphabetCount; i++) {
        printf("\t%c", nfa_alphabets[i]);
    }

    printf("\n \t\t\t----------");
    for(i = 0; i < dfa_stateCount; i++) {
    
        if(dfa_states[i].is_final == 0) {
            printf("\n%c", dfa_states[i].name);
        } else {
            printf("\n*%c", dfa_states[i].name);
        }
      
        printf("\t%s\t", dfa_states[i].StateString);

        for(j = 0; j < nfa_alphabetCount; j++) {
            printf("\t%c", dfa_states[i].trans[j]);
        }
    }
    printf("\n");
}


void move(int st, int j) {
    int ctr = 0;

    while(ctr < nfa_states[st].transitions[j].numberOfTransitions) {
        pushA(nfa_states[st].transitions[j].toStates[ctr++]);
    }
}

void lambda_closure(int st) {
    int ctr = 0, in_state = st, curst = st, check;

    while(stackA_top != -1) {
        curst = popA();
        ctr = 0;
        in_state = curst;

        while(ctr <= nfa_states[curst].transitions[nfa_alphabetCount].numberOfTransitions) {
            // проверяем, есть ли состояние in_state в stackB
            check = seek(stackB, stackB_top, in_state);
            if(check == 0) {
                pushB(in_state);
            }

            // берем следующее состояние [in_state], куда мы попадаем из nfa_states[curst]
            in_state = nfa_states[curst].transitions[nfa_alphabetCount].toStates[ctr++];

            // проверяем, есть ли состояние in_state в stackA
            check = seek(stackA, stackA_top, in_state);

            // если состояние in_state отсутствеут в stackA, и ctr не превышает количества переходов из nfa_states[curst]
            if(check == 0 && ctr <= nfa_states[curst].transitions[nfa_alphabetCount].numberOfTransitions) {
                // то ложим в stackA
                pushA(in_state);
            }
        }
    }
}

void convert() {
    int i = 0, dfa_stateCount = 0, fin = 0;
    pushA(nfa_startState);
    lambda_closure(peekA());
    toString();

    dfa_states[dfa_stateCount].name = 'A';
    dfa_stateCount++;
    strcpy(dfa_states[0].StateString, temp);

    while(i < dfa_stateCount) {
        for(j = 0; j < nfa_alphabetCount; j++) {
            fin = 0;
            copy(i);

            // ложим в stackA все следующие ходы
            while(stackB_top != -1) {
                move(popB(), j);
            }

            // находим все epsilon closure из peekA(), и ложим в stackB
            while(stackA_top != -1) {
                lambda_closure(peekA());
            }

            // строим temp из stackB
            toString();

            int k = 0;
            for(k = 0; k < dfa_stateCount; k++) {
                // если мы уже рассмотрели это состояние
                if((strcmp(temp, dfa_states[k].StateString) == 0)) {
                    // даем им названия и завершаем цикл
                    dfa_states[i].trans[j] = dfa_states[k].name;
                    break;
                }
            }

            if(k == dfa_stateCount) {
                dfa_stateCount++;

                // ищем среди финальных состояний
                for(k = 0; k < nfa_finalStateCount; k++) {

                    // если в stackB есть финальное состояние
                    fin = seek(stackB, stackB_top, nfa_finalStates[k]);
                    if(fin == 1) {
                        // то помечаем последнее состояние из dfa_states как конечное
                        dfa_states[dfa_stateCount - 1].is_final = 1;
                        break;
                    }                 
                }

                strcpy(dfa_states[dfa_stateCount - 1].StateString, temp);
                dfa_states[dfa_stateCount - 1].name = 'A' + dfa_stateCount - 1;
                dfa_states[i].trans[j] = dfa_states[dfa_stateCount - 1].name;
            }
        }
        i++;
    }
}

int main() {
    int fin = 0, i;
    char ans, st[20];

    printf("Кол-во состояний в NFA: ");
    scanf("%d", &nfa_stateCount);

    for(i = 0; i < nfa_stateCount; i++) {
        nfa_states[i].no = i;
    }

    printf("Стартовое состояние: ");
    scanf("%d", &nfa_startState);

    printf("Кол-во завершающих состояний в NFA: ");
    scanf("%d", &nfa_finalStateCount);

    printf("Введите завершающие состояния: \n");
    for(i = 0; i < nfa_finalStateCount; i++) {
        scanf("%d", &nfa_finalStates[i]);
    }

    printf("Кол-во символов в алфавите: ");
    scanf("%d", &nfa_alphabetCount);

    getchar();
    printf("Введите символы алфавита:\n");
    for(i = 0; i < nfa_alphabetCount; i++) {
        scanf("%c", &nfa_alphabets[i]);
        getchar();
    }

    nfa_alphabets[i]='z';

    printf("Введите переходы: (-1 - для выхода)\n");

    for(i = 0; i < nfa_stateCount; i++) {
        for(j = 0; j <= nfa_alphabetCount; j++) {
            nfa_states[i].transitions[j].sym = nfa_alphabets[j];
            int k = 0;
            ans = 'y';

            while(ans == 'y') {
                printf("move(%d,%c) : ",i, nfa_alphabets[j]);
                scanf("%d", &nfa_states[i].transitions[j].toStates[k++]);

                if(nfa_states[i].transitions[j].toStates[k - 1] == -1) {
                    k--; 
                    ans = 'n';
                    break;
                }
            }
            nfa_states[i].transitions[j].numberOfTransitions = k;
        }
    }

    // Конвертация
    i = 0;
    dfa_stateCount = 0;
    fin = 0;

    pushA(nfa_startState);
    lambda_closure(peekA());
    toString();

    dfa_states[dfa_stateCount].name = 'A';
    dfa_stateCount++;
    strcpy(dfa_states[0].StateString, temp);

    while(i < dfa_stateCount) {
        for(j = 0; j < nfa_alphabetCount; j++) {
            fin = 0;
            copy(i);

            // ложим в stackA все следующие ходы
            while(stackB_top != -1) {
                move(popB(), j);
            }

            // находим все epsilon closure из peekA(), и ложим в stackB
            while(stackA_top != -1) {
                lambda_closure(peekA());
            }

            // строим temp из stackB
            toString();

            int k = 0;
            for(k = 0; k < dfa_stateCount; k++) {
                // если мы уже рассмотрели это состояние
                if((strcmp(temp, dfa_states[k].StateString) == 0)) {
                    // даем им названия и завершаем цикл
                    dfa_states[i].trans[j] = dfa_states[k].name;
                    break;
                }
            }

            if(k == dfa_stateCount) {
                dfa_stateCount++;

                // ищем среди финальных состояний
                for(k = 0; k < nfa_finalStateCount; k++) {

                    // если в stackB есть финальное состояние
                    fin = seek(stackB, stackB_top, nfa_finalStates[k]);
                    if(fin == 1) {
                        // то помечаем последнее состояние из dfa_states как конечное
                        dfa_states[dfa_stateCount - 1].is_final = 1;
                        break;
                    }                 
                }

                strcpy(dfa_states[dfa_stateCount - 1].StateString, temp);
                dfa_states[dfa_stateCount - 1].name = 'A' + dfa_stateCount - 1;
                dfa_states[i].trans[j] = dfa_states[dfa_stateCount - 1].name;
            }
        }
        i++;
    }

    display_DFA();
}
