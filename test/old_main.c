#include<stdio.h>
#include<stdlib.h>
#include<string.h>

struct AlphaNode {
    char letter;
    struct AlphaNode *next;
    struct AlphaNode *prev; 
};

// typedef struct {
//     float x;
//     float y;
//     struct Alpha* letters;
// } Cell;

struct AlphaNode* createNode(char letter) {
    struct AlphaNode* newNode = (struct AlphaNode*)malloc(sizeof(struct AlphaNode));
    if (newNode == NULL) {
        printf("memory allocation failed\n");
        exit(1);
    }
    newNode->letter = letter;
    newNode->next = NULL;
    newNode->prev = NULL;
    return newNode;
}

void printList(struct AlphaNode* head) {
    struct AlphaNode* temp = head;
    printf("Forward traversal: ");
    while(temp != NULL) {
        printf("%d ", temp->letter);
        if (temp->next == NULL) break;
        temp = temp->next;
    }
    printf("\n");

    printf("Reverse traversal: ");
    while(temp != NULL) {
        printf("%d ", temp->letter);
        temp = temp->prev;
    }
    printf("\n");
}

void freeList(struct AlphaNode* head) {
    struct AlphaNode* p;
    while(p != NULL) {
        p = head;
        head = head->next;
        free(p);
    }
}

void print1d(char *v, int size) {
    for(int i = 0; i < size; i++) {
        printf("%c, ", v[i]);
    }
    printf("\n");
}

char* init_letters(int start, int end) {
    int i = 0;
    size_t size = (end - start) + 1;
    printf("Letters array size: %d\n", (int)size);
    char* letters = (char*)malloc((int)size*sizeof(char));
    if (letters != NULL) {
        while(i <= size) {
            *(letters + i) = start;
            start++;
            i++;
        }
        print1d(letters, size);
        return letters;
    }
    return NULL;
}


int main(void)
{
    int start = 65; // A
    int end = 90; // Z

    char* letters = init_letters(start, end);

    struct AlphaNode* head = createNode(letters[0]);
    struct AlphaNode* temp = head;
    while(temp != NULL) {
        for(int i = 0; i < strlen(letters); i++) {
            if (temp->next == NULL) {
                break;
            }
            temp = createNode(letters[i]);
            temp = temp->next;
        }
        head = temp;
    }

    printList(head);


    return 0;
}