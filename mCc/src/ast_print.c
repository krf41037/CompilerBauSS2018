#include "mCc/ast_print.h"

#include <assert.h>

#include "mCc/ast_visit.h"

#define LABEL_SIZE 64

const char *mCc_ast_print_unary_op(enum mCc_ast_unary_op op)
{
	switch(op){
		case MCC_AST_UNARY_OP_MINUS: return "-";
		case MCC_AST_UNARY_OP_EXCLAM: return "!";
	}

	return "unknown op";
}

const char *mCc_ast_print_binary_op(struct mCc_ast_expression *expression)
{
    if(expression->binary_op_type == MCC_AST_BINARY_OP_TYPE_ADD){
      switch(expression->add_op){
        case MCC_AST_BINARY_OP_ADD: return "+";
        case MCC_AST_BINARY_OP_SUB: return "-";
      }
    }
    else if(expression->binary_op_type == MCC_AST_BINARY_OP_TYPE_MUL){
      switch(expression->mul_op){
        case MCC_AST_BINARY_OP_MUL: return "*";
        case MCC_AST_BINARY_OP_DIV: return "/";
      }
    }
    else if(expression->binary_op_type == MCC_AST_BINARY_OP_TYPE_BINARY){
      switch(expression->op){
        case MCC_AST_BINARY_OP_AND: return "&&";
        case MCC_AST_BINARY_OP_OR: return "||";
      }
    }
    else if(expression->binary_op_type == MCC_AST_BINARY_OP_TYPE_COMPARE){
      switch (expression->compare_op){
        case MCC_AST_BINARY_OP_SMT: return "<";
        case MCC_AST_BINARY_OP_GRT: return ">";
        case MCC_AST_BINARY_OP_GRE: return ">=";
        case MCC_AST_BINARY_OP_SME: return "<=";
        case MCC_AST_BINARY_OP_EQ: return "==";
        case MCC_AST_BINARY_OP_UEQ: return "!=";
      }
    }

	return "unknown op";
}


/* ------------------------------------------------------------- DOT Printer */

static void print_dot_begin(FILE *out)
{
	assert(out);

	fprintf(out, "digraph \"AST\" {\n");
	fprintf(out, "\tnodesep=0.6\n");
}

static void print_dot_end(FILE *out)
{
	assert(out);

	fprintf(out, "}\n");
}

static void print_dot_node(FILE *out, const void *node, const char *label)
{
	assert(out);
	assert(node);
	assert(label);

	fprintf(out, "\t\"%p\" [shape=box, label=\"%s\"];\n", node, label);
}

static void print_dot_edge(FILE *out, const void *src_node,
                           const void *dst_node, const char *label)
{
	assert(out);
	assert(src_node);
	assert(dst_node);
	assert(label);

	fprintf(out, "\t\"%p\" -> \"%p\" [label=\"%s\"];\n", src_node, dst_node,
	        label);
}

static void print_dot_expression_literal(struct mCc_ast_expression *expression,
                                         void *data)
{
	assert(expression);
	assert(data);

	FILE *out = data;
	print_dot_node(out, expression, "expr: lit");
	print_dot_edge(out, expression, expression->literal, "literal");
}

static void
print_dot_expression_binary_op(struct mCc_ast_expression *expression,
                               void *data)
{
	assert(expression);
	assert(data);

	char label[LABEL_SIZE] = { 0 };
	snprintf(label, sizeof(label), "expr: %s",
	         mCc_ast_print_binary_op(expression));

	FILE *out = data;
	print_dot_node(out, expression, label);
	print_dot_edge(out, expression, expression->lhs, "lhs");
	print_dot_edge(out, expression, expression->rhs, "rhs");
}

static void
print_dot_expression_unary_op(struct mCc_ast_expression *expression,
				void *data)
{
	assert(expression);
	assert(data);

        char label[LABEL_SIZE] = { 0 };
	snprintf(label, sizeof(label), "expr: %s",
		 mCc_ast_print_unary_op(expression->unary_op));
	FILE *out = data;
	print_dot_node(out, expression, label);
	print_dot_edge(out, expression, expression->u_rhs, "rhs");

}

static void print_dot_expression_parenth(struct mCc_ast_expression *expression,
                                         void *data)
{
	assert(expression);
	assert(data);

	FILE *out = data;
	print_dot_node(out, expression, "( )");
	print_dot_edge(out, expression, expression->expression, "expression");
}

static void print_dot_literal_int(struct mCc_ast_literal *literal, void *data)
{
	assert(literal);
	assert(data);

	char label[LABEL_SIZE] = { 0 };
	snprintf(label, sizeof(label), "%ld", literal->i_value);

	FILE *out = data;
	print_dot_node(out, literal, label);
}

static void print_dot_literal_float(struct mCc_ast_literal *literal, void *data)
{
	assert(literal);
	assert(data);

	char label[LABEL_SIZE] = { 0 };
	snprintf(label, sizeof(label), "%f", literal->f_value);

	FILE *out = data;
	print_dot_node(out, literal, label);
}

static void print_dot_literal_bool(struct mCc_ast_literal *literal, void *data)
{
	assert(literal);
	assert(data);

	char label[LABEL_SIZE] = { 0 };
	snprintf(label, sizeof(label), "%i", literal->b_value);

	FILE *out = data;
	print_dot_node(out, literal, label);
}

static void print_dot_literal_string(struct mCc_ast_literal *literal, void *data)
{
        assert(literal);
        assert(data);

        char* print_string = literal->s_value;
        int i;
        for(i = 0; print_string[i] != '\0'; ++i) {
        }
        print_string[--i] = '\\';

        char label[LABEL_SIZE] = { 0 };
        snprintf(label, sizeof(label), "%c%s%c", '\\', print_string, '"');

        FILE *out = data;
        print_dot_node(out, literal, label);
}

static void print_dot_literal_identifier(struct mCc_ast_literal *literal, void *data)
{
        assert(literal);
        assert(data);

        char label[LABEL_SIZE] = { 0 };
        snprintf(label, sizeof(label), "%s", literal->id_value);

        FILE *out = data;
        print_dot_node(out, literal, label);
}

static struct mCc_ast_visitor print_dot_visitor(FILE *out)
{
	assert(out);

	return (struct mCc_ast_visitor){
		.traversal = MCC_AST_VISIT_DEPTH_FIRST,
		.order = MCC_AST_VISIT_PRE_ORDER,

		.userdata = out,

		.expression_literal = print_dot_expression_literal,
		.expression_binary_op = print_dot_expression_binary_op,
		.expression_unary_op = print_dot_expression_unary_op,
		.expression_parenth = print_dot_expression_parenth,

		.literal_int = print_dot_literal_int,
		.literal_float = print_dot_literal_float,
                .literal_bool = print_dot_literal_bool,
                .literal_string = print_dot_literal_string,
                .literal_identifier = print_dot_literal_identifier,
	};
}

void mCc_ast_print_dot_expression(FILE *out,
                                  struct mCc_ast_expression *expression)
{
	assert(out);
	assert(expression);

	print_dot_begin(out);

	struct mCc_ast_visitor visitor = print_dot_visitor(out);
	mCc_ast_visit_expression(expression, &visitor);

	print_dot_end(out);
}

void mCc_ast_print_dot_literal(FILE *out, struct mCc_ast_literal *literal)
{
	assert(out);
	assert(literal);

	print_dot_begin(out);

	struct mCc_ast_visitor visitor = print_dot_visitor(out);
	mCc_ast_visit_literal(literal, &visitor);

	print_dot_end(out);
}
