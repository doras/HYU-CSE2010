#include <stdio.h>
#include <stdlib.h>

#define DEBUG(x) printf("line:%d, code:%d\n", __LINE__, x);

typedef struct _AVLNode {
    int elem;
    struct _AVLNode *left;
    struct _AVLNode *right;
    int height;
} AVLNode;

//a와 b 중에 더 큰 값을 return
int max(int a, int b)
{
    return (a>b)?a:b;
}

short successful_insertion = 1;

// argument로 받은 node의 height값을 return
int height(AVLNode* pos)
{
    if(pos == NULL) return -1;
    return pos->height;
}

// argument로 받은 node를 기준으로 left single rotation을 수행.
// rotate의 대상이 되는 두 개의 노드는 NULL이 아니라고 가정.
AVLNode* single_rotate_with_left(AVLNode* k2)
{
    AVLNode* k1 = k2->left;
    k2->left = k1->right;
    k1->right = k2;

    k2->height = max(height(k2->left), height(k2->right)) + 1;
    k1->height = max(height(k1->left), height(k2)) + 1;
    return k1;
}

// argument로 받은 node를 기준으로 right single rotation을 수행.
// rotate의 대상이 되는 두 개의 노드는 NULL이 아니라고 가정.
AVLNode* single_rotate_with_right(AVLNode* k1)
{
    AVLNode* k2 = k1->right;
    k1->right = k2->left;
    k2->left = k1;

    k1->height = max(height(k1->left), height(k1->right)) + 1;
    k2->height = max(height(k2->right), height(k1)) + 1;
    return k2;
}

// argument로 받은 node를 기준으로 left double rotation을 수행.
// rotate의 대상이 되는 세 개의 노드는 NULL이 아니라고 가정.
AVLNode* double_rotate_with_left(AVLNode* k3)
{
    k3->left = single_rotate_with_right(k3->left);

    return single_rotate_with_left(k3);
}

// argument로 받은 node를 기준으로 right double rotation을 수행.
// rotate의 대상이 되는 세 개의 노드는 NULL이 아니라고 가정.
AVLNode* double_rotate_with_right(AVLNode* k1)
{
    k1->right = single_rotate_with_left(k1->right);

    return single_rotate_with_right(k1);
}

// argument로 받은 value를 가지는 node를 recursive한 과정으로 insert함.
// 만약 이미 있는 값이면 message를 출력하고 insert는 진행되지 않음.
AVLNode* insert(int value, AVLNode* root, FILE* output)
{
    successful_insertion = 1;
    if(root == NULL) {
        root = (AVLNode*)malloc(sizeof(AVLNode));
        root->left = NULL;
        root->right = NULL;
        root->elem = value;
        root->height = 0;
    } else if(value > root->elem) {
        root->right = insert(value, root->right, output);
        if(height(root->right) - height(root->left) == 2) {
            if(value > root->right->elem) {
                root = single_rotate_with_right(root);
            } else {
                root = double_rotate_with_right(root);
            }
        }
    } else if(value < root->elem) {
        root->left = insert(value, root->left, output);
        if(height(root->left) - height(root->right) == 2) {
            if(value < root->left->elem) {
                root = single_rotate_with_left(root);
            } else {
                root = double_rotate_with_left(root);
            }
        }
    } else {
        fprintf(output, "%d already in the tree!\n", value);
        successful_insertion = 0;
    }
    root->height = max(height(root->right), height(root->left)) + 1;
    return root;
}

// lab5에서 사용했던 inorder방식의 print traversal.
void PrintInorder(AVLNode* root, FILE* output) {
    if (root == NULL) return;
    PrintInorder(root->left, output);
    fprintf(output, "%d(%d) ", root->elem, root->height);
    PrintInorder(root->right, output);
}

// 인자로 받은 tree의 모든 node를 free
void DeleteTree(AVLNode* root) {
    if (root == NULL) return;
    DeleteTree(root->left);
    DeleteTree(root->right);
    free(root);
}

AVLNode* delete(int value, AVLNode* T)
{
    AVLNode* temp;
    if(T == NULL) {
        printf("%d doesn't exist.\n", value);
    } else if(value > T->elem) {
        T->right = delete(value, T->right);
        if(height(T->left) - height(T->right) == 2) {
            if(height(T->left->left) >= height(T->left->right))
                T = single_rotate_with_left(T);
            else
                T = double_rotate_with_left(T);
        }
    } else if(value < T->elem) {
        T->left = delete(value, T->left);
        if(height(T->right) - height(T->left) == 2) {
            if(height(T->right->right) >= height(T->right->left))
                T = single_rotate_with_right(T);
            else 
                T = double_rotate_with_right(T);
        }
    } else {
        if(T->left && T->right) {
            temp = T->left;
            while(temp->right) temp = temp->right;
            T->elem = temp->elem;
            T->left = delete(temp->elem, T->left);
            if(height(T->right) - height(T->left) == 2) {
                if(height(T->right->right) > height(T->right->left))
                    T = single_rotate_with_right(T);
                else 
                    T = double_rotate_with_right(T);
            }
        } else {
            if(T->right == NULL) {
                temp = T;
                T = temp->left;
                free(temp);
            } else {
                temp = T;
                T = temp->right;
                free(temp);
            }
        }
    }
    if(T != NULL)
        T->height = max(height(T->left), height(T->right)) + 1;
    return T;
}

int main()
{
    FILE *input = fopen("input.txt", "r");
    FILE *output = fopen("output.txt", "w");
    int temp_value;
    AVLNode* root = NULL;
    while(fscanf(input, "%d", &temp_value) == 1) {
        root = insert(temp_value, root, output);
        if(successful_insertion)
        {
            PrintInorder(root, output);
            fprintf(output, "\n");
        }
    }

/*
    while(scanf("%d", &temp_value) == 1) {
        root = delete(temp_value, root);
        PrintInorder(root, output);
        printf("\n");
    }
*/

    DeleteTree(root);
    fclose(input);
    fclose(output);
    return 0;
}
