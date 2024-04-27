#ifndef LEXER_H_
#define LEXER_H_

#include <map>
#include <vector>
#include <stack>
#include <string>

typedef enum class Lexeme {
    D,
    E,
    F,
    X,
    T,
    R,
    N,
    V,
    A,
    OtherChar,
    Numeric,
    Dot,
    LeftSquare,
    RightSquare,
    LeftAngle,
    RightAngle,
    LeftParen,
    RightParen,
    LeftBrace,
    RightBrace,
    Equal,
    Plus,
    Minus,
    Times,
    Divide,
    Semicolon,
    Comma,
    Invalid
} Lexeme;

typedef enum class TokenType {
    Def = -2,
    Extern = -3,
    Var = -4,
    Identifier = -5,
    Number = -6,
    LeftSquare = -7,
    RightSquare = -8,
    LeftAngle = -9,
    RightAngle = -10,
    LeftParen = -11,
    RightParen = -12,
    LeftBrace = -13,
    RightBrace = -14,    
    Equal = -15,
    Plus = -16,
    Minus = -17,
    Times = -18,
    Divide = -19,
    Semicolon = -20,
    Comma = -21,
    Invalid = -22
} TokenType;

typedef enum class LexState {
    S0 = -1,
    S1 = -2,
    S2 = -3,
    S3 = 1,
    S4 = -4,
    S5 = 2,
    S6 = -5,
    S7 = -6,
    S8 = -7,
    S9 = 2,
    S10 = -8,
    S11 = -9,
    S12 = 3,
    S13 = 4,
    S14 = 5,
    S15 = 6,
    S16 = 7,
    S17 = 8,
    S18 = 9,
    S19 = 10,
    S20 = 11,
    S21 = 22,
    S22 = 13,
    S23 = 14,
    S24 = 15,
    S25 = 16,
    S26 = 17,
    S27 = 18,
    S28 = 19,
    S29 = 20,
    S30 = 21,
    SE = -10
} LexState;

typedef struct LexToken {
    TokenType Type;
    std::string SourceStr;

    bool operator==(const LexToken& other) const {
        return ((Type == other.Type) && (SourceStr.length() == other.SourceStr.length()) &&
                (SourceStr.compare(other.SourceStr) == 0));
    }
} LexToken;

class Scanner {
  private:
    static Scanner* Instance;
    std::map<LexState, std::map<Lexeme, LexState>> TransitionTable;
    std::map<LexState, TokenType> TokenTypeTable;
    std::map<char, Lexeme> LexemeTypeTable;
    Lexeme getLexeme(char);
    void InitialzeTransitionTable();
    void InitializeStateTable();
    void InitializeLexemeTypeTable();
    static Scanner& GetInstance();
    Scanner() = default;

  public:
    static std::vector<LexToken> Scan(std::string);
};

#endif
