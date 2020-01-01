#include "globals.h"

struct Node tree();
struct Prediction predict(unsigned short* record, struct Node node, unsigned char no_of_classes);

unsigned short** copy();







struct Question
{
	unsigned char column;
	unsigned short value;
};


struct Node
{
	unsigned char is_leaf;
	unsigned char* counts;

	struct Question question;
	struct Node* true_branch;
	struct Node* false_branch;
};

struct Prediction
{
	unsigned short label;
	float confidence;
};
