#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define ORDER 3

typedef struct _B_node {
    int n_keys;
    struct _B_node *child[ORDER];
    int key[ORDER-1];
} B_node;

struct return_package {
    short is_overflow;
    int overflowed_key;
    B_node *overflowed_child; // 오른쪽으로 삐져나간 child를 담아서 올림.
};

B_node* make_empty_B_tree();
void free_B_tree(B_node*);
void free_B_node(B_node*);
struct return_package insert(B_node*,int);
B_node* insert_to_B_tree(B_node*,int);
int delete(B_node* root, int value);
B_node* delete_to_B_tree(B_node* root, int value);
void insert_key(int*,int,int,int);
int delete_key(int*,int,int);
int get_smallest_key(int*,int,int);
int get_largest_key(int*,int);
B_node* insert_subtree(B_node**,B_node*,int,int);
B_node* delete_subtree(B_node** arr, int index, int len);
B_node* push_front_subtree(B_node**,B_node*);
B_node* push_back_subtree(B_node**,B_node*);
int insert_rotation_test(B_node*,int);
void insert_rotate_with_left(B_node*,int,int,int,B_node*);
void insert_rotate_with_right(B_node*,int,int,int,B_node*);
int delete_rotation_test(B_node* root, int src);
void delete_rotate_with_right(B_node* root, int now_idx);
struct return_package node_split(B_node*,int,int,B_node*);
B_node* node_copy(B_node*,int,B_node*);
void merge(B_node* root, int left_idx);
int find_max(B_node*);
void inorder(B_node*,FILE*);
void swap(int*,int*);


int main()
{
    FILE *input = fopen("input.txt", "r");
    FILE *output = fopen("output.txt", "w");

    char mode[2];
    int tmp_num;
    B_node *root = make_empty_B_tree();
    while(fscanf(input, "%s", mode) == 1) {
        if(mode[0] == 'i') {
            fscanf(input, "%d", &tmp_num);
            root = insert_to_B_tree(root, tmp_num);
        } else if(mode[0] == 'p') {
            inorder(root, output);
            fprintf(output, "\n");
        } else if(mode[0] == 'd') {
            fscanf(input, "%d", &tmp_num);
            root = delete_to_B_tree(root, tmp_num);
        } else {
            break;
        }
    }
    free_B_tree(root); //TODO

    fclose(input);
    fclose(output);
    return 0;
}

// 빈 B_node를 동적할당해서 return해줌.
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

// len + 1이상의 크기를 가질 수 있는 arr를 가정. 즉, 꽉 차있지 않은 arr.
// arr의 index위치에 value를 삽입하고 그 이후 element는 한 칸씩 뒤로 옮겨줌.
void insert_key(int* arr, int index, int value, int len)
{
    int i;
    for(i = len; i > index; --i) {
        arr[i] = arr[i-1];
    }
    arr[index] = value;
}

// insert 전용 rotate에 대한 함수이다.
// root의 child중 src위치에서 difference만큼 왼쪽으로 떨어져 있는 child로 rotate를 진행한다.
// src와 dst 사이의 모든 sibling들은 key가 꽉 찬 상태임을 가정한다.
void insert_rotate_with_left(B_node* root, int src, int difference, int value, B_node* subtree)
{
    int now_idx;
    int key_temp;
    int dst = src - difference;
    B_node *temp_node_prt;

    // subtree is rightmost subtree
    for(now_idx = src; now_idx != dst; --now_idx) {
        value = get_smallest_key(root->child[now_idx]->key, value, ORDER-1);
        swap(&value, &root->key[now_idx - 1]);
        subtree = push_back_subtree(root->child[now_idx]->child, subtree);
    }

    B_node *dst_node = root->child[dst];
    dst_node->key[dst_node->n_keys++] = value;
    dst_node->child[dst_node->n_keys] = subtree;
}


// insert 전용 rotate에 대한 함수이다.
// root의 child중 src위치에서 difference만큼 오른쪽으로 떨어져 있는 child로 rotate를 진행한다.
// src와 dst 사이의 모든 sibling들은 key가 꽉 찬 상태임을 가정한다.
void insert_rotate_with_right(B_node* root, int src, int difference, int value, B_node* subtree)
{
    int now_idx;
    int key_temp;
    int dst = src + difference;
    B_node *temp_node_prt;

    for(now_idx = src; now_idx != dst; ++now_idx) {
        value = get_largest_key(root->child[now_idx]->key, value);
        swap(&value, &root->key[now_idx]);
        subtree = push_front_subtree(root->child[now_idx]->child, subtree);
    }

    insert_key(root->child[dst]->key, 0, value, root->child[dst]->n_keys++);
    push_front_subtree(root->child[dst]->child, subtree);
}

// argument로 받은 root에 key값을 insert함.
// return type은 struct return_package로, 이번 호출에서 overflow가 발생했는지, 했다면 어떤 key와 subtree가 overflow되었는지 return함.
struct return_package insert(B_node *root, int key)
{
    int i, j;
    int temp_num, rotatable_index;
    int rotatable;
    struct return_package result, returned_by_child;

    for(i = 0; i < root->n_keys && root->key[i] < key; ++i);
    if(i < root->n_keys && root->key[i] == key) {
        result.is_overflow = 0;
        return result;
    }

    // if root is leaf-node
    if(!root->child[0]) { 
        if(root->n_keys == ORDER-1) {
            //means that this leaf-node is full. So, make overflow.
            result.is_overflow = 1;
            result.overflowed_key = key;
            result.overflowed_child = NULL;
            return result;
        } else {
            insert_key(root->key, i, key, root->n_keys++);
            result.is_overflow = 0;
            return result;
        }
    }

    returned_by_child = insert(root->child[i], key);
    if(returned_by_child.is_overflow) {
        rotatable = insert_rotation_test(root, i); // 주석 해제시, rotation이 적용됨.
        // rotatable = 0;

        if(rotatable < 0) {
            insert_rotate_with_left(root, i, -rotatable, returned_by_child.overflowed_key, returned_by_child.overflowed_child);
            result.is_overflow = 0;
            return result;
        } else if(rotatable > 0) {
            insert_rotate_with_right(root, i, rotatable, returned_by_child.overflowed_key, returned_by_child.overflowed_child);
            result.is_overflow = 0;
            return result;
        }
        return node_split(root, i, returned_by_child.overflowed_key, returned_by_child.overflowed_child);

    } else { // overflow didn't occur.
        result.is_overflow = 0;
        return result;
    }
}

// inorder방식으로 B_tree의 key 값들을 fprint함.
void inorder(B_node* root, FILE* output)
{
    if(root == NULL) return;
    int i;
    for(i = 0; i < root->n_keys; ++i) {
        inorder(root->child[i], output);
        fprintf(output, "%d(%d) ", root->key[i], i);
    }
    inorder(root->child[root->n_keys], output);
}

// root의 child들 중 src위치의 child에서 다른 sibling에 insert용 rotation을 할 수 있는지 검사한 후, can_rotate값을 return
// if (can_rotate == 0) -> can't rotate
// if (can_rotate < 0) -> can rotate with left side
// if (can_rotate > 0) -> can rotate with right side
int insert_rotation_test(B_node* root, int src)
{
    int can_rotate = 0, dst;

    //left rotate test
    for(dst = src - 1; dst >= 0; --dst) {
        if(root->child[dst]->n_keys < ORDER-1) {
            can_rotate = dst - src;
            break;
        }
    }
    //right rotate test
    for(dst = src + 1; dst <= root->n_keys && (!can_rotate || dst - src < can_rotate); ++dst) {
        if(root->child[dst]->n_keys < ORDER-1) {
            can_rotate = dst - src;
            break;
        }
    }
    return can_rotate;
}

// keys 배열이 꽉 차있는 상태라고 가정하고, value를 sorting된 자리에 삽입한 후
// 가장 작은 key값을 return함. 나머지 key값들은 순서대로 배열에 남아있음.
int get_smallest_key(int* keys, int value, int len)
{
    int i, j, result = keys[0];

    if(keys[0] > value) return value;

    for(i = 0; i < len && keys[i] < value; ++i);
    for(j = 0; j < i - 1; ++j) {
        keys[j] = keys[j+1];
    }
    keys[i-1] = value;
    return result;
}

// keys 배열이 꽉 차있는 상태라고 가정하고, value를 sorting된 자리에 삽입한 후
// 가장 큰 key값을 return함. 나머지 key값들은 순서대로 배열에 남아있음.
int get_largest_key(int* keys, int value)
{
    int i, j, result = keys[ORDER-2];

    if(keys[ORDER-2] < value) return value;

    for(i = 0; keys[i] < value; ++i);
    for(j = ORDER-2; j > i; --j) {
        keys[j] = keys[j-1];
    }
    keys[i] = value;
    return result;
}

// children 배열이 꽉 차있다고 가정함.
// rightmost_value를 children의 오른쪽에 삽입하고 가장 왼쪽 값은 배열에서 빼낸 후 return.
B_node* push_back_subtree(B_node** children, B_node* rightmost_value)
{
    int i;
    B_node* result = children[0];
    for(i = 0; i < ORDER - 1; ++i) {
        children[i] = children[i+1];
    }
    children[ORDER-1] = rightmost_value;
    return result;
}

// children 배열이 꽉 차있다고 가정함.
// leftmost_value를 children의 왼쪽에 삽입하고 가장 오른쪽 값은 배열에서 빼낸 후 return.
B_node* push_front_subtree(B_node** children, B_node* leftmost_value)
{
    int i;
    B_node* result = children[ORDER-1];
    for(i = ORDER-1; i > 0; --i) {
        children[i] = children[i-1];
    }
    children[0] = leftmost_value;
    return result;
}

// lhs가 pointing하는 값과 rhs가 pointing하는 값을 서로 바꿈.
void swap(int *lhs, int *rhs)
{
    int temp = *lhs;
    *lhs = *rhs;
    *rhs = temp;
}

// root노드의 child들 중 dst위치의 subtree에서 node_split을 수행함.
// node_split 수행 후, root노드에서 overflow의 발생정보를 return.
struct return_package node_split(B_node* root, int dst, int overflowed_key, B_node* overflowed_child)
{
    struct return_package result;
    B_node* new_node = node_copy(root->child[dst], overflowed_key, overflowed_child);

    if(root->n_keys == ORDER - 1) {
        result.is_overflow = 1;
        result.overflowed_child = insert_subtree(root->child, new_node, dst + 1, ORDER);
        result.overflowed_key = root->child[dst]->key[--root->child[dst]->n_keys];
        return result;
    }

    insert_key(root->key, dst, root->child[dst]->key[--root->child[dst]->n_keys], root->n_keys++);
    insert_subtree(root->child, new_node, dst + 1, root->n_keys);
    result.is_overflow = 0;
    return result;
}

// src_node의 정보와 overflowed_key overflowed_child 값을 이용하여 node_split에 필요한 추가 노드를 동적할당 한 후 세팅함.
// result_node로 copy된 src_node의 정보는 src_node에서 제거됨.
B_node* node_copy(B_node* src, int overflowed_key, B_node* overflowed_child)
{
    B_node* result = make_empty_B_tree();
    int i;

    overflowed_key = get_largest_key(src->key, overflowed_key);
    for(i = 0; i < (ORDER-1)/2 - 1; ++i) {
        result->key[i] = src->key[i + 1 + (int)ceil((ORDER-1)/2.0)];
        result->child[i] = src->child[i + 1 + (int)ceil((ORDER-1)/2.0)];
    }
    result->key[(ORDER-1)/2 - 1] = overflowed_key;
    result->n_keys = (ORDER-1)/2;
    src->n_keys -= (ORDER-1)/2 - 1;

    result->child[(ORDER-1)/2 - 1] = src->child[ORDER - 1];
    result->child[(ORDER-1)/2] = overflowed_child;

    return result;
}

// len == ORDER면, 꽉 찬 children이라고 간주하고 넘친 값을 return.
// len != ORDER면, 꽉 차지 않았다고 간주함.
B_node* insert_subtree(B_node** children, B_node* subtree, int idx, int len)
{
    if(len == ORDER) {
        if(idx == len) return subtree;

        B_node* temp = children[len-1];
        int i;

        for(i = len-1; i > idx; --i) {
            children[i] = children[i-1];
        }
        children[idx] = subtree;
        return temp;
    }

    int i;

    for(i = len; i > idx; --i) {
        children[i] = children[i-1];
    }
    children[idx] = subtree;
    return NULL;
}

// 실제 가장 상위 node를 인자로 받아서 root node에서 필요한 추가 작업을 수행.
// insert 함수를 호출하여 구현함.
B_node* insert_to_B_tree(B_node* root, int key)
{
    struct return_package total_result = insert(root, key);

    if(!total_result.is_overflow) return root;
    B_node* new_root = make_empty_B_tree();
    B_node* new_right_subtree = node_copy(root, total_result.overflowed_key, total_result.overflowed_child);
    new_root->child[0] = root;
    new_root->child[1] = new_right_subtree;
    new_root->key[new_root->n_keys++] = root->key[--root->n_keys];

    return new_root;
}

// postorder 방식을 이용하여 recursive하게 tree를 free함.
void free_B_tree(B_node* root)
{
    if(root == NULL) return;
    int i;
    for(i = 0; i <= root->n_keys; ++i) {
        free_B_tree(root->child[i]);
    }
    // printf("%d\n", root->key[0]);
    free(root);
}

// find max key value
int find_max(B_node* root)
{
    while(root->child[0]) {
        root = root->child[root->n_keys];
    }
    return root->key[root->n_keys - 1];
}

// delete the key that is given index-th key in given arr.
// return deleted key value.
int delete_key(int* arr, int index, int len)
{
    int i;
    int result = arr[index];
    for(i = index; i < len - 1; ++i) {
        arr[i] = arr[i+1];
    }
    return result;
}

// delete the subtree that is given index-th subtree in given arr.
// return deleted subtree value.
B_node* delete_subtree(B_node** arr, int index, int len)
{
    int i;
    B_node* result = arr[index];
    for(i = index; i < len - 1; ++i) {
        arr[i] = arr[i+1];
    }
    return result;
}

// test rotatable from root's src child to left or right sibling node
/* return rotatable test result that
    -1 : can rotate with a left sibling.
    0 : can't rotate with any siblings.
    +1 : can rotate with a right sibling.
*/
int delete_rotation_test(B_node* root, int src)
{
    int left = src - 1, right = src + 1;
    if(left >= 0 && root->child[left]->n_keys > ceil(ORDER / 2.0) - 1) {
        return -1;
    } else if(right <= root->n_keys && root->child[right]->n_keys > ceil(ORDER / 2.0) - 1) {
        return 1;
    } else {
        return 0;
    }
}

// Function for rotate of delete.
// Perform rotation with the closest left sibling.
void delete_rotate_with_left(B_node* root, int now_idx)
{
    B_node* dst_node = root->child[now_idx - 1];
    int key = dst_node->key[dst_node->n_keys - 1];
    B_node* subtree = dst_node->child[dst_node->n_keys];
    dst_node->n_keys -= 1;

    swap(&key, &root->key[now_idx - 1]);
    insert_key(root->child[now_idx]->key, 0, key, root->child[now_idx]->n_keys);
    insert_subtree(root->child[now_idx]->child, subtree, 0, ++root->child[now_idx]->n_keys);
}

// Function for rotate of delete.
// Perform rotation with the closest right sibling.
void delete_rotate_with_right(B_node* root, int now_idx)
{
    B_node* now_node = root->child[now_idx];
    int key = delete_key(root->child[now_idx + 1]->key, 0, root->child[now_idx + 1]->n_keys);
    B_node* subtree = delete_subtree(root->child[now_idx + 1]->child, 0, root->child[now_idx + 1]->n_keys + 1);
    root->child[now_idx + 1]->n_keys -= 1;

    swap(&key, &root->key[now_idx]);
    now_node->key[now_node->n_keys] = key;
    now_node->child[++now_node->n_keys] = subtree;
}

// Merge root's left_idx-th child with the its closest right sibling.
void merge(B_node* root, int left_idx)
{
    int center_key = delete_key(root->key, left_idx, root->n_keys);
    B_node* left_node = root->child[left_idx];
    B_node* right_node = root->child[left_idx + 1];
    int left_n = left_node->n_keys;
    int i;
    for(i = right_node->n_keys; i > 0; --i) {
        left_node->key[left_n + i] = right_node->key[i - 1];
        left_node->child[left_n + i + 1] = right_node->child[i];
    }
    left_node->key[left_n] = center_key;
    left_node->child[left_n + 1] = right_node->child[0];

    left_node->n_keys += right_node->n_keys + 1;
    free(delete_subtree(root->child, left_idx + 1, root->n_keys + 1));
    root->n_keys -= 1;
}

// This function is internal function that is invoked by delete_to_B_node function.
/* return a state value that
    0 : underflow does not occur.
    1 : underflow occurs.
*/
int delete(B_node* root, int value)
{
    int i;
    for(i = 0; i < root->n_keys && root->key[i] < value; ++i);

    // if target key is found
    if(i < root->n_keys && root->key[i] == value) {
        //if root is leaf node
        if(root->child[0] == NULL) {
            delete_key(root->key, i, root->n_keys--);
            return root->n_keys < ceil(ORDER / 2.0) - 1; // return whether underflow occurs or not.
        }
        // root is non-leaf node.
        root->key[i] = find_max(root->child[i]);
        value = root->key[i];
    }

    int is_underflow = delete(root->child[i], value);
    if(is_underflow) {
        int rotatable = delete_rotation_test(root, i);
        if(rotatable < 0) {
            delete_rotate_with_left(root, i);
            return 0;
        } else if(rotatable > 0) {
            delete_rotate_with_right(root, i);
            return 0;
        } else {
            merge(root, (i > 0) ? (i - 1) : i);
            return root->n_keys < ceil(ORDER / 2.0) - 1;
        }
    } else {
        return 0;
    }
}

// find the given key in given B-tree and delete it.
// This function is external function that perform delete key of B-tree.
B_node* delete_to_B_tree(B_node* root, int value)
{
    int is_underflow = delete(root, value);
    if(is_underflow && root->child[0]) {
        B_node* result = root->child[0];
        free(root);
        return result;
    } else {
        return root;
    }
}
