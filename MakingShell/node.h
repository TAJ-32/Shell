#ifndef NODE_H
#define NODE_H

enum node_type_e { //defines the types of the nodes we will put in our Abstract Sytntax Tree
	NODE_COMMAND, //the command

	NODE_VAR, //variable name/word
};

enum val_type_e { //represents the types of values we can store in a given node structure
	VAL_SINT = 1,
	VAL U_INT,
	VAL_SLLONG,
	VAL_ULLONG,

	VAL_FLOAT,
	VAL_DOUBLE,
	VAL_CHR,
	VAL_STR, //used for simple commands
};

union symval_u { //represents the values themselves we can store in a given node structure. Each node can only have one type of value
	long			sint;
	unsigned long		uint;
	long long		sllong;
	unsigned long long	ullong;
	double 			sfloat;
	long double 		ldouble;
	char 			chr;
	char 		       *str;
}; //specifically, this member of the node_s structure will represent a pointer to a value of whatever of these types is specified

struct node_s { //this is the struct for a node itself. It has these attributes/members
	enum node_type_e type; //the type of node it will be
	enum val_type_e val_type; //type of value we can store inside node
	union symval_u val; //the actual value we store inside the node
	int children; //number of children a node has
	struct node_s *first_child; //a pointer to a node's first child of type node_s
	struct node_s *next_sibling, *prev_sibling; //a pointer to a node's next/prev sibling of type node_s
}

struct node_s *new_node(enum node_type_etype); //creates a new node (returns a pointer to it)
void add_child_node(struct node_s *parent, struct node_s *child);
void free_node_tree(struct node_s *node);
void set_node_val_str(struct node_s *node, char *val);

#endif
