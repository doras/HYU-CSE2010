#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int cap;
    int size;
    int *arr;
} Heap;

Heap* make_heap(int);
Heap* build_heap(int*, int, int(*)(int,int));
void insert(FILE*, int, Heap*, int(*)(int,int));
void find_with_io(FILE*, int, Heap*);
int find(int, Heap*);
void print(FILE*, Heap*);
void delete_heap(Heap*);
int max(int, int);
int min(int, int);
void back_propagate(Heap*, int, int(*)(int,int));

int main()
{
    FILE *input = fopen("input.txt", "r");
    FILE *output = fopen("output.txt", "w");

    int cap;
    Heap *pq;
    char mode[2];
    int temp_num;

    fscanf(input, "%d", &cap);
    pq = make_heap(cap);

    while(fscanf(input, "%s", mode) == 1) {
        switch(mode[0]) {
            case 'i':
                fscanf(input, "%d", &temp_num);
                insert(output, temp_num, pq, max);
                break;
            case 'f':
                fscanf(input, "%d", &temp_num);
                find_with_io(output, temp_num, pq);
                break;
            case 'p':
                print(output, pq);
                break;
        }
    }

    delete_heap(pq);
    fclose(input);
    fclose(output);
    return 0;
}

// 주어진만큼의 capacity를 가지는 heap을 할당한 후 그 주소를 return.
Heap* make_heap(int cap)
{
    Heap *pq = malloc(sizeof(Heap));
    pq->cap = cap;
    pq->size = 0;
    pq->arr = malloc(sizeof(int) * cap);
    return pq;
}

// heap에 data로 넣고 싶은 int 배열과 그 길이를 argument로 받아서
// heap을 할당하고 그 주소값을 return함.
Heap* build_heap(int *data_arr, int len, int(*cmp_func)(int,int))
{
    Heap *pq;
    int i, j;
    int temp, value;
    pq = malloc(sizeof(Heap));
    if(data_arr == NULL) {
        pq->cap = 10;
        pq->size = 0;
        pq->arr = malloc(sizeof(int) * pq->cap + 1);    
        return pq;
    }
    pq->cap = (len + len * 0.25) > 10 ? (len + len * 0.25) : 10;
    pq->size = len;
    pq->arr = malloc(sizeof(int) * pq->cap + 1);
    for(i = 0; i < len; ++i) {
        pq->arr[i+1] = data_arr[i];
    }
    for(i = len / 2; i >= 1; --i) {
        value = pq->arr[i];
        for(j = i; 2*j <= len; j = temp) {
            temp = 2*j;
            if(temp != len && cmp_func(pq->arr[temp], pq->arr[temp + 1]) < 0) {
                ++temp;
            }
            if(cmp_func(pq->arr[j], pq->arr[temp]) >= 0) break;
            pq->arr[j] = pq->arr[temp];
            pq->arr[temp] = value;
        }
        pq->arr[j] = value;
    }
    return pq;
}

// 주어진 value를 heap에 cmp_func의 관점에서 insert함.
void insert(FILE *output, int value, Heap *pq, int(*cmp_func)(int,int))
{
    int idx = find(value, pq);
    if(idx) {
        fprintf(output, "%d is already in the heap.\n", value);
        return;
    }
    pq->arr[++pq->size] = value;
    back_propagate(pq, pq->size, cmp_func);
    fprintf(output, "insert %d\n", value);
}

// find함수를 이용해서 heap안에 value에 해당하는 값의 여부를 fprint한다.
void find_with_io(FILE *output, int value, Heap *pq)
{
    if(find(value, pq)) {
        fprintf(output, "%d is in the heap.\n", value);
    } else {
        fprintf(output, "%d is not in the heap.\n", value);
    }
}

// Heap안에서 value값이 있는지 확인하고 있다면 그 index, 없다면 0을 return한다.
int find(int value, Heap *pq)
{
    int i;
    for(i = 1; i <= pq->size; ++i) {
        if(pq->arr[i] == value) break;
    }
    if(i <= pq->size) {
        return i;
    }
    return 0;
}

// argument로 받은 Heap의 data를 순서대로 출력함.
void print(FILE* output, Heap *pq)
{
    int i;
    for(i = 1; i <= pq->size; ++i) {
        fprintf(output, "%d ", pq->arr[i]);
    }
    fprintf(output, "\n");
}

// argument로 받은 heap의 arr과 heap자체를 free함.
void delete_heap(Heap *pq)
{
    if(pq == NULL) return;
    free(pq->arr);
    free(pq);
}

// max 관점에서 parent와 child가 올바른 순서로 배치되어 있으면 양수나 0, 아니면 음수를 return
int max(int parent, int child)
{
    return parent - child;
}

// min 관점에서 parent와 child가 올바른 순서로 배치되어 있으면 양수나 0, 아니면 음수를 return
int min(int parent, int child)
{
    return child - parent;
}

// 주어진 heap의 index를 바탕으로 cmp_func관점에서 percolate up한다.
void back_propagate(Heap *pq, int idx, int(*cmp_func)(int,int))
{
    int i;
    int value = pq->arr[idx];
    for(i = idx; i/2 && cmp_func(pq->arr[i/2], value) < 0; i /= 2) {
        pq->arr[i] = pq->arr[i/2];
    }
    pq->arr[i] = value;
}