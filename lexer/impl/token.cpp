#include "token.hpp"

namespace lexer {

std::string to_string(TokenType type) {
    switch (type) {
    case TokenType::Illegal:
        return "illegal token";
    case TokenType::Eof:
        return "end of file";
    case TokenType::Comment:
        return "comment";
    case TokenType::Identifier:
        return "identifier";
    case TokenType::Int:
        return "integer literal";
    case TokenType::Real:
        return "real literal";
    case TokenType::Less:
        return "'<'";
    case TokenType::Greater:
        return "'>'";
    case TokenType::Eq:
        return "'='";
    case TokenType::Leq:
        return "'<='";
    case TokenType::Geq:
        return "'>='";
    case TokenType::Neq:
        return "'/='";
    case TokenType::Assign:
        return "':='";
    case TokenType::Add:
        return "'+'";
    case TokenType::Sub:
        return "'-'";
    case TokenType::Mul:
        return "'*'";
    case TokenType::Div:
        return "'/'";
    case TokenType::Mod:
        return "'%'";
    case TokenType::OpenParen:
        return "'('";
    case TokenType::OpenBrack:
        return "'['";
    case TokenType::Comma:
        return "','";
    case TokenType::Dot:
        return "'.'";
    case TokenType::TwoDots:
        return "'..'";
    case TokenType::CloseParen:
        return "')'";
    case TokenType::CloseBrack:
        return "']'";
    case TokenType::Semicolon:
        return "semicolon";
    case TokenType::Colon:
        return "':'";
    case TokenType::NewLine:
        return "new line";
    case TokenType::Var:
        return "'var'";
    case TokenType::Type:
        return "'type'";
    case TokenType::Routine:
        return "'routine'";
    case TokenType::Is:
        return "'is'";
    case TokenType::IntegerType:
        return "'integer'";
    case TokenType::RealType:
        return "'real'";
    case TokenType::Boolean:
        return "'boolean'";
    case TokenType::Record:
        return "'record'";
    case TokenType::Array:
        return "'array'";
    case TokenType::True:
        return "'true'";
    case TokenType::False:
        return "'false'";
    case TokenType::While:
        return "'while'";
    case TokenType::For:
        return "'for'";
    case TokenType::Loop:
        return "'loop'";
    case TokenType::End:
        return "'end'";
    case TokenType::Reverse:
        return "'reverse'";
    case TokenType::In:
        return "'in'";
    case TokenType::If:
        return "'if'";
    case TokenType::Then:
        return "'then'";
    case TokenType::Else:
        return "'else'";
    case TokenType::Not:
        return "'not'";
    case TokenType::And:
        return "'and'";
    case TokenType::Or:
        return "'or'";
    case TokenType::Xor:
        return "'xor'";
    case TokenType::Return:
        return "'return'";
    }

    return "<internal error: unknown token>";
}

} // namespace lexer
