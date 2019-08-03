#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef int T;

void swap(T *a, T *b);
T pivot_leftmost(T *arr, int left, int right);
T pivot_rightmost(T *arr, int left, int right);
T pivot_middle(T *arr, int left, int right);
void quick_sort(T *arr, int left, int right, T (*pivot_func)(T*, int, int));
void quick_sort_with_log(FILE *output, T *arr, int left, int right, T (*pivot_func)(T*, int, int));

int main()
{
    FILE *input = fopen("input2.txt", "r");
    FILE *output = fopen("output.txt", "w");

    char option[10];
    T (*pivot_func)(T*, int, int);
    int len;
    int *arr = NULL;
    int i;

    while(fscanf(input, "%10s", option) == 1) {
        if(strcmp(option, "leftmost") == 0) pivot_func = pivot_leftmost;
        else if(strcmp(option, "rightmost") == 0) pivot_func = pivot_rightmost;
        else if(strcmp(option, "middle") == 0) pivot_func = pivot_middle;
        else return 1;

        fscanf(input, "%d", &len);

        if(arr) free(arr);
        arr = malloc(sizeof(int) * len);
        for(i = 0; i < len; ++i) {
            fscanf(input, "%d", arr + i);
        }
        fprintf(stdout, "%s:\n", option);
        fprintf(output, "%s\n", option);

        quick_sort_with_log(stdout, arr, 0, len - 1, pivot_func);

        fprintf(stdout, "\nresult\n");
        for(i = 0; i < len; ++i) {
            fprintf(stdout, "%d ", arr[i]);
            fprintf(output, "%d ", arr[i]);
        }
        fprintf(stdout, "\n\n");
        fprintf(output, "\n\n");
    }

    free(arr);
    fclose(input);
    fclose(output);
    return 0;
}

// swap two values stored in a and b.
void swap(T *a, T *b)
{
    T tmp = *a;
    *a = *b;
    *b = tmp;
}

// select leftmost pivot.
// selected pivot is moved to rightmost index and returned.
T pivot_leftmost(T *arr, int left, int right)
{
    swap(arr + left, arr + right);
    return arr[right];
}

// select rightmost pivot.
// selected pivot is moved to rightmost index and returned.
T pivot_rightmost(T *arr, int left, int right)
{
    return arr[right];
}

// select middle pivot.
// selected pivot is moved to rightmost index and returned.
T pivot_middle(T *arr, int left, int right)
{
    swap(arr + (left + right) / 2, arr + right);
    return arr[right];
}

//
void quick_sort(T *arr, int left, int right, T (*pivot_func)(T*, int, int))
{
    if(left >= right) return;
    T pivot_value = pivot_func(arr, left, right);
    
    int left_idx = left;
    int right_idx = right - 1;

    while(1) {
        while(left_idx <= right && arr[left_idx] < pivot_value) ++left_idx;
        while(right_idx >= left && arr[right_idx] > pivot_value) ++right_idx;
        if(left_idx < right_idx) {
            swap(arr + left_idx, arr + right_idx);
        } else {
            break;
        }
    }

    swap(arr + left_idx, arr + right);

    quick_sort(arr, left, right_idx, pivot_func);
    quick_sort(arr, left_idx + 1, right, pivot_func);
}

//
void quick_sort_with_log(FILE *output, T *arr, int left, int right, T (*pivot_func)(T*, int, int))
{
    if(left >= right) return;
    T pivot_value = pivot_func(arr, left, right);
    
    int left_idx = left;
    int right_idx = right - 1;

    while(1) {
        while(left_idx <= right && arr[left_idx] < pivot_value) ++left_idx;
        while(right_idx >= left && arr[right_idx] > pivot_value) --right_idx;

        if(left_idx < right_idx) {
            swap(arr + left_idx, arr + right_idx);
        } else {
            break;
        }
    }

    swap(arr + left_idx, arr + right);

    int i;
    fprintf(output, "<");
    for(i = left; i <= right_idx; ++i) {
        fprintf(output, "%d ", arr[i]);
    }
    fprintf(output, "><%d ><", arr[i]);
    for(i = i + 1; i <= right; ++i) {
        fprintf(output, "%d ", arr[i]);
    }
    fprintf(output, ">\n");

    quick_sort_with_log(output, arr, left, right_idx, pivot_func);
    quick_sort_with_log(output, arr, left_idx + 1, right, pivot_func);
}
