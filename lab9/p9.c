#include <stdio.h>
#include <stdlib.h>

#define ORDER 3

typedef struct _B_node {
    int n_keys;
    struct _B_node *child[ORDER];
    int key[ORDER-1];
} B_node;

struct return_package {
    B_node *root;
    short is_overflow;
    int overflowed_key;
    B_node *overflowed_child; // 오른쪽으로 삐져나간 child를 담아서 올림.
};

B_node* make_empty_B_tree();
void insert_key(int*,int,int,int);
struct return_package insert(B_node*, int);
void rotate(B_node*,int,int,int);
void inorder(B_node*,FILE*);

int main()
{
    FILE *input = stdin; //fopen("input.txt", "r");
    FILE *output = stdout;

    char mode[2];
    int tmp_num;
    B_node *root = make_empty_B_tree();

    while(fscanf(input, "%s", mode) == 1) {
        if(mode[0] == 'i') {
            fscanf(input, "%d", &tmp_num);
            root = insert(root, tmp_num).root;
    //    inorder(root, output); fprintf(output, "\n");
        } else if(mode[0] == 'p') {
            //TODO
        }
    }

    // fclose(input);
    // fclose(output);
    return 0;
}

//
B_node* make_empty_B_tree()
{
    int i;
    B_node* temp = malloc(sizeof(B_node));
    temp->n_keys = 0;
    for(i = 0; i < ORDER; ++i) {
        temp->child[i] = NULL;
    }
    return temp;
}

//
void insert_key(int* arr, int index, int value, int len)
{
    int i;
    int temp;
    for(i = index; i < len; ++i) {
        temp = arr[i];
        arr[i] = value;
        value = temp;
    }
}

//
void rotate_with_left(B_node* root, int src, int difference, int value, B_node* subtree)
{
    int now_idx, i;
    int key_temp;
    int dst = src - difference;
    B_node *temp_node_prt;


    // to place largest key of src_node_keys at "value", insert argument 'value' in src_node_keys and sort
    int *src_node_keys = root->child[src]->key;
    if(src_node_keys[ORDER-2] > value) {
        for(i = 0; src_node_keys[i] < value; ++i);
        for(; i < ORDER-1; ++i) {
            key_temp = src_node_keys[i];
            src_node_keys[i] = value;
            value = key_temp;
        }
    }

    for(now_idx = src; now_idx != dst; --now_idx) {
        temp_node_prt = root->child[now_idx]->child[ORDER-1];
        root->child[now_idx]->child[ORDER-1] = subtree;
        subtree = temp_node_prt;
        for(i = ORDER-2; i >= 0; --i) {
            key_temp = root->child[now_idx]->key[i];
            root->child[now_idx]->key[i] = value;
            value = key_temp;

            temp_node_prt = root->child[now_idx]->child[i];
            root->child[now_idx]->child[i] = subtree;
            subtree = temp_node_prt;
        }
        key_temp = root->key[now_idx-1];
        root->key[now_idx-1] = value;
        value = key_temp;
    }

    B_node *dst_node = root->child[dst];
    dst_node->key[dst_node->n_keys++] = value;
    dst_node->child[dst_node->n_keys] = subtree;
}

//
void rotate_with_right(B_node* root, int src, int difference, int value, B_node* subtree)
{
    int now_idx, i;
    int key_temp;
    int dst = src + difference;
    B_node *temp_node_prt;


    // to place smallest key of src_node_keys at "value", insert argument 'value' in src_node_keys and sort
    int *src_node_keys = root->child[src]->key;
    if(src_node_keys[0] < value) {
        for(i = ORDER-2; src_node_keys[i] > value; --i);
        for(; i >= 0; --i) {
            key_temp = src_node_keys[i];
            src_node_keys[i] = value;
            value = key_temp;
        }
    }

    { // now_idx = src
        now_idx = src;
        for(i = 0; i < ORDER-1; ++i) {
            key_temp = root->child[now_idx]->key[i];
            root->child[now_idx]->key[i] = value;
            value = key_temp;
        }
        key_temp = root->key[now_idx-1];
        root->key[now_idx-1] = value;
        value = key_temp;
    }
    for(now_idx = src + 1; now_idx != dst; ++now_idx) {
        for(i = 0; i < ORDER-1; ++i) {
            key_temp = root->child[now_idx]->key[i];
            root->child[now_idx]->key[i] = value;
            value = key_temp;

            temp_node_prt = root->child[now_idx]->child[i];
            root->child[now_idx]->child[i] = subtree;
            subtree = temp_node_prt;
        }
        temp_node_prt = root->child[now_idx]->child[ORDER-1];
        root->child[now_idx]->child[ORDER-1] = subtree;
        subtree = temp_node_prt;

        key_temp = root->key[now_idx-1];
        root->key[now_idx-1] = value;
        value = key_temp;
    }

    B_node *dst_node = root->child[dst];
    for(i = 0; i < dst_node->n_keys; ++i) {
        key_temp = dst_node->key[i];
        dst_node->key[i] = value;
        value = key_temp;
    }
    dst_node->key[dst_node->n_keys++] = value;

    for(i = 0; i < dst_node->n_keys; ++i) {
        temp_node_prt = dst_node->child[i];
        dst_node->child[i] = subtree;
        subtree = temp_node_prt;
    }
    dst_node->child[dst_node->n_keys] = subtree;
}

//
struct return_package insert(B_node *root, int key)
{
    int i, j;
    int temp_num, rotatable_index;
    int can_rotate = 0;
    struct return_package result, returned_by_child;

    for(i = 0; i < root->n_keys && root->key[i] < key; ++i);
    if(root->key[i] == key) {
        result.root = root;
        result.is_overflow = 0;
        return result;
    }

    //means "root" node is NOT leaf-node now.
    if(root->child[i]) {
        returned_by_child = insert(root->child[i], key);
        root->child[i] = returned_by_child.root;
        if(returned_by_child.is_overflow) {
            //TODO: 오버플로우 발생시 위쪽.

            //left rotate test
            for(j = i - 1; j >= 0; --j) {
                if(root->child[j]->n_keys < ORDER-1) {
                    can_rotate = i - j;
                    break;
                }
            }
            //right rotate test
            for(j = i + 1; j <= root->n_keys && (!can_rotate || j - i < can_rotate); ++j) {
                if(root->child[j]->n_keys < ORDER-1) {
                    can_rotate = i - j;
                    break;
                }
            }

            // if (can_rotate == 0) -> can't rotate
            // if (can_rotate > 0) -> can rotate with left side
            // if (can_rotate < 0) -> can rotate with right side

            if(can_rotate > 0) {
                rotate_with_left(root, i, can_rotate, returned_by_child.overflowed_key, returned_by_child.overflowed_child);
            } else if(can_rotate < 0) {
                rotate_with_right(root, i, -can_rotate, returned_by_child.overflowed_key, returned_by_child.overflowed_child);
            } else {
                //TODO: do node split
            }


    //이거 맞나?
            {
                result.root = root;
                result.is_overflow = 0;
                return result;
            }
        } else { // overflow didn't occur.
            result.root = root;
            result.is_overflow = 0;
            return result;
        }
    } else { //means "root" node is leaf-node.
        if(root->n_keys == ORDER-1) {
            //means that this leaf-node is full. So, make overflow.
            result.is_overflow = 1;
            result.overflowed_key = key;
            result.root = root;
            result.overflowed_child = NULL;
            return result;
        } else {
            insert_key(root->key, i, key, ORDER-1);
            ++root->n_keys;
            result.is_overflow = 0;
            result.root = root;
            result.overflowed_child = NULL;
            return result;
        }
    }
}

//
void inorder(B_node* root, FILE* output)
{
    if(root == NULL) return;
    int i;
    for(i = 0; i < root->n_keys; ++i) {
        inorder(root->child[i], output);
        fprintf(output, "%d ", root->key[i]);
    }
    inorder(root->child[root->n_keys], output);
}