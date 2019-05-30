#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct _HashTable {
    int size;
    int* table;
    int (*hash_func)(struct _HashTable*, int);
    int (*solution)(struct _HashTable*, int, int);
} HashTable;

HashTable* make_hash_table(int size, int (*hash_func)(HashTable*, int), int (*solution)(HashTable*, int, int));
void free_hash_table(HashTable* hash);
int insert(HashTable* hash, int value);
int delete(HashTable* hash, int value);
int find(HashTable* hash, int value);
void print_hash_table(HashTable* hash, FILE* output);
int mod_hash_func(HashTable* this, int value);
int linear_sol(HashTable* this, int value, int i);
int quadratic_sol(HashTable* this, int value, int i);
int double_sol(HashTable* this, int value, int i);

int main()
{
    char solution[15];
    int num_test_case;
    int temp_num;
    char oper[2];
    int (*curr_sol)(HashTable*, int, int);
    int (*curr_hash_func)(HashTable*, int) = mod_hash_func;
    HashTable* hash_table = NULL;
    int find_result;

    FILE* input = fopen("input.txt", "r");
    FILE* output = fopen("output.txt", "w");

    fscanf(input, "%d", &num_test_case);
    for(; num_test_case > 0; --num_test_case) {
        fscanf(input, "%s", solution);
        fprintf(output, "%s\n", solution);
        if(strcmp(solution, "Linear") == 0) {
            curr_sol = linear_sol;
        } else if(strcmp(solution, "Quadratic") == 0) {
            curr_sol = quadratic_sol;
        } else if(strcmp(solution, "Double") == 0) {
            curr_sol = double_sol;
        } else return 1;

        fscanf(input, "%d", &temp_num);
        hash_table = make_hash_table(temp_num, curr_hash_func, curr_sol);
        
        while(fscanf(input, "%s", oper) == 1 && oper[0] != 'q') {
            switch(oper[0]) {
                case 'i':
                    fscanf(input, "%d", &temp_num);
                    if(insert(hash_table, temp_num) == 0) {
                        fprintf(output, "Inserted %d\n", temp_num);
                    } else {
                        fprintf(output, "Already exists\n");
                    }
                    break;
                case 'd':
                    fscanf(input, "%d", &temp_num);
                    if(delete(hash_table, temp_num) == 0) {
                        fprintf(output, "Deleted %d\n", temp_num);
                    } else {
                        fprintf(output, "%d not exists\n", temp_num);
                    }
                    break;
                case 'f':
                    fscanf(input, "%d", &temp_num);
                    find_result = find(hash_table, temp_num);
                    if(find_result >= 0) {
                        fprintf(output, "%d has index of %d\n", temp_num, find_result);
                    } else {
                        fprintf(output, "Not found\n");
                    }
                    break;
                case 'p':
                    print_hash_table(hash_table, output);
                    break;
                default:
                    break;
            }
        }
        fprintf(output, "\n");
        free_hash_table(hash_table);
    }

    fclose(input);
    fclose(output);
    return 0;
}

// Allocate a hash table that has given information.
HashTable* make_hash_table(int size, int (*hash_func)(HashTable*, int), int (*solution)(HashTable*, int, int))
{
    HashTable* result = malloc(sizeof(HashTable));
    result->size = size;
    result->hash_func = hash_func;
    result->solution = solution;
    result->table = malloc(sizeof(int) * size);
    
    int i;
    for(i = 0; i < size; ++i) {
        result->table[i] = 0;
    }

    return result;
}

// Deallocate the given hash table
void free_hash_table(HashTable* hash)
{
    if(hash == NULL) {
        return;
    }
    free(hash->table);
    free(hash);
}

// Find the given value in given hash table.
// if the value is found, return that index, else return -1
int find(HashTable* hash, int value)
{
    int i;
    for(i = 0; 
        hash->table[hash->solution(hash, value, i)] != 0 
        && hash->table[hash->solution(hash, value, i)] != value;
        ++i) {
            if(i > hash->size) {
                return -1;
            }
        }
    
    if(hash->table[hash->solution(hash, value, i)] == 0) {
        return -1;
    }
    return hash->solution(hash, value, i);
}

// print all values in given hash table
void print_hash_table(HashTable* hash, FILE* output)
{
    int i;
    for(i = 0; i < hash->size; ++i) {
        fprintf(output, "%d ", (hash->table[i] >= 0) ? hash->table[i] : 0);
    }
    fprintf(output, "\n");
}

// Insert the given value into given hash table.
// if insertion is successful return 0, else return 1
int insert(HashTable* hash, int value)
{
    int i;
    for(i = 0; 
        hash->table[hash->solution(hash, value, i)] > 0 
        && hash->table[hash->solution(hash, value, i)] != value;
        ++i);
    
    if(hash->table[hash->solution(hash, value, i)] <= 0) {
        hash->table[hash->solution(hash, value, i)] = value;
        return 0;
    }
    return 1;
}

// Delete the given value from given hash table.
// if deletion is successful return 0, else return 1
int delete(HashTable* hash, int value)
{
    int idx = find(hash, value);
    if(idx < 0) {
        return 1;
    }
    hash->table[idx] = -1;
    return 0;
}

// Hash function of simple modulo operation.
int mod_hash_func(HashTable* this, int value)
{
    return value % this->size;
}

// Solution function for collision in hash table.
// Linearly probe.
int linear_sol(HashTable* this, int value, int i)
{
    return (this->hash_func(this, value) + i) % this->size;
}

// Solution function for collision in hash table.
// Quadratically probe.
int quadratic_sol(HashTable* this, int value, int i)
{
    return (this->hash_func(this, value) + (i * i)) % this->size;
}

// Solution function for collision in hash table.
// Probe by double hash function.
int double_sol(HashTable* this, int value, int i)
{
    return (this->hash_func(this, value) + (i * (7 - (value % 7)))) % this->size;
}