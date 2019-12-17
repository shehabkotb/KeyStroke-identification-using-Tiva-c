#define CLASS_INDEX 9
#define NO_OF_FEATURES 9

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

struct Prediction
{
	unsigned short label;
	float confidence;
};

unsigned char ask_question(unsigned short* record, struct Question q);

void partition(unsigned short** records, unsigned char size, struct Question q, unsigned short*** true_records, unsigned short*** false_records,
	unsigned char* no_of_true_records, unsigned char* no_of_false_records);

unsigned char* get_class_counts(unsigned short** records, unsigned char size, unsigned char no_of_classes);

float gini(unsigned short** records, unsigned char size, unsigned char no_of_classes);

float info_gain(unsigned short** left, unsigned char left_size, unsigned short** right, unsigned char right_size, float current_impurity,
	unsigned char no_of_classes);

void find_best_question(unsigned short** records, unsigned char size, unsigned char no_of_classes, struct Question* best_question,
	float* best_gain);

struct Node build_tree(unsigned short** records, unsigned char size, unsigned char no_of_classes);

struct Prediction predict(unsigned short* record, struct Node node, unsigned char no_of_classes);

struct Element* new_element(struct Node* node, unsigned short** records, unsigned char size);

void push(struct Stack* stack, struct Element* element);

struct Element* pop(struct Stack* stack);

unsigned char is_empty(struct Stack* stack);