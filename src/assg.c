/*
 * CMPUT 201 Student Submission License Version 2.0
 *
 * Copyright 2026 Yiqi Liang
 *
 * Unauthorized redistribution is forbidden under all circumstances. Use of this software without explicit authorization from the author and the CMPUT 201 Instructor is prohibited.
 *
 * This software was produced as a solution for an assignment or lab in the course CMPUT 201 - Practical Programming Methodology at the University of Alberta, Canada. This solution is confidential and remains confidential after it is submitted for grading. The course staff has the right to run plagiarism-detection tools on any code developed under this license, even beyond the duration of the course.
 *
 * Copying any part of this solution without including this copyright notice is illegal.
 *
 * If any portion of this software is included in a solution submitted for grading at an educational institution, the submitter will be subject to the plagiarism sanctions at that institution.
 *
 * This software cannot be publicly posted under any circumstances, whether by the original student or by a third party. If this software is found in any public website or public repository, the person finding it is kindly requested to immediately report, including the URL or other repository locating information, to the following email addresses:
 *
 * hktang@ualberta.ca
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "assg.h"

#define INITIAL_CAPACITY 10
#define LINE_SIZE 256

// Create a new Node, allocate memory and initialize all node values.
Node* create_node(int x, int y, int g, int h, Node *parent) {
    Node *p = malloc(sizeof(Node));

    if (p == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }

    p->x = x;
    p->y = y;
    p->g = g;
    p->h = h;
    p->f = g + h;
    p->parent = parent;

    return p;
}

// Compare two Nodes.
// Return true if a has higher priority than b.
// Compare f first, then h, x, and y.
bool smaller(Node *a, Node *b) {
    if (a->f < b->f) {
        return true;
    }

    if (a->f > b->f) {
        return false;
    }

    if (a->h < b->h) {
        return true;
    }

    if (a->h > b->h) {
        return false;
    }

    if (a->x < b->x) {
        return true;
    }

    if (a->x > b->x) {
        return false;
    }

    if (a->y < b->y) {
        return true;
    }

    return false;
}

// Initialize the MinHeap.
// Create an empty Open List using malloc.
void heap_init(MinHeap *heap) {
    heap->size = 0;
    heap->capacity = INITIAL_CAPACITY;

    heap->data = malloc(heap->capacity * sizeof(Node *));

    if (heap->data == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
}

// Heapify Up.
// Move a Node upward to maintain Min-Heap order.
void heapify_up(MinHeap *heap, int index) {
    while (index > 0) {
        int parent = (index - 1) / 2;

        if (smaller(heap->data[index], heap->data[parent])) {
            Node *temp = heap->data[index];
            heap->data[index] = heap->data[parent];
            heap->data[parent] = temp;
            index = parent;
        }
        else {
            break;
        }
    }
}

// Heapify Down.
// Move a Node downward to maintain Min-Heap order.
void heapify_down(MinHeap *heap, int index) {
    while (1) {
        int left = 2 * index + 1;
        int right = 2 * index + 2;
        int smallest = index;

        if (left < heap->size) {
            if (smaller(heap->data[left], heap->data[smallest])) {
                smallest = left;
            }
        }

        if (right < heap->size) {
            if (smaller(heap->data[right], heap->data[smallest])) {
                smallest = right;
            }
        }

        if (smallest == index) {
            break;
        }

        Node *temp = heap->data[index];
        heap->data[index] = heap->data[smallest];
        heap->data[smallest] = temp;

        index = smallest;
    }
}

// Insert a Node into the Open List.
// Resize the array if full, then heapify up.
void heap_insert(MinHeap *heap, Node *node) {
    if (node == NULL) {
        return;
    }

    if (heap->size == heap->capacity) {
        int new_capacity = heap->capacity * 2;

        Node **temp = realloc(heap->data, new_capacity * sizeof(Node *));

        if (temp == NULL) {
            fprintf(stderr, "Memory allocation failed\n");
            exit(EXIT_FAILURE);
        }

        heap->data = temp;
        heap->capacity = new_capacity;
    }

    heap->data[heap->size] = node;
    heap->size++;

    heapify_up(heap, heap->size - 1);
}

// Extract the minimum Node.
// Remove the root Node and restore Min-Heap order.
Node* heap_extract_min(MinHeap *heap) {
    if (heap->size == 0) {
        return NULL;
    }

    Node *result = heap->data[0];

    heap->size--;

    if (heap->size > 0) {
        heap->data[0] = heap->data[heap->size];
        heapify_down(heap, 0);
    }

    return result;
}

// Find a Node in the Open List.
// Return its index, or -1 if not found.
int heap_find(const MinHeap *heap, int x, int y) {
    for (int i = 0; i < heap->size; i++) {
        if (heap->data[i]->x == x && heap->data[i]->y == y) {
            return i;
        }
    }

    return -1;
}

// Decrease Key.
// Update g, h, and f, then restore heap order.
void heap_decrease_key(MinHeap *heap, int index, int new_g, int new_h) {
    if (index < 0 || index >= heap->size) {
        return;
    }

    Node *p = heap->data[index];

    p->g = new_g;
    p->h = new_h;
    p->f = new_g + new_h;

    heapify_up(heap, index);
}

// Check if the Open List is empty.
// Return true if size is zero.
bool heap_is_empty(const MinHeap *heap) {
    if (heap->size == 0) {
        return true;
    }

    return false;
}

// Free the Open List.
// Free remaining Nodes and the dynamic array.
void heap_free(MinHeap *heap) {
    for (int i = 0; i < heap->size; i++) {
        free(heap->data[i]);
    }

    free(heap->data);

    heap->data = NULL;
    heap->size = 0;
    heap->capacity = 0;
}

// Initialize the Closed List.
// Create an empty dynamic array.
void closed_list_init(ClosedList *list) {
    list->size = 0;
    list->capacity = INITIAL_CAPACITY;

    list->data = malloc(list->capacity * sizeof(Node *));

    if (list->data == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
}

// Add a Node to the Closed List.
// Resize the array if necessary.
void closed_list_add(ClosedList *list, Node *node) {
    if (node == NULL) {
        return;
    }

    if (list->size == list->capacity) {
        int new_capacity = list->capacity * 2;

        Node **temp = realloc(list->data, new_capacity * sizeof(Node *));

        if (temp == NULL) {
            fprintf(stderr, "Memory allocation failed\n");
            exit(EXIT_FAILURE);
        }

        list->data = temp;
        list->capacity = new_capacity;
    }

    list->data[list->size] = node;
    list->size++;
}

// Find a Node in the Closed List.
// Return true if the coordinates exist.
bool closed_list_contains(const ClosedList *list, int x, int y) {
    for (int i = 0; i < list->size; i++) {
        if (list->data[i]->x == x && list->data[i]->y == y) {
            return true;
        }
    }

    return false;
}

// Free the Closed List.
// Free extracted Nodes and the dynamic array.
void closed_list_free(ClosedList *list) {
    for (int i = 0; i < list->size; i++) {
        free(list->data[i]);
    }

    free(list->data);

    list->data = NULL;
    list->size = 0;
    list->capacity = 0;
}

// Main Function.
// Read milestone input and process all commands.
int main(void) {
    MinHeap open;
    ClosedList closed;

    heap_init(&open);
    closed_list_init(&closed);

    char line[LINE_SIZE];
    int x, y, g, h;

    // Find the insertion section.
    while (fgets(line, LINE_SIZE, stdin) != NULL) {
        if (strstr(line, "[O] Insert:") != NULL) {
            break;
        }
    }

    // Read and insert Nodes until "done".
    while (fgets(line, LINE_SIZE, stdin) != NULL) {
        if (strncmp(line, "done", 4) == 0) {
            break;
        }

        if (sscanf(line, " %d , %d , %d , %d", &x, &y, &g, &h) == 4) {
            Node *p = create_node(x, y, g, h, NULL);
            heap_insert(&open, p);
        }
    }

    // Read and process commands.
    while (fgets(line, LINE_SIZE, stdin) != NULL) {

        // Remove the minimum Node from Open List and move it into Closed List.
        if (strstr(line, "[O] Extract") != NULL) {
            Node *p = heap_extract_min(&open);

            if (p != NULL) {
                closed_list_add(&closed, p);
                printf("Extracted Node (%d, %d), f = %d, g = %d, h = %d\n", p->x, p->y, p->f, p->g, p->h);
            }
        }

        // Find the Node and update its g-cost while keeping the original h-cost.
        else if (strstr(line, "[O] DecreaseKey") != NULL) {
            int new_g;

            if (sscanf(line, "[O] DecreaseKey (%d, %d), new_g = %d", &x, &y, &new_g) == 3) {
                int index = heap_find(&open, x, y);

                if (index != -1) {
                    Node *p = open.data[index];
                    int old_h = p->h;

                    heap_decrease_key(&open, index, new_g, old_h);

                    printf("Decreased key for Node (%d, %d): f = %d, g = %d, h = %d\n", p->x, p->y, p->f, p->g, p->h);
                }
            }
        }

        // Search for a Node in the Open List.
        else if (strstr(line, "[O] Find") != NULL) {
            if (sscanf(line, "[O] Find (%d, %d)", &x, &y) == 2) {
                int index = heap_find(&open, x, y);

                if (index != -1) {
                    printf("Node (%d, %d) has been found\n", x, y);
                }
                else {
                    printf("Node (%d, %d) has not been found\n", x, y);
                }
            }
        }

        // Search for a Node in the Closed List.
        else if (strstr(line, "[C] Find") != NULL) {
            if (sscanf(line, "[C] Find (%d, %d)", &x, &y) == 2) {
                if (closed_list_contains(&closed, x, y)) {
                    printf("Node (%d, %d) has been found\n", x, y);
                }
                else {
                    printf("Node (%d, %d) has not been found\n", x, y);
                }
            }
        }

        // Check if Open List is empty.
        else if (strstr(line, "[O] IsEmpty") != NULL) {
            if (heap_is_empty(&open)) {
                printf("True\n");
            }
            else {
                printf("False\n");
            }
        }

        // Check if Closed List is empty.
        else if (strstr(line, "[C] IsEmpty") != NULL) {
            if (closed.size == 0) {
                printf("True\n");
            }
            else {
                printf("False\n");
            }
        }
    }

    // Free all dynamically allocated memory.
    heap_free(&open);
    closed_list_free(&closed);

    return 0;
}
