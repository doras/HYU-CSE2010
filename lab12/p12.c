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

    FILE* input = fopen("/Users/doras/Desktop/shoon/HYU-assignment/2019_CSE2010_2018008659/lab12/input.txt", "r");
    FILE* output = fopen("/Users/doras/Desktop/shoon/HYU-assignment/2019_CSE2010_2018008659/lab12/output.txt", "w");

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
    }

    fclose(input);
    fclose(output);
    return 0;
}

//
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

//
void free_hash_table(HashTable* hash)
{
    if(hash == NULL) {
        return;
    }
    free(hash->table);
    free(hash);
}

//
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

//
void print_hash_table(HashTable* hash, FILE* output)
{
    int i;
    for(i = 0; i < hash->size; ++i) {
        fprintf(output, "%d ", (hash->table[i] >= 0) ? hash->table[i] : 0);
    }
    fprintf(output, "\n");
}

//
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

//
int delete(HashTable* hash, int value)
{
    int idx = find(hash, value);
    if(idx < 0) {
        return 1;
    }
    hash->table[idx] = -1;
    return 0;
}

//
int mod_hash_func(HashTable* this, int value)
{
    return value % this->size;
}

//
int linear_sol(HashTable* this, int value, int i)
{
    return (this->hash_func(this, value) + i) % this->size;
}

//
int quadratic_sol(HashTable* this, int value, int i)
{
    return (this->hash_func(this, value) + (i * i)) % this->size;
}

//
int double_sol(HashTable* this, int value, int i)
{
    return (this->hash_func(this, value) + (i * (7 - (value % 7)))) % this->size;
}