#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct{
	int arr[100];
	int top;
}Stack;

Stack* makeStack()
{
	Stack *temp = (Stack*)malloc(sizeof(Stack));
	temp->top = -1;
	return temp;
}

//argument로 받은 Stack 포인터를 사용해서 해당 Stack을 free
void deleteStack(Stack *stack)
{
	free(stack);
}

//argument로 받은 Stack 포인터와 value를 이용해 해당 스택에 int 값을 push, Stack이 꽉 찼으면 Full을 fpirnt
void push(FILE *output, Stack *stack, int value)
{
	if(stack->top == 99){
		fprintf(output,"Full\n");
		return;
	}
	stack->arr[++stack->top] = value;
	return;
}

//arugument로 받은 Stack 포인터를 이용해 해당 스택에서 pop함. 스택이 비어있으면 Empty를 fprint
void pop(FILE *output, Stack *stack)
{
	if(stack->top < 0){
		fprintf(output,"Empty\n");
		return;
	}
	fprintf(output, "%d\n", stack->arr[stack->top--]);
	return;
}


int main()
{
	FILE *input = fopen("input3-1.txt","r"), *output = fopen("output.txt", "w");
	int num, temp;
	char mode[5];

	fscanf(input, "%d", &num);

	Stack *stack = makeStack();
	
	for(int i = 0; i < num; ++i){
		fscanf(input, "%s", mode);
		if(strcmp(mode,"push") == 0){
			fscanf(input, "%d", &temp);
			push(output, stack, temp);
		}
		else if(strcmp(mode,"pop") == 0){
			pop(output, stack);
		}
	}
	fclose(input);
	fclose(output);
	deleteStack(stack);

	return 0;
}
