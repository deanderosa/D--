#include <cctype>
#include <cstdlib>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include "lexer.h"

bool isAcceptState(LexState state) {
  return (int)state > 0;
}

template <typename T>
static void clearAndPush(std::stack<T>& fullStack, T&& item) {
  while (!fullStack.empty()) {
    fullStack.pop();
  }
  fullStack.push(item);
}

TokenType LexToken::getType() {
  return Type;
}

std::string LexToken::getStr() {
  return SourceStr;
}

void LexToken::commonInit(TokenType type) {
  Type = type;
  switch(type) {
  case TokenType::Def:
    SourceStr = "def";
    break;
  case TokenType::Extern:
    SourceStr = "extern";
    break;
  case TokenType::Var:
    SourceStr = "var";
    break;
  case TokenType::LeftSquare:
    SourceStr = "[";
    break;
  case TokenType::RightSquare:
    SourceStr = "[";
    break;
  case TokenType::LeftAngle:
    SourceStr = "<";
    break;
  case TokenType::RightAngle:
    SourceStr = ">";
    break;
  case TokenType::LeftParen:
    SourceStr = "(";
    break;
  case TokenType::RightParen:
    SourceStr = ")";
    break;
  case TokenType::LeftBrace:
    SourceStr = "{";
    break;
  case TokenType::RightBrace:
    SourceStr = "}";
    break;
  case TokenType::Equal:
    SourceStr = "=";
    break;
  case TokenType::Semicolon:
    SourceStr = ";";
    break;
  case TokenType::Comma:
    SourceStr = ",";
    break;
  case TokenType::Eof:
    SourceStr = "";
    break;
  default:
    SourceStr = "";
    break;
  }
}

bool LexToken::operator==(const LexToken& other) const{
  return ((Type == other.Type) && (SourceStr.length() == other.SourceStr.length()) &&
	  (SourceStr.compare(other.SourceStr) == 0));
}

LexToken::LexToken(TokenType type) {
  commonInit(type);
}

LexToken::LexToken(TokenType type, std::string sourceStr) {
  commonInit(type);
  if (SourceStr == "") {
    SourceStr = sourceStr;
  }
}

Scanner* Scanner::Instance = nullptr;

Scanner& Scanner::getInstance() {
  if (nullptr == Instance) {
    Instance = new Scanner;
  }
  return *Instance;
}

void Scanner::initialzeTransitionTable() {
  Scanner scanner = Scanner::getInstance();
  std::map<Lexeme, LexState> state0Transitions = {
    {Lexeme::D, LexState::S1},           {Lexeme::E, LexState::S4},
    {Lexeme::F, LexState::S13},          {Lexeme::X, LexState::S13},
    {Lexeme::T, LexState::S13},          {Lexeme::R, LexState::S13},
    {Lexeme::N, LexState::S13},          {Lexeme::V, LexState::S10},
    {Lexeme::A, LexState::S13},          {Lexeme::OtherChar, LexState::S13},
    {Lexeme::Numeric, LexState::S14},    {Lexeme::Dot, LexState::SE},
    {Lexeme::LeftSquare, LexState::S16}, {Lexeme::RightSquare, LexState::S17},
    {Lexeme::LeftAngle, LexState::S18},  {Lexeme::RightAngle, LexState::S19},
    {Lexeme::LeftParen, LexState::S20},  {Lexeme::RightParen, LexState::S21},
    {Lexeme::LeftBrace, LexState::S22},  {Lexeme::RightBrace, LexState::S23},
    {Lexeme::Equal, LexState::S24},      {Lexeme::Operator, LexState::S25},
    {Lexeme::Semicolon, LexState::S26},  {Lexeme::Comma, LexState::S27},
    {Lexeme::Invalid, LexState::SE}};
  std::map<Lexeme, LexState> state1Transitions = {
    {Lexeme::D, LexState::S13},         {Lexeme::E, LexState::S2},
    {Lexeme::F, LexState::S13},         {Lexeme::X, LexState::S13},
    {Lexeme::T, LexState::S13},         {Lexeme::R, LexState::S13},
    {Lexeme::N, LexState::S13},         {Lexeme::V, LexState::S13},
    {Lexeme::A, LexState::S13},         {Lexeme::OtherChar, LexState::S13},
    {Lexeme::Numeric, LexState::S13},   {Lexeme::Dot, LexState::SE},
    {Lexeme::LeftSquare, LexState::SE}, {Lexeme::RightSquare, LexState::SE},
    {Lexeme::LeftAngle, LexState::SE},  {Lexeme::RightAngle, LexState::SE},
    {Lexeme::LeftParen, LexState::SE},  {Lexeme::RightParen, LexState::SE},
    {Lexeme::LeftBrace, LexState::SE},  {Lexeme::RightBrace, LexState::SE},
    {Lexeme::Equal, LexState::SE},      {Lexeme::Operator, LexState::SE},
    {Lexeme::Semicolon, LexState::SE},  {Lexeme::Comma, LexState::SE},
    {Lexeme::Invalid, LexState::SE}};
  std::map<Lexeme, LexState> state2Transitions = {
    {Lexeme::D, LexState::S13},         {Lexeme::E, LexState::S13},
    {Lexeme::F, LexState::S3},          {Lexeme::X, LexState::S13},
    {Lexeme::T, LexState::S13},         {Lexeme::R, LexState::S13},
    {Lexeme::N, LexState::S13},         {Lexeme::V, LexState::S13},
    {Lexeme::A, LexState::S13},         {Lexeme::OtherChar, LexState::S13},
    {Lexeme::Numeric, LexState::S13},   {Lexeme::Dot, LexState::SE},
    {Lexeme::LeftSquare, LexState::SE}, {Lexeme::RightSquare, LexState::SE},
    {Lexeme::LeftAngle, LexState::SE},  {Lexeme::RightAngle, LexState::SE},
    {Lexeme::LeftParen, LexState::SE},  {Lexeme::RightParen, LexState::SE},
    {Lexeme::LeftBrace, LexState::SE},  {Lexeme::RightBrace, LexState::SE},
    {Lexeme::Equal, LexState::SE},      {Lexeme::Operator, LexState::SE},
    {Lexeme::Semicolon, LexState::SE},  {Lexeme::Comma, LexState::SE},
    {Lexeme::Invalid, LexState::SE}};
  std::map<Lexeme, LexState> state3Transitions = {
    {Lexeme::D, LexState::S13},         {Lexeme::E, LexState::S13},
    {Lexeme::F, LexState::S13},         {Lexeme::X, LexState::S13},
    {Lexeme::T, LexState::S13},         {Lexeme::R, LexState::S13},
    {Lexeme::N, LexState::S13},         {Lexeme::V, LexState::S13},
    {Lexeme::A, LexState::S13},         {Lexeme::OtherChar, LexState::S13},
    {Lexeme::Numeric, LexState::S13},   {Lexeme::Dot, LexState::SE},
    {Lexeme::LeftSquare, LexState::SE}, {Lexeme::RightSquare, LexState::SE},
    {Lexeme::LeftAngle, LexState::SE},  {Lexeme::RightAngle, LexState::SE},
    {Lexeme::LeftParen, LexState::SE},  {Lexeme::RightParen, LexState::SE},
    {Lexeme::LeftBrace, LexState::SE},  {Lexeme::RightBrace, LexState::SE},
    {Lexeme::Equal, LexState::SE},      {Lexeme::Operator, LexState::SE},
    {Lexeme::Semicolon, LexState::SE},  {Lexeme::Comma, LexState::SE},
    {Lexeme::Invalid, LexState::SE}};
  std::map<Lexeme, LexState> state4Transitions = {
    {Lexeme::D, LexState::S13},         {Lexeme::E, LexState::S13},
    {Lexeme::F, LexState::S13},         {Lexeme::X, LexState::S13},
    {Lexeme::T, LexState::S5},          {Lexeme::R, LexState::S13},
    {Lexeme::N, LexState::S13},         {Lexeme::V, LexState::S13},
    {Lexeme::A, LexState::S13},         {Lexeme::OtherChar, LexState::S13},
    {Lexeme::Numeric, LexState::S13},   {Lexeme::Dot, LexState::SE},
    {Lexeme::LeftSquare, LexState::SE}, {Lexeme::RightSquare, LexState::SE},
    {Lexeme::LeftAngle, LexState::SE},  {Lexeme::RightAngle, LexState::SE},
    {Lexeme::LeftParen, LexState::SE},  {Lexeme::RightParen, LexState::SE},
    {Lexeme::LeftBrace, LexState::SE},  {Lexeme::RightBrace, LexState::SE},
    {Lexeme::Equal, LexState::SE},      {Lexeme::Operator, LexState::SE},
    {Lexeme::Semicolon, LexState::SE},  {Lexeme::Comma, LexState::SE},
    {Lexeme::Invalid, LexState::SE}};
  std::map<Lexeme, LexState> state5Transitions = {
    {Lexeme::D, LexState::S13},         {Lexeme::E, LexState::S13},
    {Lexeme::F, LexState::S13},         {Lexeme::X, LexState::S13},
    {Lexeme::T, LexState::S13},         {Lexeme::R, LexState::S6},
    {Lexeme::N, LexState::S13},         {Lexeme::V, LexState::S13},
    {Lexeme::A, LexState::S13},         {Lexeme::OtherChar, LexState::S13},
    {Lexeme::Numeric, LexState::S13},   {Lexeme::Dot, LexState::SE},
    {Lexeme::LeftSquare, LexState::SE}, {Lexeme::RightSquare, LexState::SE},
    {Lexeme::LeftAngle, LexState::SE},  {Lexeme::RightAngle, LexState::SE},
    {Lexeme::LeftParen, LexState::SE},  {Lexeme::RightParen, LexState::SE},
    {Lexeme::LeftBrace, LexState::SE},  {Lexeme::RightBrace, LexState::SE},
    {Lexeme::Equal, LexState::SE},      {Lexeme::Operator, LexState::SE},
    {Lexeme::Semicolon, LexState::SE},  {Lexeme::Comma, LexState::SE},
    {Lexeme::Invalid, LexState::SE}};
  std::map<Lexeme, LexState> state6Transitions = {
    {Lexeme::D, LexState::S13},         {Lexeme::E, LexState::S7},
    {Lexeme::F, LexState::S13},         {Lexeme::X, LexState::S13},
    {Lexeme::T, LexState::S13},         {Lexeme::R, LexState::S13},
    {Lexeme::N, LexState::S13},         {Lexeme::V, LexState::S13},
    {Lexeme::A, LexState::S13},         {Lexeme::OtherChar, LexState::S13},
    {Lexeme::Numeric, LexState::S13},   {Lexeme::Dot, LexState::SE},
    {Lexeme::LeftSquare, LexState::SE}, {Lexeme::RightSquare, LexState::SE},
    {Lexeme::LeftAngle, LexState::SE},  {Lexeme::RightAngle, LexState::SE},
    {Lexeme::LeftParen, LexState::SE},  {Lexeme::RightParen, LexState::SE},
    {Lexeme::LeftBrace, LexState::SE},  {Lexeme::RightBrace, LexState::SE},
    {Lexeme::Equal, LexState::SE},      {Lexeme::Operator, LexState::SE},
    {Lexeme::Semicolon, LexState::SE},  {Lexeme::Comma, LexState::SE},
    {Lexeme::Invalid, LexState::SE}};
  std::map<Lexeme, LexState> state7Transitions = {
    {Lexeme::D, LexState::S13},         {Lexeme::E, LexState::S13},
    {Lexeme::F, LexState::S13},         {Lexeme::X, LexState::S13},
    {Lexeme::T, LexState::S13},         {Lexeme::R, LexState::S8},
    {Lexeme::N, LexState::S13},         {Lexeme::V, LexState::S13},
    {Lexeme::A, LexState::S13},         {Lexeme::OtherChar, LexState::S13},
    {Lexeme::Numeric, LexState::S13},   {Lexeme::Dot, LexState::SE},
    {Lexeme::LeftSquare, LexState::SE}, {Lexeme::RightSquare, LexState::SE},
    {Lexeme::LeftAngle, LexState::SE},  {Lexeme::RightAngle, LexState::SE},
    {Lexeme::LeftParen, LexState::SE},  {Lexeme::RightParen, LexState::SE},
    {Lexeme::LeftBrace, LexState::SE},  {Lexeme::RightBrace, LexState::SE},
    {Lexeme::Equal, LexState::SE},      {Lexeme::Operator, LexState::SE},
    {Lexeme::Semicolon, LexState::SE},  {Lexeme::Comma, LexState::SE},
    {Lexeme::Invalid, LexState::SE}};
  std::map<Lexeme, LexState> state8Transitions = {
    {Lexeme::D, LexState::S13},         {Lexeme::E, LexState::S13},
    {Lexeme::F, LexState::S13},         {Lexeme::X, LexState::S13},
    {Lexeme::T, LexState::S13},         {Lexeme::R, LexState::S13},
    {Lexeme::N, LexState::S9},          {Lexeme::V, LexState::S13},
    {Lexeme::A, LexState::S13},         {Lexeme::OtherChar, LexState::S13},
    {Lexeme::Numeric, LexState::S13},   {Lexeme::Dot, LexState::SE},
    {Lexeme::LeftSquare, LexState::SE}, {Lexeme::RightSquare, LexState::SE},
    {Lexeme::LeftAngle, LexState::SE},  {Lexeme::RightAngle, LexState::SE},
    {Lexeme::LeftParen, LexState::SE},  {Lexeme::RightParen, LexState::SE},
    {Lexeme::LeftBrace, LexState::SE},  {Lexeme::RightBrace, LexState::SE},
    {Lexeme::Equal, LexState::SE},      {Lexeme::Operator, LexState::SE},
    {Lexeme::Semicolon, LexState::SE},  {Lexeme::Comma, LexState::SE},
    {Lexeme::Invalid, LexState::SE}};
  std::map<Lexeme, LexState> state9Transitions = {
    {Lexeme::D, LexState::S13},         {Lexeme::E, LexState::S3},
    {Lexeme::F, LexState::S13},         {Lexeme::X, LexState::S13},
    {Lexeme::T, LexState::S13},         {Lexeme::R, LexState::S13},
    {Lexeme::N, LexState::S13},         {Lexeme::V, LexState::S13},
    {Lexeme::A, LexState::S13},         {Lexeme::OtherChar, LexState::S13},
    {Lexeme::Numeric, LexState::S13},   {Lexeme::Dot, LexState::SE},
    {Lexeme::LeftSquare, LexState::SE}, {Lexeme::RightSquare, LexState::SE},
    {Lexeme::LeftAngle, LexState::SE},  {Lexeme::RightAngle, LexState::SE},
    {Lexeme::LeftParen, LexState::SE},  {Lexeme::RightParen, LexState::SE},
    {Lexeme::LeftBrace, LexState::SE},  {Lexeme::RightBrace, LexState::SE},
    {Lexeme::Equal, LexState::SE},      {Lexeme::Operator, LexState::SE},
    {Lexeme::Semicolon, LexState::SE},  {Lexeme::Comma, LexState::SE},
    {Lexeme::Invalid, LexState::SE}};
  std::map<Lexeme, LexState> state10Transitions = {
    {Lexeme::D, LexState::S13},         {Lexeme::E, LexState::S13},
    {Lexeme::F, LexState::S13},         {Lexeme::X, LexState::S13},
    {Lexeme::T, LexState::S13},         {Lexeme::R, LexState::S13},
    {Lexeme::N, LexState::S13},         {Lexeme::V, LexState::S13},
    {Lexeme::A, LexState::S11},         {Lexeme::OtherChar, LexState::S13},
    {Lexeme::Numeric, LexState::S13},   {Lexeme::Dot, LexState::SE},
    {Lexeme::LeftSquare, LexState::SE}, {Lexeme::RightSquare, LexState::SE},
    {Lexeme::LeftAngle, LexState::SE},  {Lexeme::RightAngle, LexState::SE},
    {Lexeme::LeftParen, LexState::SE},  {Lexeme::RightParen, LexState::SE},
    {Lexeme::LeftBrace, LexState::SE},  {Lexeme::RightBrace, LexState::SE},
    {Lexeme::Equal, LexState::SE},      {Lexeme::Operator, LexState::SE},
    {Lexeme::Semicolon, LexState::SE},  {Lexeme::Comma, LexState::SE},
    {Lexeme::Invalid, LexState::SE}};
  std::map<Lexeme, LexState> state11Transitions = {
    {Lexeme::D, LexState::S13},         {Lexeme::E, LexState::S13},
    {Lexeme::F, LexState::S13},         {Lexeme::X, LexState::S13},
    {Lexeme::T, LexState::S13},         {Lexeme::R, LexState::S12},
    {Lexeme::N, LexState::S13},         {Lexeme::V, LexState::S13},
    {Lexeme::A, LexState::S13},         {Lexeme::OtherChar, LexState::S13},
    {Lexeme::Numeric, LexState::S13},   {Lexeme::Dot, LexState::SE},
    {Lexeme::LeftSquare, LexState::SE}, {Lexeme::RightSquare, LexState::SE},
    {Lexeme::LeftAngle, LexState::SE},  {Lexeme::RightAngle, LexState::SE},
    {Lexeme::LeftParen, LexState::SE},  {Lexeme::RightParen, LexState::SE},
    {Lexeme::LeftBrace, LexState::SE},  {Lexeme::RightBrace, LexState::SE},
    {Lexeme::Equal, LexState::SE},      {Lexeme::Operator, LexState::SE},
    {Lexeme::Semicolon, LexState::SE},  {Lexeme::Comma, LexState::SE},
    {Lexeme::Invalid, LexState::SE}};
  std::map<Lexeme, LexState> state12Transitions = {
    {Lexeme::D, LexState::S13},         {Lexeme::E, LexState::S13},
    {Lexeme::F, LexState::S13},         {Lexeme::X, LexState::S13},
    {Lexeme::T, LexState::S13},         {Lexeme::R, LexState::S13},
    {Lexeme::N, LexState::S13},         {Lexeme::V, LexState::S13},
    {Lexeme::A, LexState::S13},         {Lexeme::OtherChar, LexState::S13},
    {Lexeme::Numeric, LexState::S13},   {Lexeme::Dot, LexState::SE},
    {Lexeme::LeftSquare, LexState::SE}, {Lexeme::RightSquare, LexState::SE},
    {Lexeme::LeftAngle, LexState::SE},  {Lexeme::RightAngle, LexState::SE},
    {Lexeme::LeftParen, LexState::SE},  {Lexeme::RightParen, LexState::SE},
    {Lexeme::LeftBrace, LexState::SE},  {Lexeme::RightBrace, LexState::SE},
    {Lexeme::Equal, LexState::SE},      {Lexeme::Operator, LexState::SE},
    {Lexeme::Semicolon, LexState::SE},  {Lexeme::Comma, LexState::SE},
    {Lexeme::Invalid, LexState::SE}};
  std::map<Lexeme, LexState> state13Transitions = {
    {Lexeme::D, LexState::S13},         {Lexeme::E, LexState::S13},
    {Lexeme::F, LexState::S13},         {Lexeme::X, LexState::S13},
    {Lexeme::T, LexState::S13},         {Lexeme::R, LexState::S13},
    {Lexeme::N, LexState::S13},         {Lexeme::V, LexState::S13},
    {Lexeme::A, LexState::S13},         {Lexeme::OtherChar, LexState::S13},
    {Lexeme::Numeric, LexState::S13},   {Lexeme::Dot, LexState::SE},
    {Lexeme::LeftSquare, LexState::SE}, {Lexeme::RightSquare, LexState::SE},
    {Lexeme::LeftAngle, LexState::SE},  {Lexeme::RightAngle, LexState::SE},
    {Lexeme::LeftParen, LexState::SE},  {Lexeme::RightParen, LexState::SE},
    {Lexeme::LeftBrace, LexState::SE},  {Lexeme::RightBrace, LexState::SE},
    {Lexeme::Equal, LexState::SE},      {Lexeme::Operator, LexState::SE},
    {Lexeme::Semicolon, LexState::SE},  {Lexeme::Comma, LexState::SE},
    {Lexeme::Invalid, LexState::SE}};
  std::map<Lexeme, LexState> state14Transitions = {
    {Lexeme::D, LexState::SE},          {Lexeme::E, LexState::SE},
    {Lexeme::F, LexState::SE},          {Lexeme::X, LexState::SE},
    {Lexeme::T, LexState::SE},          {Lexeme::R, LexState::SE},
    {Lexeme::N, LexState::SE},          {Lexeme::V, LexState::SE},
    {Lexeme::A, LexState::SE},          {Lexeme::OtherChar, LexState::SE},
    {Lexeme::Numeric, LexState::S14},   {Lexeme::Dot, LexState::S15},
    {Lexeme::LeftSquare, LexState::SE}, {Lexeme::RightSquare, LexState::SE},
    {Lexeme::LeftAngle, LexState::SE},  {Lexeme::RightAngle, LexState::SE},
    {Lexeme::LeftParen, LexState::SE},  {Lexeme::RightParen, LexState::SE},
    {Lexeme::LeftBrace, LexState::SE},  {Lexeme::RightBrace, LexState::SE},
    {Lexeme::Equal, LexState::SE},      {Lexeme::Operator, LexState::SE},
    {Lexeme::Semicolon, LexState::SE},  {Lexeme::Comma, LexState::SE},
    {Lexeme::Invalid, LexState::SE}};
  std::map<Lexeme, LexState> state15Transitions = {
    {Lexeme::D, LexState::SE},          {Lexeme::E, LexState::SE},
    {Lexeme::F, LexState::SE},          {Lexeme::X, LexState::SE},
    {Lexeme::T, LexState::SE},          {Lexeme::R, LexState::SE},
    {Lexeme::N, LexState::SE},          {Lexeme::V, LexState::SE},
    {Lexeme::A, LexState::SE},          {Lexeme::OtherChar, LexState::SE},
    {Lexeme::Numeric, LexState::S15},   {Lexeme::Dot, LexState::SE},
    {Lexeme::LeftSquare, LexState::SE}, {Lexeme::RightSquare, LexState::SE},
    {Lexeme::LeftAngle, LexState::SE},  {Lexeme::RightAngle, LexState::SE},
    {Lexeme::LeftParen, LexState::SE},  {Lexeme::RightParen, LexState::SE},
    {Lexeme::LeftBrace, LexState::SE},  {Lexeme::RightBrace, LexState::SE},
    {Lexeme::Equal, LexState::SE},      {Lexeme::Operator, LexState::SE},
    {Lexeme::Semicolon, LexState::SE},  {Lexeme::Comma, LexState::SE},
    {Lexeme::Invalid, LexState::SE}};
  std::map<Lexeme, LexState> state16Transitions = {
    {Lexeme::D, LexState::SE},          {Lexeme::E, LexState::SE},
    {Lexeme::F, LexState::SE},          {Lexeme::X, LexState::SE},
    {Lexeme::T, LexState::SE},          {Lexeme::R, LexState::SE},
    {Lexeme::N, LexState::SE},          {Lexeme::V, LexState::SE},
    {Lexeme::A, LexState::SE},          {Lexeme::OtherChar, LexState::SE},
    {Lexeme::Numeric, LexState::SE},    {Lexeme::Dot, LexState::SE},
    {Lexeme::LeftSquare, LexState::SE}, {Lexeme::RightSquare, LexState::SE},
    {Lexeme::LeftAngle, LexState::SE},  {Lexeme::RightAngle, LexState::SE},
    {Lexeme::LeftParen, LexState::SE},  {Lexeme::RightParen, LexState::SE},
    {Lexeme::LeftBrace, LexState::SE},  {Lexeme::RightBrace, LexState::SE},
    {Lexeme::Equal, LexState::SE},      {Lexeme::Operator, LexState::SE},
    {Lexeme::Semicolon, LexState::SE},  {Lexeme::Comma, LexState::SE},
    {Lexeme::Invalid, LexState::SE}};
  std::map<Lexeme, LexState> state17Transitions = {
    {Lexeme::D, LexState::SE},          {Lexeme::E, LexState::SE},
    {Lexeme::F, LexState::SE},          {Lexeme::X, LexState::SE},
    {Lexeme::T, LexState::SE},          {Lexeme::R, LexState::SE},
    {Lexeme::N, LexState::SE},          {Lexeme::V, LexState::SE},
    {Lexeme::A, LexState::SE},          {Lexeme::OtherChar, LexState::SE},
    {Lexeme::Numeric, LexState::SE},    {Lexeme::Dot, LexState::SE},
    {Lexeme::LeftSquare, LexState::SE}, {Lexeme::RightSquare, LexState::SE},
    {Lexeme::LeftAngle, LexState::SE},  {Lexeme::RightAngle, LexState::SE},
    {Lexeme::LeftParen, LexState::SE},  {Lexeme::RightParen, LexState::SE},
    {Lexeme::LeftBrace, LexState::SE},  {Lexeme::RightBrace, LexState::SE},
    {Lexeme::Equal, LexState::SE},      {Lexeme::Operator, LexState::SE},
    {Lexeme::Semicolon, LexState::SE},  {Lexeme::Comma, LexState::SE},
    {Lexeme::Invalid, LexState::SE}};
  std::map<Lexeme, LexState> state18Transitions = {
    {Lexeme::D, LexState::SE},          {Lexeme::E, LexState::SE},
    {Lexeme::F, LexState::SE},          {Lexeme::X, LexState::SE},
    {Lexeme::T, LexState::SE},          {Lexeme::R, LexState::SE},
    {Lexeme::N, LexState::SE},          {Lexeme::V, LexState::SE},
    {Lexeme::A, LexState::SE},          {Lexeme::OtherChar, LexState::SE},
    {Lexeme::Numeric, LexState::SE},    {Lexeme::Dot, LexState::SE},
    {Lexeme::LeftSquare, LexState::SE}, {Lexeme::RightSquare, LexState::SE},
    {Lexeme::LeftAngle, LexState::SE},  {Lexeme::RightAngle, LexState::SE},
    {Lexeme::LeftParen, LexState::SE},  {Lexeme::RightParen, LexState::SE},
    {Lexeme::LeftBrace, LexState::SE},  {Lexeme::RightBrace, LexState::SE},
    {Lexeme::Equal, LexState::SE},      {Lexeme::Operator, LexState::SE},
    {Lexeme::Semicolon, LexState::SE},  {Lexeme::Comma, LexState::SE},
    {Lexeme::Invalid, LexState::SE}};
  std::map<Lexeme, LexState> state19Transitions = {
    {Lexeme::D, LexState::SE},          {Lexeme::E, LexState::SE},
    {Lexeme::F, LexState::SE},          {Lexeme::X, LexState::SE},
    {Lexeme::T, LexState::SE},          {Lexeme::R, LexState::SE},
    {Lexeme::N, LexState::SE},          {Lexeme::V, LexState::SE},
    {Lexeme::A, LexState::SE},          {Lexeme::OtherChar, LexState::SE},
    {Lexeme::Numeric, LexState::SE},    {Lexeme::Dot, LexState::SE},
    {Lexeme::LeftSquare, LexState::SE}, {Lexeme::RightSquare, LexState::SE},
    {Lexeme::LeftAngle, LexState::SE},  {Lexeme::RightAngle, LexState::SE},
    {Lexeme::LeftParen, LexState::SE},  {Lexeme::RightParen, LexState::SE},
    {Lexeme::LeftBrace, LexState::SE},  {Lexeme::RightBrace, LexState::SE},
    {Lexeme::Equal, LexState::SE},      {Lexeme::Operator, LexState::SE},
    {Lexeme::Semicolon, LexState::SE},  {Lexeme::Comma, LexState::SE},
    {Lexeme::Invalid, LexState::SE}};
  std::map<Lexeme, LexState> state20Transitions = {
    {Lexeme::D, LexState::SE},          {Lexeme::E, LexState::SE},
    {Lexeme::F, LexState::SE},          {Lexeme::X, LexState::SE},
    {Lexeme::T, LexState::SE},          {Lexeme::R, LexState::SE},
    {Lexeme::N, LexState::SE},          {Lexeme::V, LexState::SE},
    {Lexeme::A, LexState::SE},          {Lexeme::OtherChar, LexState::SE},
    {Lexeme::Numeric, LexState::SE},    {Lexeme::Dot, LexState::SE},
    {Lexeme::LeftSquare, LexState::SE}, {Lexeme::RightSquare, LexState::SE},
    {Lexeme::LeftAngle, LexState::SE},  {Lexeme::RightAngle, LexState::SE},
    {Lexeme::LeftParen, LexState::SE},  {Lexeme::RightParen, LexState::SE},
    {Lexeme::LeftBrace, LexState::SE},  {Lexeme::RightBrace, LexState::SE},
    {Lexeme::Equal, LexState::SE},      {Lexeme::Operator, LexState::SE},
    {Lexeme::Semicolon, LexState::SE},  {Lexeme::Comma, LexState::SE},
    {Lexeme::Invalid, LexState::SE}};
  std::map<Lexeme, LexState> state21Transitions = {
    {Lexeme::D, LexState::SE},          {Lexeme::E, LexState::SE},
    {Lexeme::F, LexState::SE},          {Lexeme::X, LexState::SE},
    {Lexeme::T, LexState::SE},          {Lexeme::R, LexState::SE},
    {Lexeme::N, LexState::SE},          {Lexeme::V, LexState::SE},
    {Lexeme::A, LexState::SE},          {Lexeme::OtherChar, LexState::SE},
    {Lexeme::Numeric, LexState::SE},    {Lexeme::Dot, LexState::SE},
    {Lexeme::LeftSquare, LexState::SE}, {Lexeme::RightSquare, LexState::SE},
    {Lexeme::LeftAngle, LexState::SE},  {Lexeme::RightAngle, LexState::SE},
    {Lexeme::LeftParen, LexState::SE},  {Lexeme::RightParen, LexState::SE},
    {Lexeme::LeftBrace, LexState::SE},  {Lexeme::RightBrace, LexState::SE},
    {Lexeme::Equal, LexState::SE},      {Lexeme::Operator, LexState::SE},
    {Lexeme::Semicolon, LexState::SE},  {Lexeme::Comma, LexState::SE},
    {Lexeme::Invalid, LexState::SE}};
  std::map<Lexeme, LexState> state22Transitions = {
    {Lexeme::D, LexState::SE},          {Lexeme::E, LexState::SE},
    {Lexeme::F, LexState::SE},          {Lexeme::X, LexState::SE},
    {Lexeme::T, LexState::SE},          {Lexeme::R, LexState::SE},
    {Lexeme::N, LexState::SE},          {Lexeme::V, LexState::SE},
    {Lexeme::A, LexState::SE},          {Lexeme::OtherChar, LexState::SE},
    {Lexeme::Numeric, LexState::SE},    {Lexeme::Dot, LexState::SE},
    {Lexeme::LeftSquare, LexState::SE}, {Lexeme::RightSquare, LexState::SE},
    {Lexeme::LeftAngle, LexState::SE},  {Lexeme::RightAngle, LexState::SE},
    {Lexeme::LeftParen, LexState::SE},  {Lexeme::RightParen, LexState::SE},
    {Lexeme::LeftBrace, LexState::SE},  {Lexeme::RightBrace, LexState::SE},
    {Lexeme::Equal, LexState::SE},      {Lexeme::Operator, LexState::SE},
    {Lexeme::Semicolon, LexState::SE},  {Lexeme::Comma, LexState::SE},
    {Lexeme::Invalid, LexState::SE}};
  std::map<Lexeme, LexState> state23Transitions = {
    {Lexeme::D, LexState::SE},          {Lexeme::E, LexState::SE},
    {Lexeme::F, LexState::SE},          {Lexeme::X, LexState::SE},
    {Lexeme::T, LexState::SE},          {Lexeme::R, LexState::SE},
    {Lexeme::N, LexState::SE},          {Lexeme::V, LexState::SE},
    {Lexeme::A, LexState::SE},          {Lexeme::OtherChar, LexState::SE},
    {Lexeme::Numeric, LexState::SE},    {Lexeme::Dot, LexState::SE},
    {Lexeme::LeftSquare, LexState::SE}, {Lexeme::RightSquare, LexState::SE},
    {Lexeme::LeftAngle, LexState::SE},  {Lexeme::RightAngle, LexState::SE},
    {Lexeme::LeftParen, LexState::SE},  {Lexeme::RightParen, LexState::SE},
    {Lexeme::LeftBrace, LexState::SE},  {Lexeme::RightBrace, LexState::SE},
    {Lexeme::Equal, LexState::SE},      {Lexeme::Operator, LexState::SE},
    {Lexeme::Semicolon, LexState::SE},  {Lexeme::Comma, LexState::SE},
    {Lexeme::Invalid, LexState::SE}};
  std::map<Lexeme, LexState> state24Transitions = {
    {Lexeme::D, LexState::SE},          {Lexeme::E, LexState::SE},
    {Lexeme::F, LexState::SE},          {Lexeme::X, LexState::SE},
    {Lexeme::T, LexState::SE},          {Lexeme::R, LexState::SE},
    {Lexeme::N, LexState::SE},          {Lexeme::V, LexState::SE},
    {Lexeme::A, LexState::SE},          {Lexeme::OtherChar, LexState::SE},
    {Lexeme::Numeric, LexState::SE},    {Lexeme::Dot, LexState::SE},
    {Lexeme::LeftSquare, LexState::SE}, {Lexeme::RightSquare, LexState::SE},
    {Lexeme::LeftAngle, LexState::SE},  {Lexeme::RightAngle, LexState::SE},
    {Lexeme::LeftParen, LexState::SE},  {Lexeme::RightParen, LexState::SE},
    {Lexeme::Equal, LexState::SE},      {Lexeme::Operator, LexState::SE},
    {Lexeme::Semicolon, LexState::SE},  {Lexeme::Comma, LexState::SE},
    {Lexeme::Invalid, LexState::SE}};
  std::map<Lexeme, LexState> state25Transitions = {
    {Lexeme::D, LexState::SE},          {Lexeme::E, LexState::SE},
    {Lexeme::F, LexState::SE},          {Lexeme::X, LexState::SE},
    {Lexeme::T, LexState::SE},          {Lexeme::R, LexState::SE},
    {Lexeme::N, LexState::SE},          {Lexeme::V, LexState::SE},
    {Lexeme::A, LexState::SE},          {Lexeme::OtherChar, LexState::SE},
    {Lexeme::Numeric, LexState::SE},    {Lexeme::Dot, LexState::SE},
    {Lexeme::LeftSquare, LexState::SE}, {Lexeme::RightSquare, LexState::SE},
    {Lexeme::LeftAngle, LexState::SE},  {Lexeme::RightAngle, LexState::SE},
    {Lexeme::LeftParen, LexState::SE},  {Lexeme::RightParen, LexState::SE},
    {Lexeme::LeftBrace, LexState::SE},  {Lexeme::RightBrace, LexState::SE},
    {Lexeme::Equal, LexState::SE},      {Lexeme::Operator, LexState::SE},
    {Lexeme::Semicolon, LexState::SE},  {Lexeme::Comma, LexState::SE},
    {Lexeme::Invalid, LexState::SE}};
  std::map<Lexeme, LexState> state26Transitions = {
    {Lexeme::D, LexState::SE},          {Lexeme::E, LexState::SE},
    {Lexeme::F, LexState::SE},          {Lexeme::X, LexState::SE},
    {Lexeme::T, LexState::SE},          {Lexeme::R, LexState::SE},
    {Lexeme::N, LexState::SE},          {Lexeme::V, LexState::SE},
    {Lexeme::A, LexState::SE},          {Lexeme::OtherChar, LexState::SE},
    {Lexeme::Numeric, LexState::SE},    {Lexeme::Dot, LexState::SE},
    {Lexeme::LeftSquare, LexState::SE}, {Lexeme::RightSquare, LexState::SE},
    {Lexeme::LeftAngle, LexState::SE},  {Lexeme::RightAngle, LexState::SE},
    {Lexeme::LeftParen, LexState::SE},  {Lexeme::RightParen, LexState::SE},
    {Lexeme::LeftBrace, LexState::SE},  {Lexeme::RightBrace, LexState::SE},
    {Lexeme::Equal, LexState::SE},      {Lexeme::Operator, LexState::SE},
    {Lexeme::Semicolon, LexState::SE},  {Lexeme::Comma, LexState::SE},
    {Lexeme::Invalid, LexState::SE}};
  std::map<Lexeme, LexState> state27Transitions = {
    {Lexeme::D, LexState::SE},          {Lexeme::E, LexState::SE},
    {Lexeme::F, LexState::SE},          {Lexeme::X, LexState::SE},
    {Lexeme::T, LexState::SE},          {Lexeme::R, LexState::SE},
    {Lexeme::N, LexState::SE},          {Lexeme::V, LexState::SE},
    {Lexeme::A, LexState::SE},          {Lexeme::OtherChar, LexState::SE},
    {Lexeme::Numeric, LexState::SE},    {Lexeme::Dot, LexState::SE},
    {Lexeme::LeftSquare, LexState::SE}, {Lexeme::RightSquare, LexState::SE},
    {Lexeme::LeftAngle, LexState::SE},  {Lexeme::RightAngle, LexState::SE},
    {Lexeme::LeftParen, LexState::SE},  {Lexeme::RightParen, LexState::SE},
    {Lexeme::LeftBrace, LexState::SE},  {Lexeme::RightBrace, LexState::SE},
    {Lexeme::Equal, LexState::SE},      {Lexeme::Operator, LexState::SE},
    {Lexeme::Semicolon, LexState::SE},  {Lexeme::Comma, LexState::SE},
    {Lexeme::Invalid, LexState::SE}};
  std::map<Lexeme, LexState> stateETransitions = {
    {Lexeme::D, LexState::SE},          {Lexeme::E, LexState::SE},
    {Lexeme::F, LexState::SE},          {Lexeme::X, LexState::SE},
    {Lexeme::T, LexState::SE},          {Lexeme::R, LexState::SE},
    {Lexeme::N, LexState::SE},          {Lexeme::V, LexState::SE},
    {Lexeme::A, LexState::SE},          {Lexeme::OtherChar, LexState::SE},
    {Lexeme::Numeric, LexState::SE},    {Lexeme::Dot, LexState::SE},
    {Lexeme::LeftSquare, LexState::SE}, {Lexeme::RightSquare, LexState::SE},
    {Lexeme::LeftAngle, LexState::SE},  {Lexeme::RightAngle, LexState::SE},
    {Lexeme::LeftParen, LexState::SE},  {Lexeme::RightParen, LexState::SE},
    {Lexeme::Equal, LexState::SE},      {Lexeme::Operator, LexState::SE},
    {Lexeme::Semicolon, LexState::SE},  {Lexeme::Comma, LexState::SE},
    {Lexeme::Invalid, LexState::SE}};

  TransitionTable = {{LexState::S0, std::move(state0Transitions)},
		     {LexState::S1, std::move(state1Transitions)},
		     {LexState::S2, std::move(state2Transitions)},
		     {LexState::S3, std::move(state3Transitions)},
		     {LexState::S4, std::move(state4Transitions)},
		     {LexState::S5, std::move(state5Transitions)},
		     {LexState::S6, std::move(state6Transitions)},
		     {LexState::S7, std::move(state7Transitions)},
		     {LexState::S8, std::move(state8Transitions)},
		     {LexState::S9, std::move(state9Transitions)},
		     {LexState::S10, std::move(state10Transitions)},
		     {LexState::S11, std::move(state11Transitions)},
		     {LexState::S12, std::move(state12Transitions)},
		     {LexState::S13, std::move(state13Transitions)},
		     {LexState::S14, std::move(state14Transitions)},
		     {LexState::S15, std::move(state15Transitions)},
		     {LexState::S16, std::move(state16Transitions)},
		     {LexState::S17, std::move(state17Transitions)},
		     {LexState::S18, std::move(state18Transitions)},
		     {LexState::S19, std::move(state19Transitions)},
		     {LexState::S20, std::move(state20Transitions)},
		     {LexState::S21, std::move(state21Transitions)},
		     {LexState::S22, std::move(state22Transitions)},
		     {LexState::S23, std::move(state23Transitions)},
		     {LexState::S24, std::move(state24Transitions)},
		     {LexState::S25, std::move(state25Transitions)},
		     {LexState::S26, std::move(state26Transitions)},
		     {LexState::S27, std::move(state27Transitions)},
		     {LexState::SE, std::move(stateETransitions)}};
}

void Scanner::initializeStateTable() {
  TokenTypeTable = {
    {LexState::S0, TokenType::Invalid},     {LexState::S1, TokenType::Identifier},
    {LexState::S2, TokenType::Identifier},  {LexState::S3, TokenType::Def},
    {LexState::S4, TokenType::Identifier},  {LexState::S5, TokenType::Identifier},
    {LexState::S6, TokenType::Identifier},  {LexState::S7, TokenType::Identifier},
    {LexState::S8, TokenType::Identifier},  {LexState::S9, TokenType::Extern},
    {LexState::S10, TokenType::Identifier}, {LexState::S11, TokenType::Identifier},
    {LexState::S12, TokenType::Var},        {LexState::S13, TokenType::Identifier},
    {LexState::S14, TokenType::Number},     {LexState::S15, TokenType::Number},
    {LexState::S16, TokenType::LeftSquare}, {LexState::S17, TokenType::RightSquare},
    {LexState::S18, TokenType::LeftAngle},  {LexState::S19, TokenType::RightAngle},
    {LexState::S20, TokenType::LeftParen},  {LexState::S21, TokenType::RightParen},
    {LexState::S22, TokenType::LeftBrace},  {LexState::S23, TokenType::RightBrace},
    {LexState::S24, TokenType::Equal},      {LexState::S25, TokenType::Operator},
    {LexState::S26, TokenType::Semicolon},  {LexState::S27, TokenType::Comma},
    {LexState::SE, TokenType::Invalid}};
}

void Scanner::initializeLexemeTypeTable() {
  LexemeTypeTable = {
    {'d', Lexeme::D},          {'e', Lexeme::E},          {'f', Lexeme::F},
    {'x', Lexeme::X},          {'t', Lexeme::T},          {'r', Lexeme::R},
    {'n', Lexeme::N},          {'v', Lexeme::V},          {'a', Lexeme::A},
    {'.', Lexeme::Dot},        {'[', Lexeme::LeftSquare}, {']', Lexeme::RightSquare},
    {'<', Lexeme::LeftAngle},  {'>', Lexeme::RightAngle}, {'(', Lexeme::LeftParen},
    {')', Lexeme::RightParen}, {'{', Lexeme::LeftBrace},  {'}', Lexeme::RightBrace},
    {'=', Lexeme::Equal},      {'+', Lexeme::Operator},   {'-', Lexeme::Operator},
    {'*', Lexeme::Operator},   {'/', Lexeme::Operator},   {';', Lexeme::Semicolon},
    {',', Lexeme::Comma}};
}

Lexeme Scanner::getLexeme(char input) {
  if (LexemeTypeTable.find(input) != LexemeTypeTable.end()) {
    return LexemeTypeTable[input];
  }
  Lexeme type;
  if (isalpha((int)input)) {
    type = Lexeme::OtherChar;
  } else if (isdigit((int)input)) {
    type = Lexeme::Numeric;
  } else {
    type = Lexeme::Invalid;
  }
  return type;
}

std::vector<LexToken> Scanner::scan(std::string inputBuffer) {
  Scanner scanner = Scanner::getInstance();
  scanner.initialzeTransitionTable();
  scanner.initializeStateTable();
  scanner.initializeLexemeTypeTable();

  LexState currState = LexState::S0;
  char currChar;
  std::string lexeme;
  size_t bufferPos = 0;
  std::stack<LexState> stateStack;
  std::vector<LexToken> tokens;
  bool inComment = false;
  while (bufferPos < inputBuffer.length()) {
    currChar = inputBuffer[bufferPos];
    if (currChar == '\f' || currChar == '\n' || currChar == '\r') {
      inComment = false;
      bufferPos += 1;
    } else if (currChar == '#') {
      inComment = true;
      bufferPos += 1;

    } else if (isspace(currChar)) {
      bufferPos += 1;
    } else if (inComment) {
      bufferPos += 1;
    } else {
      lexeme = "";
      currState = LexState::S0;
      clearAndPush(stateStack, std::move(LexState::SE));
      while (currState != LexState::SE) {
	lexeme += currChar;
	if (scanner.TokenTypeTable[currState] != TokenType::Invalid) {
	  clearAndPush(stateStack, std::move(LexState::SE));
	}
	stateStack.push(currState);
	Lexeme type = scanner.getLexeme(currChar);
	currState = scanner.TransitionTable[currState][type];
	bufferPos += 1;
	currChar = inputBuffer[bufferPos];
      }
      while (scanner.TokenTypeTable[currState] == TokenType::Invalid) {
	currState = stateStack.top();
	stateStack.pop();
	if (!stateStack.empty()) {
	  lexeme = lexeme.substr(0, lexeme.length() - 1);
	  bufferPos = bufferPos - 1;
	} else {
	  break;
	}
      }
      if (scanner.TokenTypeTable[currState] != TokenType::Invalid) {
	LexToken token(scanner.TokenTypeTable[currState], lexeme);
	tokens.push_back(token);
      } else {
	std::stringstream diag;
	diag << "Invalid lexeme";
	diag << lexeme;
	throw std::runtime_error(diag.str());
      }
    }
  }
  tokens.push_back(LexToken(TokenType::Eof));
  return tokens;
}
