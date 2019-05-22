#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct _Skip_node {
    int elem;
    int level;
    struct _Skip_node** forward;
} Skip_node;

int coin_flip();
Skip_node* make_empty_skipped_list(int);
Skip_node* make_empty_skip_node(int,int,Skip_node**);
void delete_skipped_list(Skip_node*);
Skip_node** search(Skip_node*,int);
void insert(Skip_node*,int);
void delete(Skip_node*,int);
Skip_node* find(Skip_node*,int);
int choose_level(int);
void print_skipped_list(FILE*,Skip_node*);

int main()
{
    Skip_node* header;
    int temp_num;
    Skip_node* node_temp;
    char mode[2];
    int i;

    FILE* input = fopen("input.txt", "r");
    FILE* output = fopen("output.txt", "w");
    
    srand(time(NULL));

    fscanf(input, "%d", &temp_num);
    header = make_empty_skipped_list(temp_num);

    while(fscanf(input, "%s", mode) == 1) {
        switch(mode[0]) {
            case 'i':
            {
                fscanf(input, "%d", &temp_num);
                insert(header, temp_num);
                break;
            }
            case 'd':
            {
                fscanf(input, "%d", &temp_num);
                delete(header, temp_num);
                break;
            }
            case 'f':
            {
                fscanf(input, "%d", &temp_num);
                node_temp = find(header, temp_num);
                if(!node_temp) {
                    fprintf(output ,"There isn't %d.\n", temp_num);
                    break;
                }
                fprintf(output, "elem : %d, address : %p\n", node_temp->elem, node_temp);
                for(i = 0; i <= node_temp->level; ++i) {
                    fprintf(output, "next[%d] : %d\n", i, node_temp->forward[i]->elem);
                }
                break;
            }
            case 'p':
            {
                print_skipped_list(output, header);
                break;
            }
        }
    }
    delete_skipped_list(header);
    fclose(input);
    fclose(output);
    return 0;
}

// return 0 or 1, respectively, with 0.5 probability
int coin_flip()
{
    return rand() % 2;
}

// dynamically allocate header node and tail node, return header node's address.
// max_level is 0-based value.
Skip_node* make_empty_skipped_list(int max_level)
{
    Skip_node* head = malloc(sizeof(Skip_node));
    Skip_node* tail = malloc(sizeof(Skip_node));

    head->level = max_level;
    head->forward = malloc(sizeof(Skip_node*) * (max_level + 1));

    tail->level = max_level;
    tail->forward = NULL;

    int i;
    for(i = 0; i <= max_level; ++i) {
        head->forward[i] = tail;
    }
    
    return head;
}

// deallocate the skipped list with given skipped list's header
void delete_skipped_list(Skip_node* node)
{
    Skip_node* temp;

    // while node is not tail
    while(node->forward) {
        temp = node;
        node = node->forward[0];
        free(temp->forward);
        free(temp);
    }
    free(node);
}

// search the node that contains given value in given skipped list
// return the positions of the items that has the largest key less than given value in each level
// this function will return dynamically allocated array, therefore it needs deallocation.
Skip_node** search(Skip_node* header, int value)
{
    Skip_node** result = malloc(sizeof(Skip_node*) * (header->level + 1));
    int i;
    for(i = header->level; i >= 0; --i) {
        for(; header->forward[i]->forward && header->forward[i]->elem < value; header = header->forward[i]);
        result[i] = header;
    }
    return result;
}

// insert a new node that contains given value to given Skipped_list
void insert(Skip_node* header, int value)
{
    Skip_node** search_result = search(header, value);
    int chosen_level = choose_level(header->level);
    Skip_node* new_node = make_empty_skip_node(value, chosen_level, search_result);

    int i;
    for(i = 0; i <= chosen_level; ++i) {
        new_node->forward[i] = search_result[i]->forward[i];
        search_result[i]->forward[i] = new_node;
    }

    free(search_result);
}

// By calling coin_flip func, return an randomly generated integer that is from 0 to given max_level including both end.
int choose_level(int max_level)
{
    int count = 0;
    while(coin_flip()) ++count;

    if(count > max_level) return max_level;
    return count;
}

// dynamically allocate a Skip_node that has given value and level.
// forward of each node of search_result will replace to a new node.
Skip_node* make_empty_skip_node(int value, int level, Skip_node** search_result)
{
    Skip_node* new_node = malloc(sizeof(Skip_node));
    new_node->elem = value;
    new_node->level = level;
    new_node->forward = malloc(sizeof(Skip_node*) * (level + 1));

    return new_node;
}

// print given skipped list's elements and levels
void print_skipped_list(FILE* output, Skip_node* header)
{
    fprintf(output, "header(%d)\n", header->level);
    header = header->forward[0];
    for(; header->forward; header = header->forward[0]) {
        fprintf(output, "%d(%d)\n", header->elem, header->level);
    }
    fprintf(output, "tail(%d)\n", header->level);
}

// delete node that has given value as element from given skipped list.
void delete(Skip_node* header, int value)
{
    Skip_node** search_result = search(header, value);
    Skip_node* target = search_result[0]->forward[0];

    if(target->elem != value) {
        free(search_result);
        return;
    }

    int i;
    for(i = 0; i <= target->level; ++i) {
        search_result[i]->forward[i] = target->forward[i];
    }

    free(target->forward);
    free(target);
    free(search_result);
}

// find the node that has given value as its element. And return its address.
Skip_node* find(Skip_node* header, int value)
{
    int i;
    Skip_node* temp = header;
    for(i = header->level; i >= 0; --i) {
        for(; temp->forward[i]->forward && temp->forward[i]->elem <= value; temp = temp->forward[i]);
        if(temp != header && temp->elem == value) return temp;
    }
    return NULL;
}
