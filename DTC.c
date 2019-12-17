#include "stdlib.h"
#include "DTC.h"

unsigned char ask_question(unsigned short* record, struct Question q)
{
	return (unsigned char)(record[q.column] >= q.value);
}

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
		if (true_or_false[i] = ask_question(records[i], q))
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

unsigned char* get_class_counts(unsigned short** records, unsigned char size, unsigned char no_of_classes)
{
	unsigned char* counts = (unsigned char*)calloc((no_of_classes + 1), sizeof(unsigned char));

	for (unsigned char i = 0; i < size; i++)
	{
		counts[records[i][CLASS_INDEX]]++;
	}

	return counts;
}

float gini(unsigned short** records, unsigned char size, unsigned char no_of_classes)
{
	unsigned char* counts = get_class_counts(records, size, no_of_classes);
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

float info_gain(unsigned short** left, unsigned char left_size, unsigned short** right, unsigned char right_size, float current_impurity,
	unsigned char no_of_classes)
{
	float p = (left_size * 1.0F) / (left_size + right_size);
	return current_impurity - (p * gini(left, left_size, no_of_classes)) - ((1 - p) * gini(right, right_size, no_of_classes));
}

void find_best_question(unsigned short** records, unsigned char size, unsigned char no_of_classes, struct Question* best_question,
	float* best_gain)
{
	float gain;
	*best_gain = 0;
	struct Question question;
	float current_impurity = gini(records, size, no_of_classes);
	unsigned short** true_records;
	unsigned short** false_records;
	unsigned char no_of_true_records;
	unsigned char no_of_false_records;

	for (unsigned char i = 0; i < NO_OF_FEATURES; i++)
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

			gain = info_gain(true_records, no_of_true_records, false_records, no_of_false_records, current_impurity, no_of_classes);
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
		find_best_question(current_records, current_size, no_of_classes, &question, &gain);
		if (gain == 0)
		{
			node->is_leaf = 1;
			node->counts = get_class_counts(current_records, current_size, no_of_classes);
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

	if (ask_question(record, node.question) == 1)
	{
		return predict(record, *node.true_branch, no_of_classes);
	}
	else
	{
		return predict(record, *node.false_branch, no_of_classes);
	}
}

struct Element* new_element(struct Node* node, unsigned short** records, unsigned char size)
{
	struct Element* element = (struct Element*)malloc(sizeof(struct Element));
	element->node = node;
	element->records = records;
	element->size = size;
	element->next = NULL;
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