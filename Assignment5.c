/*
COP 3502 Assignment 5: Theater Inventory Progam
© 4/10/2024 Giorgio Torregrosa
*/

//libraries
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXLEN 19
#define TABLESIZE 300007

//structs in use
typedef struct item {
    char name[MAXLEN + 1];
    int quantity;
    int saleprice;
} item;

typedef struct node {
    item* iPtr;
    struct node* next;
} node;

typedef struct hashtable {
    node** lists;
    int size;
} hashtable;

int hashfunc(char* word, int size); //given hashfunction that returns a valid index
void changePrice(hashtable* table, char* itemName, int newPrice, int* cash, int* complexity); //changes the price of a given item
void sellItem(hashtable* table, char* itemName, int quantity, int* cash, int* complexity); //sells an item
void buySupplies(hashtable* table, char* itemName, int quantity, int totalPrice, int* cash, int* complexity); //buys an item
item* findItem(hashtable* table, char* itemName, int* complexity); //finds an item
void insertItem(hashtable* table, char* itemName, int quantity, int saleprice, int* complexity); //inserts an item into the hashtable
hashtable* createHashTable(int size); //initializes a hash table
void freeHashTable(hashtable* table); // frees all memory associated with the hashtable

//START
int main(int argc, char* argv[]) {

    int n = 0;

    if (scanf("%d", &n) != 1) {
        return EXIT_FAILURE;
    };

    hashtable* inventory = createHashTable(TABLESIZE);
    int cash = 100000;
    int complexity = 0;

    for (int i = 0; i < n; i++) {
        char command[15];
        if (scanf("%s", command) != 1) {
            return EXIT_FAILURE;
        };

        if (strcmp(command, "buy") == 0) {
            char itemName[MAXLEN + 1];
            int quantity, totalPrice;
            if (scanf("%s %d %d", itemName, &quantity, &totalPrice) != 3) {
                return EXIT_FAILURE;
            }
            buySupplies(inventory, itemName, quantity, totalPrice, &cash, &complexity);
        }
        else if (strcmp(command, "sell") == 0) {
            char itemName[MAXLEN + 1];
            int quantity;
            if (scanf("%s %d", itemName, &quantity) != 2) {
                return EXIT_FAILURE;
            }
            sellItem(inventory, itemName, quantity, &cash, &complexity);
        }
        else if (strcmp(command, "change_price") == 0) {
            char itemName[MAXLEN + 1];
            int newPrice;
            if (scanf("%s %d", itemName, &newPrice) != 2) {
                return EXIT_FAILURE;
            }
            changePrice(inventory, itemName, newPrice, &cash, &complexity);
        }
    }

    printf("%d\n%d\n", cash, complexity);

    freeHashTable(inventory);

    return 0;
}//END


hashtable* createHashTable(int size) {
    hashtable* table = (hashtable*)malloc(sizeof(hashtable));
    if (!table) {
        return NULL;
    }
    table->lists = (node**)malloc(sizeof(node*) * size);
    if (!table->lists) {
        free(table);
        return NULL;
    }
    table->size = size;
    for (int i = 0; i < size; i++) {
        table->lists[i] = NULL;
    }
    return table;
}

void freeHashTable(hashtable* table) {
    for (int i = 0; i < table->size; i++) {
        node* current = table->lists[i];
        while (current != NULL) {
            node* temp = current;
            current = current->next;
            free(temp);
        }
    }
    free(table->lists);
    free(table);
}

int hashfunc(char* word, int size) {
    int len = strlen(word);
    int res = 0;
    for (int i = 0; i < len; i++) {
        res = (1151 * res + (word[i] - 'a')) % size;
    }
    return res;
}

void changePrice(hashtable* table, char* itemName, int newPrice, int* cash, int* complexity) {
    int localComplexity = 0;
    item* existingItem = findItem(table, itemName, &localComplexity);
    *complexity += localComplexity;

    if (existingItem != NULL) {
        existingItem->saleprice = newPrice;
    }
}

void sellItem(hashtable* table, char* itemName, int quantity, int* cash, int* complexity) {
    int localComplexity = 0;
    item* existingItem = findItem(table, itemName, &localComplexity);
    *complexity += localComplexity;

    if (existingItem != NULL) {
        int actualSold = quantity > existingItem->quantity ? existingItem->quantity : quantity;
        existingItem->quantity -= actualSold;
        *cash += actualSold * existingItem->saleprice;
        printf("%s %d %d\n", itemName, existingItem->quantity, *cash);
    }
}

void buySupplies(hashtable* table, char* itemName, int quantity, int totalPrice, int* cash, int* complexity) {
    int localComplexity = 0;
    item* existingItem = findItem(table, itemName, &localComplexity);
    if (existingItem) {
        existingItem->quantity += quantity;
    }
    else {
        insertItem(table, itemName, quantity, 0, &localComplexity);
    }
    *cash -= totalPrice;
    printf("%s %d %d\n", itemName, existingItem ? existingItem->quantity : quantity, *cash);
    *complexity += localComplexity;
}

item* findItem(hashtable* table, char* itemName, int* complexity) {
    int index = hashfunc(itemName, table->size);
    node* current = table->lists[index];
    int pos = 1; // Start pos at 1 for correct 1-based indexing
    while (current != NULL) {
        if (strcmp(current->iPtr->name, itemName) == 0) {
            *complexity += pos; // Add the position to the complexity
            return current->iPtr;
        }
        current = current->next;
        pos++;
    }
    *complexity += pos; // If not found, add the pos (length + 1) to complexity for insertion
    return NULL;
}

void insertItem(hashtable* table, char* itemName, int quantity, int saleprice, int* complexity) {
    item* newItem = (item*)malloc(sizeof(item));
    if (!newItem) {
        return; 
    }
    strcpy(newItem->name, itemName);
    newItem->quantity = quantity;
    newItem->saleprice = saleprice;

    int index = hashfunc(itemName, table->size);

    node* newNode = (node*)malloc(sizeof(node));
    if (!newNode) {
        free(newItem); // Clean up item if node allocation fails
        return;
    }
    newNode->iPtr = newItem;
    newNode->next = table->lists[index];
    table->lists[index] = newNode;

}
