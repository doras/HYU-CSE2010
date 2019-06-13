#include <stdio.h>
#include <stdlib.h>

void merge_sort_recursive(int* arr, int* tmp_arr, int left, int right);
void merge_sort_recursive_with_log(FILE* output, int* arr, int* tmp_arr, int left, int right);
void _merge(int* arr, int* tmp_arr, int left, int right, int right_end);

int main()
{
    FILE* input = fopen("input.txt", "r");
    FILE* output = fopen("output.txt", "w");

    int num_of_input;
    int temp_num;
    int i;

    fscanf(input, "%d", &num_of_input);
    int* arr = malloc(sizeof(int) * num_of_input);
    int* tmp_arr = malloc(sizeof(int) * num_of_input);

    for(i = 0; i < num_of_input; ++i) {
        fscanf(input, "%d", arr + i);
    }
    
    fprintf(output, "input :\n");
    for(i = 0; i < num_of_input; ++i) {
        fprintf(output, "%d ", arr[i]);
    }

    fprintf(output, "\n\nrecursive :\n");

    merge_sort_recursive_with_log(output, arr, tmp_arr, 0, num_of_input - 1);

    free(tmp_arr);
    free(arr);
    fclose(output);
    fclose(input);

    return 0;
}

// Perform merge sort for given arr, leftmost index and rightmost index is given left and right, respectively.
// Given tmp_arr is used as temporary array for merge sort.
void merge_sort_recursive(int* arr, int* tmp_arr, int left, int right)
{
    if(left >= right) return;
    
    int center = (right + left) / 2;
    merge_sort_recursive(arr, tmp_arr, left, center);
    merge_sort_recursive(arr, tmp_arr, center + 1, right);
    _merge(arr, tmp_arr, left, center + 1, right);
}

// Perform merge sort for given arr, leftmost index and rightmost index is given left and right, respectively.
// Given tmp_arr is used as temporary array for merge sort.
// This function is exactly same as merge_sort_recursive except that it prints logs.
void merge_sort_recursive_with_log(FILE* output, int* arr, int* tmp_arr, int left, int right)
{
    if(left >= right) return;
    
    int center = (right + left) / 2;
    merge_sort_recursive_with_log(output, arr, tmp_arr, left, center);
    merge_sort_recursive_with_log(output, arr, tmp_arr, center + 1, right);
    _merge(arr, tmp_arr, left, center + 1, right);

    int i;
    for(i = left; i <= right; ++i) {
        fprintf(output, "%d ", arr[i]);
    }
    fprintf(output, "\n");
}

// Merge given two ranges of given arr for merge sort, using given temporary arr tmp_arr.
// This function is internal function for merge sort functions.
void _merge(int* arr, int* tmp_arr, int left, int right, int right_end)
{
    int left_end = right - 1;
    int tmp_idx = left;
    int num_of_elem = right_end - left + 1;

    while(left <= left_end && right <= right_end) {
        if(arr[left] <= arr[right]) {
            tmp_arr[tmp_idx++] = arr[left++];
        } else {
            tmp_arr[tmp_idx++] = arr[right++];
        }
    }

    while(left <= left_end) {
        tmp_arr[tmp_idx++] = arr[left++];
    }
    while(right <= right_end) {
        tmp_arr[tmp_idx++] = arr[right++];
    }

    int i;
    for(i = 0; i < num_of_elem; ++i, --right_end) {
        arr[right_end] = tmp_arr[right_end];
    }
}
