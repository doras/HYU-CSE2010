#include <stdio.h>
#include <stdlib.h>

#define DEBUG printf("%d\n",__LINE__)

typedef struct _TreeNode {
    int value;
    struct _TreeNode *left, *right;
} TreeNode;

// value로 받은 값을 가지는 node를 생성해서 인자로 받은 tree의 알맞은 자리에 insert, 만약 이미 있는 값이라면 메시지를 출력한다.
TreeNode* InsertNode(int value, TreeNode* root, FILE* output) {
    if (root == NULL) {
        TreeNode* temp = (TreeNode*)malloc(sizeof(TreeNode));
        temp->value = value;
        temp->left = NULL;
        temp->right = NULL;
        return temp;
    }
    if (root->value > value) {
        root->left = InsertNode(value, root->left, output);
    } else if (root->value < value) {
        root->right = InsertNode(value, root->right, output);
    } else {
        fprintf(output, "%d already exists.\n", value);
    }
    return root;
}

/*
void insert_node_without_recursive(int value, TreeNode** root, FILE* output) {
    TreeNode *temp, *prev, *now;
    short is_direction_right; //left -> 0, right -> 1
    if (*root == NULL) {
        temp = (TreeNode*)malloc(sizeof(TreeNode));
        temp->value = value;
        temp->left = NULL;
        temp->right = NULL;
        *root = temp;
    }
    now = *root;
    while(now && now->value != value) {
        prev = now;
        if (now->value > value) {
            now = now->left;
            is_direction_right = 0;
        } else {
            now = now->right;
            is_direction_right = 1;
        }
    }
    if (now == NULL) {
        temp = (TreeNode*)malloc(sizeof(TreeNode));
        temp->value = value;
        temp->left = NULL;
        temp->right = NULL;
        if (is_direction_right) {
            prev->right = temp;
        } else {
            prev->left = temp;
        }
    } else {
        fprintf(output, "%d already exists.\n", value);
    }
}
*/

// value로 받은 값을 가지는 node를 인자로 받은 tree에서 찾아서, 있다면 free, 없다면 메시지를 출력한다.
TreeNode* DeleteNode(int value, TreeNode* root, FILE* output) {
    TreeNode* temp;
    if (root == NULL) {
        fprintf(output, "Deletion failed. %d does not exist.\n", value);
        return root;
    }
    if (root->value > value) {
        root->left = DeleteNode(value, root->left, output);
    } else if (root->value < value) {
        root->right = DeleteNode(value, root->right, output);
    } else {
        if (root->left && root->right) {
            temp = root->left;
            while(temp->right) temp = temp->right;
            root->value = temp->value;
            root->left = DeleteNode(temp->value, root->left, output);
        } else {
            if (root->left == NULL) {
                temp = root->right;
                free(root);
                return temp;
            } else {
                temp = root->left;
                free(root);
                return temp;
            }
        }
    }
    return root;
}

/*
void delete_node_without_recursive(int value, TreeNode* root, FILE* output) {
    TreeNode *prev, *now, *temp;
    short is_direction_right = 0;
    if (root == NULL) {
        fprintf(output, "Deletion fail\n");
        return;
    }
    now = root;
    while (1) {
        while (now && now->value != value) {
            prev = now;
            if (now->value > value) {
                now = now->left;
                is_direction_right = 0;
            } else {
                now = now->right;
                is_direction_right = 1;
            }
        }
        if (now == NULL) {
            fprintf(output, "Deletion fail\n");
            return;
        }
        if (now->left == NULL) {
            if (is_direction_right) {
                prev->right = now->right;
                free(now);
                return;
            }
            prev->left = now->right;
            free(now);
            return;
        } else if (now->right == NULL) {
            if (is_direction_right) {
                prev->right = now->left;
                free(now);
                return;
            }
            prev->left = now->left;
            free(now);
            return;
        }
        temp = now->left;
        while(temp->right) temp = temp->right;
        now->value = temp->value;
        value = temp->value;
        prev = now;
        now = now->left;
        is_direction_right = 0;
    }
}
*/

// value로 받은 값을 지니는 node를 인자로 받은 tree에서 찾는다. 있다면 해당 node의 주소값을, 없다면 NULL을 return.
TreeNode* FindNode(int value, TreeNode* root) {
    while(root && value != root->value){
        if (root->value > value) {
            root = root->left;
        } else {
            root = root->right;
        }
    }
    if (root == NULL) {
        return NULL;
    }
    return root;
}

//인자로 받은 tree를 inorder로 출력한다.
void PrintInorder(TreeNode* root, FILE* output) {
    if (root == NULL) return;
    PrintInorder(root->left, output);
    fprintf(output, " %d", root->value);
    PrintInorder(root->right, output);
}

// 인자로 받은 tree를 preorder로 출력한다.
void PrintPreorder(TreeNode* root, FILE* output) {
    if (root == NULL) return;
    fprintf(output, " %d", root->value);
    PrintPreorder(root->left, output);
    PrintPreorder(root->right, output);
}

// 인자로 받은 tree를 postorder로 출력한다.
void PrintPostorder(TreeNode* root, FILE* output) {
    if (root == NULL) return;
    PrintPostorder(root->left, output);
    PrintPostorder(root->right, output);
    fprintf(output, " %d", root->value);
}

// 인자로 받은 tree의 모든 node를 free한다.
void DeleteTree(TreeNode* root) {
    if (root == NULL) return;
    DeleteTree(root->left);
    DeleteTree(root->right);
    free(root);
}

int main() {
    FILE *input = fopen("input.txt", "r");
    FILE *output = fopen("output.txt", "w");
    char mode[3];
    int temp_num;
    TreeNode* root = NULL;
    while(fscanf(input, "%s", mode) == 1){
        switch (mode[0])
        {
            case 'i':
                fscanf(input, "%d", &temp_num);
                root = InsertNode(temp_num, root, output);
                break;
            case 'd':
                fscanf(input, "%d", &temp_num);
                root = DeleteNode(temp_num, root, output);
                break;
            case 'f':
                fscanf(input, "%d", &temp_num);
                if (FindNode(temp_num, root)) {
                    fprintf(output, "%d is in the tree.\n", temp_num);
                } else {
                    fprintf(output, "%d is not in the tree.\n", temp_num);
                }
                break;
            case 'p':
                if (mode[1] == 'i') {
                    fprintf(output, "pi -");
                    PrintInorder(root, output);
                    fprintf(output, "\n");
                } else if (mode[1] == 'r') {
                    fprintf(output, "pr -");
                    PrintPreorder(root, output);
                    fprintf(output, "\n");
                } else if (mode[1] == 'o') {
                    fprintf(output, "po -");
                    PrintPostorder(root, output);
                    fprintf(output, "\n");
                }
                break;
            default:
                break;
        }
    }
    DeleteTree(root);
    fclose(input);
    fclose(output);
    return 0;
}