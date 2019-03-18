#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define DEBUG(code) printf("line:%d,code:%d\n",__LINE__,code);

typedef struct 
{
	int id;
	char name[31];
} Info;
typedef struct Node_
{
	Info info;
	struct Node_ *next;
}Node;

void printCurrentList(FILE* output,Node* head)
{
	fprintf(output, "Current List > ");
	if(!head->next) return;
	head = head->next;
	fprintf(output, "%d %s", head->info.id, head->info.name);
	while(head->next){
		head = head->next;
		fprintf(output, "-%d %s", head->info.id, head->info.name);
	}
	fprintf(output,"\n");
	return;
}

void insert(FILE* output,Node* head, Info info)
{
	Node* new_node = (Node*)malloc(sizeof(Node));
	Node* save = head;
	new_node->info = info;
	while(head->next && head->next->info.id < info.id){
		head = head->next;
	}
	if(!head->next){
		head->next = new_node;
		new_node->next = NULL;
	}
	else{
		if(head->next->info.id == info.id){
			fprintf(output,"Insertion Failed. ID %d already exists.\n",info.id);
			return;
		}
		new_node->next = head->next;
		head->next = new_node;
	}
	fprintf(output,"Insertion Success : %d\n",info.id);
	printCurrentList(output,save);
	return;
}

Node* makeEmptyList()
{
	Node* temp = (Node*)malloc(sizeof(Node));
	temp->info.id = 0;
	temp->next = NULL;

	return temp;
}

void deleteList(Node* head)
{
	Node* temp;
	while(head->next){
		temp = head;
		head = head->next;
		free(temp);
	}
	free(head);
	return;
}

void delete(FILE *output, Node *head, int id)
{
	Node *save = head;
	Node *temp;
	while(head->next && head->next->info.id != id)
		head = head->next;
	if(!head->next){
		fprintf(output,"Deletion Failed : Student ID %d is not in the list.\n",id);
		return;
	}
	temp = head->next->next;
	free(head->next);
	head->next = temp;
	fprintf(output,"Deletion Success : %d\n",id);
	printCurrentList(output,save);
	return;
}

void find(FILE *output, Node* head, int id)
{
	Node *save = head;
	while(head->next && head->next->info.id != id)
		head = head->next;
	if(!head->next){
		fprintf(output,"Find %d Failed. There is no student ID\n",id);
		return;
	}
	fprintf(output,"Find Success : %d %s\n",id,head->next->info.name);
	return;
}

void printAll(FILE *output, Node *head)
{
	fprintf(output,"-----LIST-----\n");
	while(head->next){
		head = head->next;
		fprintf(output,"%d %s\n",head->info.id,head->info.name);
	}
	fprintf(output,"--------------\n");
	return;
}

int main()
{
	FILE *input = fopen("input.txt","r"), *output = fopen("output.txt","w");
	char mode, temp_name[31];
	int temp_id, temp_name_len;
	Node* head = makeEmptyList();
	Info temp_info;
	while(fscanf(input,"%c",&mode) == 1){
		switch(mode){
			case '\r':
				continue;
			case 'i':
				fscanf(input,"%d %s",&temp_id,temp_name);
				temp_name_len = strlen(temp_name);
				temp_name[temp_name_len] = ' ';
				++temp_name_len;
				fscanf(input,"%s",temp_name + temp_name_len);
				temp_info.id = temp_id;
				strcpy(temp_info.name, temp_name);
				insert(output,head,temp_info);
				break;
			case 'd':
				fscanf(input,"%d",&temp_id);
				delete(output,head,temp_id);
				break;
			case 'f':
				fscanf(input,"%d",&temp_id);
				find(output,head,temp_id);
				break;
			case 'p':
				printAll(output,head);
				break;
			default:
				break;
		}
	}
	fclose(input);
	fclose(output);
	
	return 0;
}
