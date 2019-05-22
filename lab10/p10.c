#include <stdio.h>
#include <stdlib.h>

#define DEBUG(x) printf("line : %d code : %d\n", __LINE__, x);

typedef struct {
    int size;
    int *node;
    int **matrix;
} Graph;

typedef struct {
    int *key;
    int first;
    int rear;
    int qsize;
    int max_queue_size;
} Queue;

Graph* create_graph(int* nodes, int len);
void insert_edge(Graph* graph, int a, int b);
int* top_sort(Graph* graph);
Queue* make_new_queue(int x);
void enqueue(Queue* q, int x);
int dequeue(Queue* q);
void delete_queue(Queue* q);
int find_index_of_key(int* keys, int len, int value);
int* bubble_sorted_order(int* arr, int len);
void swap(int* lhs, int* rhs);
void print_matrix(FILE* output, Graph* graph);
void delete_graph(Graph* graph);

int main()
{
    FILE* input = fopen("input.txt", "r");
    FILE* output = fopen("output.txt","w");

    char str[1001];
    int num, read_byte, count = 0;
    char* ptr = str;
    int nodes[501];
    int* top_sorted_arr;
    fgets(str, 1001, input);

    while(sscanf(ptr, "%d%n", &num, &read_byte) == 1) {
        nodes[count++] = num;
        ptr += read_byte;
    }

    Graph* graph = create_graph(nodes, count);

    int src, dst;
    while(fscanf(input, "%d%*c%d", &src, &dst) == 2) {
        insert_edge(graph, src, dst);
    }

    print_matrix(output, graph);
    fprintf(output, "\n\n\n\n");
    
    int i;
    top_sorted_arr = top_sort(graph);
    fprintf(output, "TopSort Result :");
    if(top_sorted_arr) {
        for(i = 0; i < graph->size; ++i) fprintf(output, " %d", top_sorted_arr[i]);
        free(top_sorted_arr);
    } else {
        fprintf(output, " input graph is DAG.");
    }
    fprintf(output, "\n");

    delete_graph(graph);

    fclose(input);
    fclose(output);
    return 0;
}

// arguemt로 받은 len 길이의 int 배열 nodes에 저장되어 있는 vertex를 갖는 graph를 dynamic하게 create함.
Graph* create_graph(int* nodes, int len)
{
    Graph* result = malloc(sizeof(Graph));
    result->size = len;
    result->node = malloc(sizeof(int) * len);
    result->matrix = malloc(sizeof(int*) * len);
    int i, j;
    for(i = 0; i < len; ++i) {
        result->matrix[i] = malloc(sizeof(int)*len);
        for(j = 0; j < len; ++j) {
            result->matrix[i][j] = 0;
        }
    }

    for(i = 0; i < len; ++i) {
        result->node[i] = nodes[i];
    }
    return result;
}

// graph에 vertex a에서 b로의 edge를 추가해줌.
void insert_edge(Graph* graph, int a, int b)
{
    graph->matrix[find_index_of_key(graph->node, graph->size, a)][find_index_of_key(graph->node, graph->size, b)] = 1;
}

// topological sort를 수행하여 결과가 담겨있는 int array를 return함.
// dynamic allocation된 array를 return.
int* top_sort(Graph* graph)
{
    int i, j, now_idx, temp, count = 0;
    int* result_arr = malloc(sizeof(int) * graph->size);
    int* indegree = malloc(sizeof(int) * graph->size);
    Queue* queue = make_new_queue(graph->size);
    int* order = bubble_sorted_order(graph->node, graph->size);

    for(i = 0; i < graph->size; ++i) {
        now_idx = order[i];
        indegree[now_idx] = 0;
        for(j = 0; j < graph->size; ++j) {
            indegree[now_idx] += graph->matrix[j][now_idx];
        }
        if(!indegree[now_idx]) enqueue(queue, now_idx);
    }

    while(queue->qsize) {
        temp = dequeue(queue);
        result_arr[count] = graph->node[temp];
        for(i = 0; i < graph->size; ++i) {
            now_idx = order[i];
            if(graph->matrix[temp][now_idx]) {
                if(--indegree[now_idx] == 0) {
                    enqueue(queue, now_idx);
                }
            }
        }
        ++count;
    }

    for(i = 0; i < graph->size; ++i) {
        if(indegree[i] != 0) {
            free(result_arr);
            result_arr = NULL;
            break;
        }
    }

    delete_queue(queue);
    free(indegree);
    free(order);
    return result_arr;
}

// x만큼의 max_size를 가지는 queue를 동적으로 생성하여 주소를 반환함.
Queue* make_new_queue(int x)
{
    Queue* result = malloc(sizeof(Queue));
    result->key = malloc(sizeof(int) * x);
    result->max_queue_size = x;
    result->first = 1;
    result->rear = 0;
    result->qsize = 0;
    return result;
}

// argument로 받은 Queue에 x값을 enqueue함.
void enqueue(Queue* q, int x)
{
    if(q->qsize == q->max_queue_size) {
        return;
    }
    q->rear = (q->rear + 1) % q->max_queue_size;
    q->key[q->rear] = x;
    ++q->qsize;
}

// argument로 받은 Queue에서 dequeue한 결과를 return함.
int dequeue(Queue* q)
{
    if(q->qsize == 0) return -1;

    int result = q->key[q->first];
    q->first = (q->first + 1) % q->max_queue_size;
    --q->qsize;
    return result;
}

// argument로 받은 Queue를 free함.
void delete_queue(Queue* q)
{
    free(q->key);
    free(q);
}

// argument로 받은 int array keys에서 value에 해당하는 값을 찾아 해당 index를 return함.
// 만약 없으면 -1 return.
int find_index_of_key(int* keys, int len, int value)
{
    int i;
    for(i = 0; i < len; ++i) {
        if(keys[i] == value) return i;
    }
    return -1;
}

// bubble sort 방식으로 int 배열 arr의 값들을 sorting된 순서로 접근할 수 있는 index의 array를 return함.
// This function returns dynamically allocated array.
int* bubble_sorted_order(int* arr, int len)
{
    int i, j;

    int* result = malloc(sizeof(int) * len);
    int* temp = malloc(sizeof(int) * len);
    for(i = 0; i < len; ++i) {
        result[i] = i;
        temp[i] = arr[i];
    }

    for(i = len - 1; i > 0; --i) {
        for(j = 0; j < i; ++j) {
            if(temp[j] > temp[j+1]) {
                swap(&result[j], &result[j+1]);
                swap(&temp[j], &temp[j+1]);
            }
        }
    }
    free(temp);
    return result;
}

// lhs가 pointing하는 값과 rhs가 pointing하는 값을 서로 바꿈.
void swap(int* lhs, int* rhs)
{
    int temp;
    temp = *lhs;
    *lhs = *rhs;
    *rhs = temp;
}

// graph의 matrix를 과제설명서의 예시와 같은 format으로 출력함.
void print_matrix(FILE* output, Graph* graph)
{
    int i, j;
    fprintf(output, "Adjacency matrix\n ");
    for(i = 0; i < graph->size; ++i) {
        fprintf(output, " %d", graph->node[i]);
    }
    for(i = 0; i < graph->size; ++i) {
        fprintf(output, "\n%d", graph->node[i]);
        for(j = 0; j < graph->size; ++j) {
            fprintf(output, " %d", graph->matrix[i][j]);
        }
    }
}

// graph를 free함.
void delete_graph(Graph* graph)
{
    int i;
    for(i = 0; i < graph->size; ++i) {
        free(graph->matrix[i]);
    }
    free(graph->matrix);
    free(graph->node);
    free(graph);
}