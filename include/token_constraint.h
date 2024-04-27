#include <vector>
#include <array>
#include "lexer.h"

class TokenConstraint {
  public:
    static bool SatisfiedBy(std::vector<LexToken>, std::vector<LexToken>);
};

bool TokenConstraint::SatisfiedBy(std::vector<LexToken> Actual, std::vector<LexToken> Expected) {
    bool tf = Actual.size() == Expected.size();
    if (!tf)
        return tf;
    for (size_t i = 0; i < Actual.size(); i++) {
        tf = tf && (Actual[i] == Expected[i]);
    }
    return tf;
}
