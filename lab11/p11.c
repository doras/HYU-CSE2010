#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct _Node {
    int key; // immutable
    int idx; // immutable
    int dist; // mutable when path is changed
    int heap_idx; // mutable when swapped in heap
    struct _Node* prev; // mutable when path is changed
} Node;

typedef struct {
    int capacity;
    int size;
    Node** arr;
    int (*cmp_func)(Node*,Node*);
} Heap;

int find_idx(Node** nodes, int len, int key);
Node* make_node(int key, int idx);
void free_node(Node* node);
void perc_up(Heap* heap, int idx);
void perc_down(Heap* heap, int index);
void swap(Node** lhs, Node** rhs);
int is_empty(Heap* pq);
void delete(Heap* pq);
Heap* build_heap(Node** data_arr, int len, int(*cmp_func)(Node*,Node*));
void delete_heap(Heap* pq);
int min(Node* left, Node* right);
Node* front(Heap* pq);
Node** dijkstra(int** adjacency, Node** nodes, int count, int src);

int main()
{
    FILE* input = fopen("input.txt", "r");
    FILE* output = fopen("output.txt", "w");
    int i, j;

    char str[401];
    fgets(str, 401, input);

    char *str_ptr = str;
    int read_byte;
    int num;
    Node* nodes[100];
    int** adjacency;
    int count = 0;
    while(sscanf(str_ptr, "%d%n", &num, &read_byte) == 1) {
        nodes[count] = make_node(num, count);
        count += 1;
        str_ptr += read_byte;
    }

    adjacency = malloc(sizeof(int*) * count);
    for(i = 0; i < count; ++i) {
        adjacency[i] = malloc(sizeof(int) * count);
        for(j = 0; j < count; ++j) {
            adjacency[i][j] = INT_MAX;
        }
    }

    int src, dst, weight;
    while(fscanf(input, "%d%*c%d%*c%d", &src, &dst, &weight) == 3) {
        adjacency[find_idx(nodes, count, src)][find_idx(nodes, count, dst)] = weight;
    }

    Node** prev = dijkstra(adjacency, nodes, count, src);
    
    Node* now_node = nodes[find_idx(nodes, count, dst)];
    int* result = malloc(sizeof(int) * count);
    for(i = 0; i < count; ++i) {
        result[i] = now_node->key;
        now_node = prev[now_node->idx];
        if(now_node == NULL) break;
    }

    if(result[i] != src) {
        fprintf(output, "no path\n");
    } else {
        for(; i >= 0; --i) {
            fprintf(output, "%d ", result[i]);
        }
    }

    free(prev);
    free(result);
    for(i = 0; i < count; ++i) {
        free(adjacency[i]);
        free_node(nodes[i]);
    }
    free(adjacency);

    fclose(output);
    fclose(input);
    return 0;
}

// swap the Node* values and its heap_idx
void swap(Node** lhs, Node** rhs)
{
    Node* temp = *lhs;
    *lhs = *rhs;
    *rhs = temp;

    int idx_temp = (*lhs)->heap_idx;
    (*lhs)->heap_idx = (*rhs)->heap_idx;
    (*rhs)->heap_idx = idx_temp;
}

// return index of given key in given nodes array
int find_idx(Node** nodes, int len, int key)
{
    int i;
    for(i = 0; i < len; ++i) {
        if(nodes[i]->key == key) return i;
    }
    return -1;
}

// create node that has given key
Node* make_node(int key, int idx)
{
    Node* result = malloc(sizeof(Node));
    result->key = key;
    result->prev = NULL;
    result->dist = INT_MAX;
    result->heap_idx = -1;
    result->idx = idx;
    return result;
}

// deallocate given node
void free_node(Node* node)
{
    free(node);
}

// 주어진 heap의 index를 바탕으로 percolate up한다.
void perc_up(Heap* heap, int idx)
{
    int i;
    Node* value = heap->arr[idx];
    for(i = idx; i/2 && !heap->cmp_func(heap->arr[i/2], value); i /= 2) {
        heap->arr[i] = heap->arr[i/2];
        heap->arr[i]->heap_idx = i;
    }
    heap->arr[i] = value;
    value->heap_idx = i;
}

// percolating elements in given index until heap satisfing partial ordering
void perc_down(Heap* heap, int index)
{
    int i;
    int len = heap->size;
    int temp;
    Node* value = heap->arr[index];
    for(i = index; 2*i <= len; i = temp) {
        temp = 2*i;
        if(temp != len && !heap->cmp_func(heap->arr[temp], heap->arr[temp + 1])) {
            ++temp;
        }
        if(heap->cmp_func(value, heap->arr[temp])) break;
        heap->arr[i] = heap->arr[temp];
        heap->arr[temp]->heap_idx = i;
    }
    heap->arr[i] = value;
    value->heap_idx = i;
}

// If given heap is empty, return 1. Else return 0.
int is_empty(Heap* pq)
{
    return pq->size <= 0;
}

// delete the root node of given heap
void delete(Heap* pq)
{
    if(is_empty(pq)) return;
    swap(&pq->arr[1], &pq->arr[pq->size--]);
    perc_down(pq, 1);
}

// heap에 data로 넣고 싶은 int 배열과 그 길이를 argument로 받아서
// heap을 할당하고 그 주소값을 return함.
Heap* build_heap(Node** data_arr, int len, int(*cmp_func)(Node*,Node*))
{
    int i, j;
    Node* temp, value;
    Heap *heap = malloc(sizeof(Heap));

    heap->capacity = len;
    heap->size = len;
    heap->arr = malloc(sizeof(Node*) * (len + 1));
    heap->cmp_func = cmp_func;
    for(i = 0; i < len; ++i) {
        heap->arr[i+1] = data_arr[i];
        heap->arr[i+1]->heap_idx = i+1;
    }

    for(i = len / 2; i >= 1; --i) {
        perc_down(heap, i);
    }
    return heap;
}

// if dist of left node is larger than dist of right node, return 0
// else return 1
int min(Node* left, Node* right)
{
    return left->dist <= right->dist;
}

// return the key of root node of given heap
Node* front(Heap* pq)
{
    return pq->arr[1];
}

// argument로 받은 heap의 arr과 heap자체를 free함.
void delete_heap(Heap* pq)
{
    if(pq == NULL) return;
    free(pq->arr);
    free(pq);
}

// perform dijkstra with given adjacency matrix, vertax, src. and return Node* array
Node** dijkstra(int** adjacency, Node** nodes, int count, int src)
{
    int i;
    nodes[find_idx(nodes, count, src)]->dist = 0;
    
    Heap* heap = build_heap(nodes, count, min);
    Node* now_node;
    while(!is_empty(heap)) {
        now_node = front(heap);
        delete(heap);
        if(now_node->dist == INT_MAX) break;
        for(i = 0; i < count; ++i) {
            if(adjacency[now_node->idx][i] < INT_MAX && nodes[i]->dist > now_node->dist + adjacency[now_node->idx][i]) {
                nodes[i]->dist = now_node->dist + adjacency[now_node->idx][i];
                nodes[i]->prev = now_node;
                perc_up(heap, nodes[i]->heap_idx);
            }
        }
    }

    Node** result = malloc(sizeof(Node*) * count);
    for(i = 0; i < count; ++i) {
        result[i] = nodes[i]->prev;
    }

    delete_heap(heap);
    return result;
}