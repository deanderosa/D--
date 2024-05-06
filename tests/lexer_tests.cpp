#include <gtest/gtest.h>
#include "lexer.h"
#include "token_constraint.h"

TEST(LexerTests, TestValidBuffer) {
  std::string inputBuffer = R"(
def main() {
    # Define a variable `a` with shape <2, 3>, initialized with the literal value.
    # The shape is inferred from the supplied literal.
    var a = [[1, 2, 3], [4, 5, 6]];

    # b is identical to a, the literal tensor is implicitly reshaped: defining new
    # variables is the way to reshape tensors (element count must match).
    var b<2, 3> = [1, 2, 3, 4, 5, 6];

    # transpose() and print() are the only builtin, the following will transpose
    # a and b and perform an element-wise multiplication before printing the result.
    print(transpose(a) * transpose(b));
};
)";
  std::vector<LexToken> expectedOutput = {{LexToken(TokenType::Def)},
					  {LexToken(TokenType::Identifier, "main")},
					  {LexToken(TokenType::LeftParen)},
					  {LexToken(TokenType::RightParen)},
					  {LexToken(TokenType::LeftBrace)},
					  {LexToken(TokenType::Var)},
					  {LexToken(TokenType::Identifier, "a")},
					  {LexToken(TokenType::Equal)},
					  {LexToken(TokenType::LeftSquare)},
					  {LexToken(TokenType::LeftSquare)},
					  {LexToken(TokenType::Number, "1")},
					  {LexToken(TokenType::Comma)},
					  {LexToken(TokenType::Number, "2")},
					  {LexToken(TokenType::Comma)},
					  {LexToken(TokenType::Number, "3")},
					  {LexToken(TokenType::RightSquare)},
					  {LexToken(TokenType::Comma)},
					  {LexToken(TokenType::LeftSquare)},
					  {LexToken(TokenType::Number, "4")},
					  {LexToken(TokenType::Comma)},
					  {LexToken(TokenType::Number, "5")},
					  {LexToken(TokenType::Comma)},
					  {LexToken(TokenType::Number, "6")},
					  {LexToken(TokenType::RightSquare)},
					  {LexToken(TokenType::RightSquare)},
					  {LexToken(TokenType::Semicolon)},
					  {LexToken(TokenType::Var)},
					  {LexToken(TokenType::Identifier, "b")},
					  {LexToken(TokenType::LeftAngle)},
					  {LexToken(TokenType::Number, "2")},
					  {LexToken(TokenType::Comma)},
					  {LexToken(TokenType::Number, "3")},
					  {LexToken(TokenType::RightAngle)},
					  {LexToken(TokenType::Equal)},
					  {LexToken(TokenType::LeftSquare)},
					  {LexToken(TokenType::Number, "1")},
					  {LexToken(TokenType::Comma)},
					  {LexToken(TokenType::Number, "2")},
					  {LexToken(TokenType::Comma)},
					  {LexToken(TokenType::Number, "3")},
					  {LexToken(TokenType::Comma)},
					  {LexToken(TokenType::Number, "4")},
					  {LexToken(TokenType::Comma)},
					  {LexToken(TokenType::Number, "5")},
					  {LexToken(TokenType::Comma)},
					  {LexToken(TokenType::Number, "6")},
					  {LexToken(TokenType::RightSquare)},
					  {LexToken(TokenType::Semicolon)},
					  {LexToken(TokenType::Identifier, "print")},
					  {LexToken(TokenType::LeftParen)},
					  {LexToken(TokenType::Identifier, "transpose")},
					  {LexToken(TokenType::LeftParen)},
					  {LexToken(TokenType::Identifier, "a")},
					  {LexToken(TokenType::RightParen)},
					  {LexToken(TokenType::Operator, "*")},
					  {LexToken(TokenType::Identifier, "transpose")},
					  {LexToken(TokenType::LeftParen)},
					  {LexToken(TokenType::Identifier, "b")},
					  {LexToken(TokenType::RightParen)},
					  {LexToken(TokenType::RightParen)},
					  {LexToken(TokenType::Semicolon)},
					  {LexToken(TokenType::RightBrace)},
					  {LexToken(TokenType::Semicolon)},
					  {LexToken(TokenType::Eof)}};

  std::vector<LexToken> actualOutput = Scanner::scan(inputBuffer);
  ASSERT_TRUE(TokenConstraint::SatisfiedBy(actualOutput, expectedOutput));
}


TEST(LexerTests, TestInvalidLexeme) {
  std::string inputBuffer = R"(
def main() {
    !Foreign character!;
};
)";
  ASSERT_THROW(Scanner::scan(inputBuffer), std::runtime_error);
}
