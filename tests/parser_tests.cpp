#include <gtest/gtest.h>
#include "lexer.h"
#include "parser.h"
#include "utils.h"

TEST(ParserTests, TestValidBuffer) {
  std::string inputBuffer = R"(
def main() {
    # Define a variable `a` with shape <2, 3>, initialized with the literal value.
    # The shape is inferred from the supplied literal.
    var a = [[1, 2, 3], [4, 5, 6]];
b
    # b is identical to a, the literal tensor is implicitly reshaped: defining new
    # variables is the way to reshape tensors (element count must match).
    var b<2, 3> = [1, 2, 3, 4, 5, 6];

    # transpose() and print() are the only builtin, the following will transpose
    # a and b and perform an element-wise multiplication before printing the result.
    print(transpose(a) * transpose(b));
};
)";
  auto  arrA1 = ArrayExprNode(make_vector<ExprNode>(std::make_unique<NumberExprNode>(NumberExprNode(1)),
						    std::make_unique<NumberExprNode>(NumberExprNode(2)),
						    std::make_unique<NumberExprNode>(NumberExprNode(3))));

  auto  arrA2 = ArrayExprNode(make_vector<ExprNode>(std::make_unique<NumberExprNode>(NumberExprNode(4)),
						    std::make_unique<NumberExprNode>(NumberExprNode(5)),
						    std::make_unique<NumberExprNode>(NumberExprNode(6))));

  auto arrA = ArrayExprNode(make_vector<ExprNode>(std::make_unique<ArrayExprNode>(std::move(arrA1)), std::make_unique<ArrayExprNode>(std::move(arrA2))));
  auto sizeA = make_vector<NumberExprNode>(std::make_unique<NumberExprNode>(1));

  AssgnNode assnA = AssgnNode("a", std::move(sizeA), std::make_unique<ArrayExprNode>(std::move(arrA)), true);

  auto arrB = ArrayExprNode(make_vector<ExprNode>(std::make_unique<NumberExprNode>(NumberExprNode(1)),
						  std::make_unique<NumberExprNode>(NumberExprNode(2)),
						  std::make_unique<NumberExprNode>(NumberExprNode(3)),
						  std::make_unique<NumberExprNode>(NumberExprNode(4)),
						  std::make_unique<NumberExprNode>(NumberExprNode(5)),
						  std::make_unique<NumberExprNode>(NumberExprNode(6))));
  auto sizeB = make_vector<NumberExprNode>(std::make_unique<NumberExprNode>(2), std::make_unique<NumberExprNode>(3));
  AssgnNode assnB = AssgnNode("b", std::move(sizeB), std::make_unique<ArrayExprNode>(std::move(arrB)), true);

  VariableExprNode transposeA = VariableExprNode("transpose",
						 make_vector<ExprNode>(std::make_unique<VariableExprNode>("a", std::vector<std::unique_ptr<ExprNode>>{})));
  VariableExprNode transposeB = VariableExprNode("transpose",
						 make_vector<ExprNode>(std::make_unique<VariableExprNode>("b", std::vector<std::unique_ptr<ExprNode>>{})));

  BinaryExprNode mult = BinaryExprNode(Op::Times,
				       std::make_unique<VariableExprNode>(std::move(transposeA)),
				       std::make_unique<VariableExprNode>(std::move(transposeB)));
  VariableExprNode print = VariableExprNode("print", make_vector<ExprNode>(std::make_unique<BinaryExprNode>(std::move(mult))));

  std::vector<std::unique_ptr<StmtNode>> mainBody = make_vector<StmtNode>(std::make_unique<AssgnNode>(std::move(assnA)),
									  std::make_unique<AssgnNode>(std::move(assnB)), std::make_unique<VariableExprNode>(std::move(print)));

  PrototypeNode mainProto = PrototypeNode("main", (std::vector<std::string>){});
  FunctionNode mainFunc = FunctionNode(std::make_unique<PrototypeNode>(std::move(mainProto)), std::move(mainBody));

  FunctionNode expectedAST = std::move(mainFunc);

  std::vector<LexToken> actualTokens = Scanner::scan(inputBuffer);
  auto  actualAstVec = Parser::parse(actualTokens);
  FunctionNode actualAST = dynamic_cast<FunctionNode&&>(*(actualAstVec[0]));

  ASSERT_TRUE(expectedAST == actualAST);
}
