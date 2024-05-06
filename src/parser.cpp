#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "lexer.h"
#include "parser.h"

bool operator==(const std::vector<std::unique_ptr<Node>>& n1, const std::vector<std::unique_ptr<Node>>& n2) {
  std::cout << "Called vec" << std::endl;
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

Parser* Parser::Instance = nullptr;

Parser& Parser::getInstance() {
  if (nullptr == Instance) {
    Instance = new Parser;
  }
  return *Instance;
}

void Parser::reset() {
  TokenPos = 0;
  Tokens.clear();
}

void Parser::syncTo(TokenType syncToken) {
  if (syncToken == TokenType::Invalid) {
    return;
  }
  bool syncronized = false;
  LexToken nextToken = peakNextToken();
  while (!(nextToken.getType() == syncToken)) {
    nextToken = getNextToken();
  }
}

LexToken& Parser::peakNextToken() {
  return Tokens[TokenPos];
}

LexToken& Parser::getNextToken() {
  if (TokenPos > Tokens.size()) {
    // error
    // exit
  }
  LexToken& token = Tokens[TokenPos];
  TokenPos += 1;
  return token;
}

LexToken& Parser::expect(TokenType expectedTokenType, TokenType syncTokenType) {
  LexToken& nextToken = getNextToken();
  if (!(nextToken.getType() == expectedTokenType)) {
    // error!
    syncTo(syncTokenType);
  }
  return nextToken;
}

bool Parser::accept(TokenType tokenType) {
  return (peakNextToken().getType() == tokenType);  
}

std::unique_ptr<AssgnNode> Parser::parseVarDecl() {
  expect(TokenType::Var, TokenType::Identifier);
  LexToken idToken = expect(TokenType::Identifier, TokenType::Equal);
  std::string id = idToken.getStr();
  std::vector<std::unique_ptr<NumberExprNode>> size;
  if (accept(TokenType::LeftAngle)) {
    size = parseSize();
  } else {
    size.push_back(std::make_unique<NumberExprNode>(1));
  }
  expect(TokenType::Equal, TokenType::Invalid);
  std::unique_ptr<ExprNode> expr = parseExpr();
  return std::make_unique<AssgnNode>(id, std::move(size), std::move(expr), true);
}

std::unique_ptr<ArrayExprNode> Parser::parseArray() {
  expect(TokenType::LeftSquare, TokenType::Invalid);
  std::vector<std::unique_ptr<ExprNode>> arr;
  std::unique_ptr<ExprNode> expr = parseExpr();
  arr.push_back(std::move(expr));
  while (accept(TokenType::Comma)) {
    expect(TokenType::Comma, TokenType::Invalid);
    expr = parseExpr();
    arr.push_back(std::move(expr));
  }
  expect(TokenType::RightSquare, TokenType::Invalid);
  return std::make_unique<ArrayExprNode>(std::move(arr));
}

std::vector<std::unique_ptr<NumberExprNode>> Parser::parseSize() {
  expect(TokenType::LeftAngle, TokenType::Invalid);
  std::vector<std::unique_ptr<NumberExprNode>> size;
  LexToken numToken = expect(TokenType::Number, TokenType::Invalid);
  size.push_back(std::make_unique<NumberExprNode>(std::stod(numToken.getStr())));
  while (accept(TokenType::Comma)) {
    expect(TokenType::Comma, TokenType::Invalid);
    numToken = expect(TokenType::Number, TokenType::Invalid);
    size.push_back(std::make_unique<NumberExprNode>(std::stod(numToken.getStr())));
  }
  expect(TokenType::RightAngle, TokenType::Invalid);
  return size;
}

std::unique_ptr<AssgnNode> Parser::parseAssgn() {
  LexToken idToken = expect(TokenType::Identifier, TokenType::Invalid);
  std::string id = idToken.getStr();
  expect(TokenType::Equal, TokenType::Invalid);
  std::unique_ptr<ExprNode> expr = parseExpr();
  std::vector<std::unique_ptr<NumberExprNode>> size;
  size.push_back(std::make_unique<NumberExprNode>(1));
  return std::make_unique<AssgnNode>(id, std::move(size), std::move(expr), false);
}

std::unique_ptr<NumberExprNode> Parser::parseNumberExpr() {
  LexToken numToken = expect(TokenType::Number, TokenType::Invalid);
  double val = std::stod(numToken.getStr());
  return std::make_unique<NumberExprNode>(val);
}

std::unique_ptr<ExprNode> Parser::parseParenExpr() {
  expect(TokenType::LeftParen, TokenType::Invalid);
  std::unique_ptr<ExprNode> expr = parseExpr();
  expect(TokenType::RightParen, TokenType::Invalid);
  return expr;
}

std::unique_ptr<ExprNode> Parser::parseIdentifier() {
  LexToken idToken = expect(TokenType::Identifier, TokenType::Invalid);
  std::string id = idToken.getStr();
  std::vector<std::unique_ptr<ExprNode>> args;
  if (accept(TokenType::LeftParen)) {
    expect(TokenType::LeftParen, TokenType::Invalid);
    args.push_back(parseExpr());
    while (accept(TokenType::Comma)) {
      expect(TokenType::Comma, TokenType::Invalid);
      args.push_back(parseExpr());
    }
    expect(TokenType::RightParen, TokenType::Invalid);
  }
  return std::make_unique<VariableExprNode>(id, std::move(args));
}

std::unique_ptr<ExprNode> Parser::parsePrimary() {
  std::unique_ptr<ExprNode> expr;
  if (accept(TokenType::Number)) {
    expr = parseNumberExpr();
  } else if (accept(TokenType::LeftParen)) {
    expr = parseParenExpr();
  } else {
    expr = parseIdentifier();
  }
  return expr;
}

std::unique_ptr<ExprNode> Parser::parseExpr() {
  std::unique_ptr<ExprNode> expr;
  if (accept(TokenType::LeftSquare)) {
    expr = parseArray();
  } else {
    expr = parsePrimary();
    if (accept(TokenType::Operator)) {
      expr = parseBinOpRhs(0, std::move(expr));
    }
  }
  return expr;
}

std::unique_ptr<StmtNode> Parser::parseStmt() {
  std::unique_ptr<StmtNode> stmt;
  if (accept(TokenType::LeftSquare) || \
      accept(TokenType::Number) || \
      accept(TokenType::LeftParen)) {
    stmt = parseExpr();
  } else if (accept(TokenType::Var)) {
    stmt = parseVarDecl();
  } else {
    if (Tokens[TokenPos + 1].getType() == TokenType::Equal) {
      stmt = parseAssgn();
    } else {
      stmt = parseExpr();
    }
  }
  expect(TokenType::Semicolon, TokenType::Semicolon);
  return stmt;
}

std::vector<std::unique_ptr<StmtNode>> Parser::parseStmtList() {
  std::vector<std::unique_ptr<StmtNode>> stmts;
  while (accept(TokenType::Var) || \
	 accept(TokenType::LeftSquare) || \
	 accept(TokenType::Identifier) || \
	 accept(TokenType::Number)) {
    stmts.push_back(parseStmt());
  }
  return stmts;
}

std::unique_ptr<BinaryExprNode> Parser::parseBinOpRhs(int prec, std::unique_ptr<ExprNode> LHS) {
  LexToken opToken = expect(TokenType::Operator, TokenType::Invalid);
  Op op = (Op)(opToken.getStr().c_str()[0]);
  std::unique_ptr<ExprNode> rhs = parsePrimary();
  if (accept(TokenType::Operator)) {
      LexToken lookaheadOpToken =  expect(TokenType::Operator, TokenType::Invalid);
      Op lookaheadOp = (Op)(lookaheadOpToken.getStr().c_str()[0]);
      if (BinopPrecedence[lookaheadOp] > BinopPrecedence[op]) {
	std::unique_ptr<ExprNode> newRhs = parseBinOpRhs(1 + BinopPrecedence[lookaheadOp], std::move(rhs));
	rhs.swap(newRhs);
      }
    }
  return std::make_unique<BinaryExprNode>(op, std::move(LHS), std::move(rhs));
}

std::unique_ptr<PrototypeNode> Parser::parsePrototype() {
  LexToken idToken = expect(TokenType::Identifier, TokenType::LeftParen);
  std::string id = idToken.getStr();
  expect(TokenType::LeftParen, TokenType::Invalid);
  std::vector<std::string> args;
  if(accept(TokenType::Identifier)) {
    LexToken argToken = expect(TokenType::Identifier, TokenType::RightParen);
    args.push_back(argToken.getStr());
    while (accept(TokenType::Comma)) {
      expect(TokenType::Comma, TokenType::Invalid);
      argToken = expect(TokenType::Identifier, TokenType::RightParen);
      args.push_back(argToken.getStr());
    }
  }
  expect(TokenType::RightParen, TokenType::Invalid);
  return std::make_unique<PrototypeNode>(id, std::move(args));
}

std::unique_ptr<PrototypeNode> Parser::parseExtern() {
  expect(TokenType::Extern, TokenType::LeftParen);
  std::unique_ptr<PrototypeNode> prototype = parsePrototype();
  return prototype;
}

std::unique_ptr<FunctionNode> Parser::parseFunction() {
  expect(TokenType::Def, TokenType::LeftParen);
  std::unique_ptr<PrototypeNode> prototype = parsePrototype();
  expect(TokenType::LeftBrace, TokenType::Semicolon);
  std::vector<std::unique_ptr<StmtNode>> stmtList = parseStmtList();
  expect(TokenType::RightBrace, TokenType::Invalid);
  return std::make_unique<FunctionNode>(std::move(prototype), std::move(stmtList));
}

std::vector<std::unique_ptr<Node>> Parser::parse(std::vector<LexToken> tokens) {
  Parser parser = Parser::getInstance();
  parser.Tokens = tokens;
  parser.TokenPos = 0;
  parser.BinopPrecedence = {{Op::Plus, 10}, {Op::Minus, 10}, {Op::Times, 20}, {Op::Divide, 20}, {Op::Modulus, 20}};
  std::vector<std::unique_ptr<Node>> externFuncsAndDefs;
  std::unique_ptr<Node> func;

  while (!(parser.accept(TokenType::Eof))) {
    if (parser.accept(TokenType::Extern)) {
      func = parser.parseExtern();
    } else if (parser.accept(TokenType::Def)) {
      func = parser.parseFunction();
    } else {
      break;
    }
    externFuncsAndDefs.push_back(std::move(func));
  }
  return externFuncsAndDefs;
}
