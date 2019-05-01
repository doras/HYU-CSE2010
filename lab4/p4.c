#include <stdio.h>
#include <stdlib.h>

typedef struct _TreeNode{
    short int left_thread;
    short int right_thread;
    char data;
    struct _TreeNode *left_child;
    struct _TreeNode *right_child;
} TreeNode;

typedef struct _QueueNode{
    TreeNode **data;
    struct _QueueNode *next;
} QueueNode;
typedef struct{
    QueueNode *head;
    QueueNode *tail;
} Queue;

void push_stack(TreeNode* value, TreeNode** stack, int *top);
TreeNode* pop_stack(TreeNode** stack, int *top);
void iter_inorder(TreeNode* root);
void iter_levelorder(TreeNode* root);
void iter_preorder(TreeNode*);

//빈 queue를 만들어서 return
Queue* makeQ(){
    Queue *temp = (Queue*)malloc(sizeof(Queue));
    temp->head = NULL;
    temp->tail = NULL;
    return temp;
}
//Queue에 들어갈 data인 TreeNode**을 받아 enQ함.
void enQ(Queue *queue, TreeNode **data) {
    QueueNode *temp = (QueueNode*)malloc(sizeof(QueueNode));
    temp->data = data;
    temp->next = NULL;
    if (!queue->tail) {
        queue->head = temp;
        queue->tail = temp;
    }
    else {
        queue->tail->next = temp;
        queue->tail = temp;
    }
}

//Queue에서 deQ함.
TreeNode** deQ(Queue *queue) {
    QueueNode *temp;
    TreeNode **result;
    if (!queue->head)
        return NULL;

    if (queue->head == queue->tail) {
        temp = queue->head;
        queue->head = NULL;
        queue->tail = NULL;
    }
    else {
        temp = queue->head;
        queue->head = queue->head->next;
    }
    result = temp->data;
    free(temp);
    return result;
}

//인자로 받은 Queue의 노드를 전부 free하고 Queue를 삭제함
void deleteQ(Queue *queue){
    while(queue->head) deQ(queue);
    free(queue);
}

typedef struct _StackNode{
    TreeNode *data;
    struct _StackNode *next;
} StackNode;

//빈 헤드용 Stack을 만들어서 return
StackNode* makeStack(){
    StackNode *temp = (StackNode*)malloc(sizeof(StackNode));
    temp->data = NULL;
    temp->next = NULL;
    return temp;
}
//Stack에 들어갈 data인 TreeNode*을 받아 push함.
void push(StackNode *stack, TreeNode *data) {
    StackNode *temp = (StackNode*)malloc(sizeof(StackNode));
    temp->data = data;
    temp->next = stack->next;
    stack->next = temp;
}

//Stack에서 pop함.
TreeNode* pop(StackNode *stack) {
    StackNode *temp;
    TreeNode *result;
    if (!stack->next)
        return NULL;

    temp = stack->next;
    stack->next = stack->next->next;
    result = temp->data;
    free(temp);
    return result;
}

//인자로 받은 Stack의 노드를 전부 free하고 Stack을 삭제함
void deleteStack(StackNode *stack){
    while(stack->next){
        pop(stack);
    }
    free(stack);
}

//만들어야 하는 트리가 completely full binary tree라고 가정할 때, 그 data의 배열을 받아서 not-threaded tree를 만들어주는 함수
TreeNode* makeTree(const char *arr, int len)
{
    int count = 0;
    Queue *queue = makeQ();
    TreeNode **temp_pptr;
    TreeNode *root = (TreeNode*)malloc(sizeof(TreeNode));
    root->right_child = root;
    root->right_thread = 0;
    enQ(queue, &root->left_child);
    while(count < len){
        temp_pptr = deQ(queue);
        *temp_pptr = (TreeNode*)malloc(sizeof(TreeNode));
        (*temp_pptr)->data = arr[count];
        (*temp_pptr)->left_child = NULL;
        (*temp_pptr)->right_child = NULL;
        enQ(queue, &(*temp_pptr)->left_child);
        enQ(queue, &(*temp_pptr)->right_child);
        ++count;
    }
    deleteQ(queue);
    return root;
}

//주어진 tree를 traverse 하며, 각 node에 thread pointer를 만든다.
void makeThread(TreeNode *root){
    TreeNode *now_node = root->left_child;
    TreeNode *prev_node = root;
    StackNode *stack = makeStack();
    while(1){
        for( ; now_node; now_node = now_node->left_child)
        {
            push(stack, now_node);
        }
        now_node = pop(stack);
        if (!now_node) {
            break;
        }
        if (!prev_node->right_child) {
            prev_node->right_thread = 1;
            prev_node->right_child = now_node;
        }
        else {
            prev_node->right_thread = 0;
        }
        if (!now_node->left_child) {
            now_node->left_thread = 1;
            now_node->left_child = prev_node;
        }
        else {
            now_node->left_thread = 0;
        }
        prev_node = now_node;
        now_node = now_node->right_child;
    }
    now_node = root;
    if (!prev_node->right_child) {
        prev_node->right_thread = 1;
        prev_node->right_child = now_node;
    }
    else {
        prev_node->right_thread = 0;
    }
    if (!now_node->left_child) {
        now_node->left_thread = 1;
        now_node->left_child = prev_node;
    }
    else {
        now_node->left_thread = 0;
    }
}

//inorder 방식으로 봤을 때, successor을 return
TreeNode* insucc(TreeNode *now_node){
    TreeNode *temp = now_node->right_child;
    if (!now_node->right_thread) {
        while(!temp->left_thread) {
            temp = temp->left_child;
        }
    }
    return temp;
}

//tree 전체를 inorder로 출력
void tinorder(FILE *output, TreeNode *root){
    TreeNode *temp = root;
    while(1){
        temp = insucc(temp);
        if(temp == root) break;
        fprintf(output,"%c ",temp->data);
    }
    fprintf(output, "\n");
}

//tree 전체를 free
void deleteTree(TreeNode *root){
    TreeNode *prev_node;
    TreeNode *temp;
    temp = insucc(root);
    prev_node = temp;
    while(1){
        temp = insucc(temp);
        free(prev_node);
        prev_node = temp;
        if(temp == root) break;
    }
    free(root);
}

int main(){
    int num;
    FILE *input = fopen("input.txt","r"), *output = fopen("output.txt","w");
    char data_temp[2];
    char *data_arr;
    TreeNode *root;
    int i;

    fscanf(input, "%d", &num);
    data_arr = (char*)malloc(sizeof(char)*num);
    for(i = 0; i < num; i++) {
        fscanf(input, "%s", data_temp);
        data_arr[i] = data_temp[0];
    }

    root = makeTree(data_arr, num);
    makeThread(root);
    tinorder(output, root);
    deleteTree(root);


    fclose(input);
    fclose(output);
    free(data_arr);
    return 0;
}

void push_stack(TreeNode* value, TreeNode** stack, int *top)
{
    stack[++*top] = value;
}

TreeNode* pop_stack(TreeNode** stack, int *top)
{
    if(*top < 0) return NULL;
    return stack[(*top)--];
}

void iter_inorder(TreeNode* root)
{
    TreeNode* stack[20];
    int top = -1;
    TreeNode* temp = root->left_child;
    while(1) {
        for(;temp;temp = temp->left_child) {
            push_stack(temp, stack, &top);
        }
        temp = pop_stack(stack, &top);
        if(!temp) break;
        printf("%c ", temp->data);
        temp = temp->right_child;
    }
}

void iter_levelorder(TreeNode* root)
{
    Queue* q = makeQ();
    TreeNode* temp = root->left_child;
    enQ(q,temp);
    while(1) {
        temp = deQ(q);
        if(!temp) break;
        printf("%c ", temp->data);
        if(temp->left_child)
            enQ(q, temp->left_child);
        if(temp->right_child)
            enQ(q, temp->right_child);
    }
}

void iter_preorder(TreeNode* root)
{
    TreeNode* stack[20];
    int top = -1;
    TreeNode* temp = root->left_child;
    while(1) {
        for(;temp;temp = temp->left_child) {
            printf("%c ", temp->data);
            push_stack(temp, stack, &top);
        }
        temp = pop_stack(stack, &top);
        if(!temp) break;
        temp = temp->right_child;
    }
}
