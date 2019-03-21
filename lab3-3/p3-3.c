#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define DEBUG(code) printf("line:%d,code:%d\n",__LINE__,code);

typedef struct{
	char arr[100];
	int top;
}Stack;

//빈 Stack을 동적할당 후 해당 주소를 return
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

//argument로 받은 Stack 포인터와 value를 이용해 해당 스택에 char 값을 push
void push(Stack *stack, char value)
{
	if(stack->top == 99){
		return;
	}
	stack->arr[++stack->top] = value;
	return;
}

//argument로 받은 Stack이 비어있으면 true return
int isEmpty(Stack *stack)
{
	if(stack->top < 0)
		return 1;
	return 0;
}

//arugument로 받은 Stack 포인터를 이용해 해당 스택에서 pop해서 그 값을 return
char pop(Stack *stack)
{
	if(isEmpty(stack)){
		return 0;
	}
	return stack->arr[stack->top--];
}

//argument로 받은 Stack의 top 원소를 return
char top(Stack *stack)
{
	if(stack->top < 0) return 0;
	return stack->arr[stack->top];
}

//argument로 받은 operator의 precedence값을 return
int getPrec(char chr)
{
	if(!chr) return 0;
	if(chr == '+' || chr == '-')
		return 1;
	if(chr == '*' || chr == '/' || chr == '%')
		return 2;
	if(chr == '(')
		return 0;

	return -1;
}

//operator를 char형 인수로 받아서 postfix를 만드는데에 필요한 연산을 수행.
void operatorCtrl(Stack* stack, char chr, char *form, int *idx)
{
	int my_prec = getPrec(chr);
	int top_prec = getPrec(top(stack));
	if(top_prec < my_prec){
		push(stack,chr);
		return;
	}
	else{
		form[++*idx] = pop(stack);
		operatorCtrl(stack,chr,form,idx);
		return;
	}
}

//operator를 받아서 연산을 수행 후 return
int operate(char opr, int num1, int num2)
{
	switch(opr){
		case '*':
			return num1 * num2;
		case '/':
			return num1 / num2;
		case '%':
			return num1 % num2;
		case '+':
			return num1 + num2;
		case '-':
			return num1 - num2;
	}
}

//postfix로 표현된 수식의 char배열을 받아서 계산결과를 return
int computePostfix(char *arr)
{
	int idx;
	Stack *stack = makeStack();
	int len = strlen(arr);
	int result;
	int num1,num2;
	for(idx = 0; idx < len; ++idx){
		if(arr[idx] > '0' && arr[idx] <= '9'){
			push(stack,arr[idx] - '0');
			continue;
		}
		num2 = pop(stack);
		num1 = pop(stack);
		push(stack, operate(arr[idx],num1,num2));
	}
	result = pop(stack);
	deleteStack(stack);
	return result;
}

int main()
{
	FILE *input = fopen("input3-3.txt","r"), *output = fopen("output.txt", "w");
	char temp;
	char form[101];
	int idx = -1;
	int top_prec = -1;
	Stack *stack;

	stack = makeStack();
	
	fprintf(output, "Infix Form : ");
	while(1){
		fscanf(input, "%c", &temp);
		if(temp == '#'){
			while(!isEmpty(stack)){
				form[++idx] = pop(stack);
			}
			break;
		}
		fprintf(output,"%c",temp);
		if(temp > '0' && temp <= '9'){
			form[++idx] = temp;
			continue;
		}
		if(temp == '('){
			push(stack,temp);
			continue;
		}
		if(temp == ')'){
			while(top(stack) != '('){
				form[++idx] = pop(stack);
			}
			pop(stack);
			continue;
		}
		operatorCtrl(stack,temp,form,&idx);
	}
	form[++idx] = 0;
	fprintf(output,"\nPostfix Form : %s\nEvaluation Result : %d\n",form, computePostfix(form));
	
	deleteStack(stack);
	fclose(input);
	fclose(output);
	return 0;
}
