#include "stdlib.h"
#include "DecisionTree.h"

#define NUM_CLASSES 9
#define NUM_FEATURES 9

unsigned short** copy(){
	unsigned short **arr = (unsigned short **)malloc(20 * sizeof(unsigned short *)); 
	for (int i=0; i<20; i++) {
		arr[i] = (unsigned short *)malloc(WORDLENGTH * sizeof(unsigned short)); 
	}
	
	for(int i=0;i<20;i++){
		for(int j=0;j<10;j++){
			arr[i][j] = read.data[i][j];
		}
	}
	return arr;
}

unsigned char* num_of_classes(unsigned short** records, unsigned char size, unsigned char no_of_classes) {
	unsigned char* counts = (unsigned char*)calloc((no_of_classes + 1), sizeof(unsigned char));

	for (unsigned char i = 0; i < size; i++) {
		counts[records[i][NUM_CLASSES]]++;
	}
	return counts;
}

float Gini_Index(unsigned short** records, unsigned char size, unsigned char no_of_classes)
{
	unsigned char* counts = num_of_classes(records, size, no_of_classes);
	float impurity = 1;
	float prob_of_class;

	for (unsigned char i = 1; i <= no_of_classes; i++)
	{
		prob_of_class = (counts[i] * 1.0F) / size;
		impurity -= (prob_of_class * prob_of_class);
	}
	free(counts);

	return impurity;
}

unsigned char query(unsigned short* record, struct Question q) {
	return (unsigned char)(record[q.column] >= q.value);
}

float Igain(unsigned short** left, unsigned char left_size, unsigned short** right, unsigned char right_size, float current_impurity, unsigned char no_of_classes) {
	float p = (left_size * 1.0F) / (left_size + right_size);
	return current_impurity - (p * Gini_Index(left, left_size, no_of_classes)) - ((1 - p) * Gini_Index(right, right_size, no_of_classes));
}


struct Element
{
	struct Node* node;
	unsigned short** records;
	unsigned char size;
	struct Element* next;
};

struct Stack
{
	struct Element* element;
};

void partition(unsigned short** records, unsigned char size, struct Question q, unsigned short*** true_records, unsigned short*** false_records,
	unsigned char* no_of_true_records, unsigned char* no_of_false_records)
{
	*no_of_true_records = 0;
	*no_of_false_records = 0;
	unsigned char true_index = 0;
	unsigned char false_index = 0;
	unsigned char* true_or_false = (unsigned char*)malloc(sizeof(unsigned char) * size);

	for (unsigned char i = 0; i < size; i++)
	{
		if (true_or_false[i] = query(records[i], q))
		{
			(*no_of_true_records)++;
		}
		else
		{
			(*no_of_false_records)++;
		}
	}

	*true_records = (unsigned short**)malloc(sizeof(unsigned short*) * (*no_of_true_records));
	*false_records = (unsigned short**)malloc(sizeof(unsigned short*) * (*no_of_false_records));

	for (unsigned char i = 0; i < size; i++)
	{
		if (true_or_false[i])
		{
			(*true_records)[true_index++] = records[i];
		}
		else
		{
			(*false_records)[false_index++] = records[i];
		}
	}

	free(true_or_false);
}

void best_gain(unsigned short** records, unsigned char size, unsigned char no_of_classes, struct Question* best_question, float* best_gain) {
	float gain;
	*best_gain = 0;
	struct Question question;
	float current_impurity = Gini_Index(records, size, no_of_classes);
	unsigned short** true_records;
	unsigned short** false_records;
	unsigned char no_of_true_records;
	unsigned char no_of_false_records;

	for (unsigned char i = 0; i < NUM_FEATURES; i++)
	{
		for (unsigned char j = 0; j < size; j++)
		{
			question.column = i;
			question.value = records[j][i];
			partition(records, size, question, &true_records, &false_records, &no_of_true_records, &no_of_false_records);

			if ((no_of_true_records == 0) || (no_of_false_records == 0))
			{
				continue;
			}

			gain = Igain(true_records, no_of_true_records, false_records, no_of_false_records, current_impurity, no_of_classes);
			if (gain > *best_gain)
			{
				*best_gain = gain;
				*best_question = question;
			}

			free(true_records);
			free(false_records);
		}
	}
}

void push(struct Stack* stack, struct Element* element)
{
	element->next = stack->element;
	stack->element = element;
}

struct Element* pop(struct Stack* stack)
{
	struct Element* element = stack->element;
	stack->element = element->next;
	return element;
}

unsigned char is_empty(struct Stack* stack)
{
	if (stack->element == NULL)
	{
		return 1;
	}
	return 0;
}

struct Element* new_element(struct Node* node, unsigned short** records, unsigned char size)
{
	struct Element* element = (struct Element*)malloc(sizeof(struct Element));
	element->node = node;
	element->records = records;
	element->size = size;
	element->next = NULL;
	
}

struct Node build_tree(unsigned short** records, unsigned char size, unsigned char no_of_classes)
{
	struct Node* root = (struct Node*)malloc(sizeof(struct Node));
	struct Node* node = root;
	struct Stack* stack = (struct Stack*)malloc(sizeof(struct Stack));
	stack->element = NULL;
	struct Element* element;
	struct Question question;
	float gain;
	unsigned short** true_records;
	unsigned short** false_records;
	unsigned char no_of_true_records;
	unsigned char no_of_false_records;
	unsigned short** current_records = (unsigned short**)malloc(sizeof(unsigned short*) * size);
	for (unsigned char i = 0; i < size; i++)
	{
		current_records[i] = records[i];
	}
	unsigned char current_size = size;

	while (1)
	{
		best_gain(current_records, current_size, no_of_classes, &question, &gain);
		if (gain == 0)
		{
			node->is_leaf = 1;
			node->counts = num_of_classes(current_records, current_size, no_of_classes);
			if (is_empty(stack) == 1)
			{
				break;
			}
			element = pop(stack);
			node = element->node;
			current_size = element->size;
			free(current_records);
			current_records = (unsigned short**)malloc(sizeof(unsigned short*) * current_size);
			for (unsigned char i = 0; i < current_size; i++)
			{
				current_records[i] = element->records[i];
			}
			free(element->records);
			node->false_branch = (struct Node*)malloc(sizeof(struct Node));
			node = node->false_branch;
			free(element);
		}
		else
		{
			node->question = question;
			partition(current_records, current_size, question, &true_records, &false_records, &no_of_true_records, &no_of_false_records);
			free(current_records);
			current_records = (unsigned short**)malloc(sizeof(unsigned short*) * no_of_true_records);
			for (unsigned char i = 0; i < no_of_true_records; i++)
			{
				current_records[i] = true_records[i];
			}
			free(true_records);
			current_size = no_of_true_records;
			push(stack, new_element(node, false_records, no_of_false_records));
			node->true_branch = (struct Node*)malloc(sizeof(struct Node));
			node = node->true_branch;
		}
	}

	return *root;
}

struct Node tree(){
	unsigned short** z = copy();
	struct Node x = build_tree(z,20,2);
	
	for(int i = 0;i < 20; i++){
		free(z[i]);
	}
	free(z);

	return x;
}

struct Prediction predict(unsigned short* record, struct Node node, unsigned char no_of_classes)
{
	if (node.is_leaf == 1)
	{
		struct Prediction pred = { 1, node.counts[1] };
		for (unsigned char i = 2; i <= no_of_classes; i++)
		{
			if (node.counts[i] > node.counts[pred.label])
			{
				pred.label = i;
			}
			pred.confidence += node.counts[i];
		}
		pred.confidence = node.counts[pred.label] / pred.confidence;

		return pred;
	}

	if (query(record, node.question) == 1)
	{
		return predict(record, *node.true_branch, no_of_classes);
	}
	else
	{
		return predict(record, *node.false_branch, no_of_classes);
	}
}



