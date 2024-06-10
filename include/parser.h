#ifndef PARSER_H_
#define PARSER_H_

#include <vector>
#include <memory>
#include "lexer.h"
#include <iostream>

template<typename T>
bool operator==(const std::vector<std::unique_ptr<T>>& n1, const std::vector<std::unique_ptr<T>>& n2) {
  if (n1.size() != n2.size()) {
    return false;
  }
  for (size_t i = 0; i < n1.size(); i++) {
    if (!(*n1[i] == *n2[i])) {
      return false;
    }
  }
  return true;
}

typedef enum class Op {
  Plus = '+',
  Minus = '-',
  Times = '*',
  Divide = '/',
  Modulus = '%'
} Op;

class Node {
public:
  virtual bool operator==(const Node& other) const __attribute__((used)) {
    return false;
  }
};

bool operator==(const std::vector<std::unique_ptr<Node>>& n1, const std::vector<std::unique_ptr<Node>>& n2);

class StmtNode : public Node {
public:
};

class ExprNode : public StmtNode {
public:
};

class NumberExprNode : public ExprNode {
  double Val;

public:
  virtual bool operator==(const Node& other_) const override __attribute__((used)) {
    if (auto other=dynamic_cast<const NumberExprNode*>(&other_)) {
      return (Val == other->Val);
    } else {
      return Node::operator==(other_);
    }
  }

  NumberExprNode(double val) : Val(val) {}
};

class VariableExprNode : public ExprNode {
  std::string Name;
  std::vector<std::unique_ptr<ExprNode>> Args;

public:
  virtual bool operator==(const Node& other_) const override __attribute__((used)) {
    if (auto other=dynamic_cast<const VariableExprNode*>(&other_)) {
      return (Name == other->Name && (Args == other->Args));
    } else {
      return Node::operator==(other_);
    }
  }

  VariableExprNode(std::string identifier, std::vector<std::unique_ptr<ExprNode>> args)
    : Name(identifier), Args(std::move(args)) {}
};

class BinaryExprNode : public ExprNode {
  Op Oper;
  std::unique_ptr<ExprNode> LHS;
  std::unique_ptr<ExprNode> RHS;

public:
  virtual bool operator==(const Node& other_) const override __attribute__((used)) {
    if (auto other=dynamic_cast<const BinaryExprNode*>(&other_)) {
      return ((Oper == other->Oper) && \
	      (*LHS == *(other->LHS)) &&	\
	      (*RHS == *(other->RHS)));
    } else {
      return Node::operator==(other_);
    }
  }

  BinaryExprNode(enum Op oper, std::unique_ptr<ExprNode> lhs, std::unique_ptr<ExprNode> rhs)
    : Oper(oper), LHS(std::move(lhs)), RHS(std::move(rhs)) {}
};

class ArrayExprNode : public ExprNode {
  std::vector<std::unique_ptr<ExprNode>> Entries;

public:
  virtual bool operator==(const Node& other_) const override __attribute__((used)) {
    if (auto other=dynamic_cast<const ArrayExprNode*>(&other_)) {
      return (Entries == other->Entries);
    } else {
      return Node::operator==(other_);
    }
  }

  ArrayExprNode(std::vector<std::unique_ptr<ExprNode>> entries)
    : Entries(std::move(entries)) {}
};

class AssgnNode : public StmtNode {
  std::string Name;
  std::vector<std::unique_ptr<NumberExprNode>> Size;
  std::unique_ptr<ExprNode> Expr;
  bool IsDecl;

public:
  virtual bool operator==(const Node& other_) const override __attribute__((used)) {
    if (auto other=dynamic_cast<const AssgnNode*>(&other_)) {
      return ((Name == other->Name) && \
	      (Size == other->Size) && \
	      (*Expr == *(other->Expr)) && \
	      (IsDecl == other->IsDecl));
    } else {
      return Node::operator==(other_);
    }
  }

  AssgnNode(std::string identifier, std::vector<std::unique_ptr<NumberExprNode>> size, std::unique_ptr<ExprNode> expr, bool isDecl)
    : Name(identifier), Size(std::move(size)), Expr(std::move(expr)), IsDecl(isDecl) {};
};

class PrototypeNode : public Node {
  std::string Name;
  std::vector<std::string> Args;

public:
  virtual bool operator==(const Node& other_) const override __attribute__((used)) {
    if (auto other=dynamic_cast<const PrototypeNode*>(&other_)) {
      return (Name == other->Name && (Args == other->Args));
    } else {
      return Node::operator==(other_);
    }
  }

  PrototypeNode(std::string identifier, std::vector<std::string> args)
    : Name(identifier), Args(std::move(args)) {};
};

class FunctionNode : public Node {
  std::unique_ptr<PrototypeNode> Prototype;
  std::vector<std::unique_ptr<StmtNode>> Body;

public:
  virtual bool operator==(const Node& other_) const override __attribute__((used)) {
    if (auto other=dynamic_cast<const FunctionNode*>(&other_)) {
      return ((*Prototype == *(other->Prototype)) && (Body == other->Body));
    } else {
      return Node::operator==(other_);
    }
  }

  FunctionNode(std::unique_ptr<PrototypeNode> prototype, std::vector<std::unique_ptr<StmtNode>> body)
    : Prototype(std::move(prototype)), Body(std::move(body)) {};
};

class Parser {
private:
  static Parser* Instance;
  static Parser& getInstance();
  size_t TokenPos;
  std::vector<LexToken> Tokens;
  std::map<Op, int> BinopPrecedence;
  void reset();
  void rollback();
  void syncTo(TokenType);
  LexToken& peakNextToken();
  LexToken& getNextToken();
  LexToken& expect(TokenType, TokenType);
  bool accept(TokenType);
  std::unique_ptr<AssgnNode> parseVarDecl();
  std::unique_ptr<ArrayExprNode> parseArray();
  std::vector<std::unique_ptr<NumberExprNode>> parseSize();
  std::unique_ptr<AssgnNode> parseAssgn();
  std::unique_ptr<NumberExprNode> parseNumberExpr();
  std::unique_ptr<ExprNode> parseParenExpr();
  std::unique_ptr<ExprNode> parseIdentifier();
  std::unique_ptr<ExprNode> parsePrimary();
  std::unique_ptr<ExprNode> parseExpr();
  std::unique_ptr<StmtNode> parseStmt();
  std::vector<std::unique_ptr<StmtNode>> parseStmtList();
  std::unique_ptr<BinaryExprNode> parseBinOpRhs(int, std::unique_ptr<ExprNode>);
  std::unique_ptr<PrototypeNode> parsePrototype();
  std::unique_ptr<PrototypeNode> parseExtern();
  std::unique_ptr<FunctionNode> parseFunction();
  Parser() = default;

public:
  static std::vector<std::unique_ptr<Node>> parse(std::vector<LexToken>);
};

#endif
