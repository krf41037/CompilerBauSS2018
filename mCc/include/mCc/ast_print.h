#ifndef MCC_AST_PRINT_H
#define MCC_AST_PRINT_H

#include <stdio.h>

#include "mCc/ast.h"

#ifdef __cplusplus
extern "C" {
#endif

const char *mCc_ast_print_unary_op(enum mCc_ast_unary_op op);

const char *mCc_ast_print_binary_op(struct mCc_ast_expression *expression);

/* ------------------------------------------------------------- DOT Printer */

void mCc_ast_print_dot_expression(FILE *out,
                                  struct mCc_ast_expression *expression);

void mCc_ast_print_dot_literal(FILE *out, struct mCc_ast_literal *literal);

#ifdef __cplusplus
}
#endif

#endif
