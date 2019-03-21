#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
	int arr[100];
	int size;
	int front,rear;
} Queue;

//비어있는 Queue 하나를 동적할당 해서 해당 주소를 return.
Queue* makeQ()
{
	Queue *temp;
	temp = (Queue*)malloc(sizeof(Queue));
	temp->size = 0;
	temp->front = 1;
	temp->rear = 0;
	return temp;
}

// Queue포인터와 value를 argument로 받아 해당 Queue에 value를 enqueue함. queue가 꽉 찼으면 Full을 fprint.
void enQ(FILE *output, Queue *que, int value)
{
	if(que->size == 100){
		fprintf(output, "Full\n");
		return;
	}
	que->arr[++que->rear] = value;
	++que->size;
	return;
}

// Queue포인터를 argument로 받아 해당 Queue에서 dequeue함. Queue가 비어있으면 Empty를 fprint, dequeue가 성공하면 해당 값을 fprint.
void deQ(FILE *output, Queue *que)
{
	if(que->size == 0){
		fprintf(output, "Empty\n");
		return;
	}
	fprintf(output, "%d\n", que->arr[que->front++]);
	--que->size;
	return;
}

//Queue포인터를 인수로 받아 해당 Queue를 free해줌.
void deleteQ(Queue *que)
{
	free(que);
}

int main()
{
	FILE *input = fopen("input3-2.txt","r"), *output = fopen("output.txt","w");
	int num, temp;
	char mode[4];
	Queue *que;

	fscanf(input,"%d",&num);

	que = makeQ();
	
	for(int i = 0; i < num; ++i){
		fscanf(input, "%s", mode);
		if(strcmp(mode, "enQ") == 0){
			fscanf(input, "%d", &temp);
			enQ(output, que, temp);
		}
		else if(strcmp(mode, "deQ") == 0){
			deQ(output, que);
		}
		else{
			continue;
		}
	}

	fclose(input);
	fclose(output);
	deleteQ(que);

	return 0;
}

