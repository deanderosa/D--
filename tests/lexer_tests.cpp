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
    std::vector<LexToken> expectedOutput = {{TokenType::Def, "def"},
                                            {TokenType::Identifier, "main"},
                                            {TokenType::LeftParen, "("},
                                            {TokenType::RightParen, ")"},
                                            {TokenType::LeftBrace, "{"},
                                            {TokenType::Var, "var"},
                                            {TokenType::Identifier, "a"},
                                            {TokenType::Equal, "="},
                                            {TokenType::LeftSquare, "["},
                                            {TokenType::LeftSquare, "["},
                                            {TokenType::Number, "1"},
                                            {TokenType::Comma, ","},
                                            {TokenType::Number, "2"},
                                            {TokenType::Comma, ","},
                                            {TokenType::Number, "3"},
                                            {TokenType::RightSquare, "]"},
                                            {TokenType::Comma, ","},
                                            {TokenType::LeftSquare, "["},
                                            {TokenType::Number, "4"},
                                            {TokenType::Comma, ","},
                                            {TokenType::Number, "5"},
                                            {TokenType::Comma, ","},
                                            {TokenType::Number, "6"},
                                            {TokenType::RightSquare, "]"},
                                            {TokenType::RightSquare, "]"},
                                            {TokenType::Semicolon, ";"},
                                            {TokenType::Var, "var"},
                                            {TokenType::Identifier, "b"},
                                            {TokenType::LeftAngle, "<"},
                                            {TokenType::Number, "2"},
                                            {TokenType::Comma, ","},
                                            {TokenType::Number, "3"},
                                            {TokenType::RightAngle, ">"},
                                            {TokenType::Equal, "="},
                                            {TokenType::LeftSquare, "["},
                                            {TokenType::Number, "1"},
                                            {TokenType::Comma, ","},
                                            {TokenType::Number, "2"},
                                            {TokenType::Comma, ","},
                                            {TokenType::Number, "3"},
                                            {TokenType::Comma, ","},
                                            {TokenType::Number, "4"},
                                            {TokenType::Comma, ","},
                                            {TokenType::Number, "5"},
                                            {TokenType::Comma, ","},
                                            {TokenType::Number, "6"},
                                            {TokenType::RightSquare, "]"},
                                            {TokenType::Semicolon, ";"},
                                            {TokenType::Identifier, "print"},
                                            {TokenType::LeftParen, "("},
                                            {TokenType::Identifier, "transpose"},
                                            {TokenType::LeftParen, "("},
                                            {TokenType::Identifier, "a"},
                                            {TokenType::RightParen, ")"},
                                            {TokenType::Times, "*"},
                                            {TokenType::Identifier, "transpose"},
                                            {TokenType::LeftParen, "("},
                                            {TokenType::Identifier, "b"},
                                            {TokenType::RightParen, ")"},
                                            {TokenType::RightParen, ")"},
                                            {TokenType::Semicolon, ";"},
                                            {TokenType::RightBrace, "}"},
                                            {TokenType::Semicolon, ";"}};

    std::vector<LexToken> actualOutput = Scanner::Scan(inputBuffer);
    ASSERT_TRUE(TokenConstraint::SatisfiedBy(actualOutput, expectedOutput));
}


TEST(LexerTests, TestInvalidLexeme) {
    std::string inputBuffer = R"(
def main() {
    !Foreign character!;
};
)";
    ASSERT_THROW(Scanner::Scan(inputBuffer), std::runtime_error);
}
