/****************************************************************************
FILE      : node-types.h
SUBJECT   : Declaration of the various node types.
PROGRAMMER: (C) Copyright 2003 by Peter Chapin

Please send comments or bug reports to

     Peter Chapin
     Vermont Technical College
     Williston, VT 05495
     PChapin@vtc.vsc.edu
****************************************************************************/

#ifndef NODETYPES_H
#define NODETYPES_H

#include <string>
#include <map>

enum symbol_type { tINT, tLONG, tINTARRAY, tLONGARRAY , tERROR};

// Make a structure to store all of the attributes of the variables.
struct symbol_attrs {
  symbol_type vartype;
  int num_elements; 
};

// Declaring the symbol table.
extern std::map<std::string, symbol_attrs> symbol_table;

//
// Expression syntax nodes
//

// typedef int tlocation_t;

class expr_node {
public:
  virtual ~expr_node() { }
  virtual symbol_type generate() = 0;
};


class and_node : public expr_node {
private:
  expr_node *left, *right;
  int line_number;
public:
  and_node(expr_node *l, expr_node *r, int line) 
    : left(l), right(r), line_number(line) { }
  virtual ~and_node() { delete left; delete right; }
  virtual symbol_type generate();
};


class or_node : public expr_node {
private:
  expr_node *left, *right;
  int line_number;

public:
  or_node(expr_node *l, expr_node *r, int line) 
    : left(l), right(r), line_number(line) { }
  virtual ~or_node() { delete left; delete right; }
  virtual symbol_type generate();
};


enum relational_type
  { EQ_TYPE, NE_TYPE, LT_TYPE, GT_TYPE, LE_TYPE, GE_TYPE };


class relational_node : public expr_node {
private:
  expr_node *left, *right;
  relational_type type;
  int line_number;

public:
  relational_node(expr_node *l, expr_node *r, relational_type t, int line) 
    : left(l), right(r), type(t), line_number(line) { }
  virtual ~relational_node() { delete left; delete right; }
  virtual symbol_type generate();
};


class add_node : public expr_node {
private:
  expr_node *left, *right;
  int line_number;

public:
  add_node(expr_node *l, expr_node *r, int line) 
    : left(l), right(r), line_number(line) { }
  virtual ~add_node() { delete left; delete right; }
  virtual symbol_type generate();
};


class sub_node : public expr_node {
private:
  expr_node *left, *right;
  int line_number;

public:
  sub_node(expr_node *l, expr_node *r, int line) 
    : left(l), right(r), line_number(line) { }
  virtual ~sub_node() { delete left; delete right; }
  virtual symbol_type generate();
};


class mul_node : public expr_node {
private:
  expr_node *left, *right;
  int line_number;

public:
  mul_node(expr_node *l, expr_node *r, int line) 
    : left(l), right(r), line_number(line) { }
  virtual ~mul_node() { delete left; delete right; }
  virtual symbol_type generate();
};


class div_node : public expr_node {
private:
  expr_node *left, *right;
  int line_number;

public:
  div_node(expr_node *l, expr_node *r, int line) 
    : left(l), right(r), line_number(line) { }
  virtual ~div_node() { delete left; delete right; }
  virtual symbol_type generate();
};


class num_node : public expr_node {
private:
  int value;
  symbol_type num_type;
  int line_number;

public:
  num_node(int v, symbol_type t, int line) 
    : value(v), num_type(t), line_number(line) { }
  virtual symbol_type generate();
};


class id_node : public expr_node {
private:
  std::string name;
  int line_number;
  
public:
  id_node(const std::string &n, int line) 
    : name(n), line_number(line) { }
  virtual symbol_type generate();
};


class arrayref_node : public expr_node {
private:
  std::string array_name;
  expr_node *index_expression;
  int line_number;

public:
  arrayref_node(std::string &n, expr_node *index, int line) :
    array_name(n), index_expression(index), line_number(line) { }
  virtual ~arrayref_node() { delete index_expression; }
  virtual symbol_type generate();
};


//
// Statement syntax nodes
//


class stmt_node {
public:
  virtual ~stmt_node() { }
  virtual void generate() = 0;
};

class statementlist_node : public stmt_node {
private:
  stmt_node *statement_list;
  stmt_node *statement;

public:
  statementlist_node(stmt_node *s_list, stmt_node *s) :
    statement_list(s_list), statement(s) { }
  virtual ~statementlist_node() { delete statement_list; delete statement; }
  virtual void generate();
};


class if_node : public stmt_node {
private:
  expr_node *expression;
  stmt_node *then_clause;
  stmt_node *else_clause;

public:
  if_node(expr_node *e, stmt_node *t_clause, stmt_node *e_clause) :
    expression(e), then_clause(t_clause), else_clause(e_clause)
  { }

  virtual ~if_node()
    { delete expression; delete then_clause, delete else_clause; }
  virtual void generate();
};


class while_node : public stmt_node {
private:
  expr_node *expression;
  stmt_node *statement_list;
  
public:
  while_node(expr_node *e, stmt_node *s_list) :
    expression(e), statement_list(s_list)
  { }

  virtual ~while_node()
    { delete expression; delete statement_list; }
  virtual void generate();
};


class return_node : public stmt_node {
private:
  expr_node *expression;

public:
  return_node(expr_node *e) : expression(e) { }

  virtual ~return_node()
    { delete expression; }
  virtual void generate();
};


class assignment_node : public stmt_node {
private:
  std::string name;
  expr_node *expression;
  int line_number;
  
public:
  assignment_node(const std::string &n, expr_node *e, int line) :
    name(n), expression(e), line_number(line) { }

  virtual ~assignment_node()
    { delete expression; }
  virtual void generate();
};


class arrayassign_node : public stmt_node {
private:
  std::string array_name;
  expr_node *index_expression;
  expr_node *expression;
  int line_number;

public:
  arrayassign_node(const std::string &n, expr_node *index, expr_node *e, int line) :
    array_name(n), index_expression(index), expression(e), line_number(line) { }

  virtual ~arrayassign_node()
    { delete index_expression; delete expression; }
  virtual void generate();
};

#endif
