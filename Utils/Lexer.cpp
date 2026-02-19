#include "Lexer.hpp"

CommentRule::CommentRule(std::string start, std::string end) 
    : start_marker(start), end_marker(end) {}

template <typename TEnum>
Token<TEnum>::Token(TEnum t, std::string v) : type(t), value(v) {}

template <typename TEnum>
LexerConfig<TEnum>::LexerConfig() {}

template <typename TEnum>
LexerConfig<TEnum>::LexerConfig(TEnum word_id, TEnum eof_id, TEnum null_val)
    : default_word_type(word_id), default_eof_type(eof_id), null_type(null_val) {
    separators_table.assign(256, null_type);
}

template <typename TEnum>
void LexerConfig<TEnum>::addSeparatorToken(unsigned char separator, TEnum type) {
    separators_table[separator] = type;
}

template <typename TEnum>
void LexerConfig<TEnum>::addCommentRule(std::string start, std::string end) {
    active_comment_rules.push_back(CommentRule(start, end));
}

template <typename TEnum>
void LexerConfig<TEnum>::addWithSpace(std::string WithSpace) {
    withSpace = WithSpace;
}

template <typename TEnum>
GenericLexer<TEnum>::GenericLexer(std::string source, const LexerConfig<TEnum>& cfg)
    : _config(cfg), _src(source), _cursor(0) {}

template <typename TEnum>
bool GenericLexer<TEnum>::isSeparator(char c) {
    return _config.separators_table[static_cast<unsigned char>(c)] != _config.null_type;
}

template <typename TEnum>
bool GenericLexer<TEnum>::isQuote(char c) {
    return (c == '"' || c == '\'');
}

template <typename TEnum>
bool GenericLexer<TEnum>::startsWith(const std::string& pattern) {
    if (_src.length() - _cursor < pattern.length()) return false;
    return _src.compare(_cursor, pattern.length(), pattern) == 0;
}

template <typename TEnum>
bool GenericLexer<TEnum>::isCommentStart() {
    for (size_t i = 0; i < _config.active_comment_rules.size(); ++i) {
        if (startsWith(_config.active_comment_rules[i].start_marker)) return true;
    }
    return false;
}

template <typename TEnum>
void GenericLexer<TEnum>::handleQuote(char quoteType, std::vector<Token<TEnum> >& tokens) {
    std::string wordVal = "";
    _cursor++;
    while (_cursor < _src.length() && _src[_cursor] != quoteType) {
        wordVal += _src[_cursor++];
    }
    if (_cursor < _src.length()) _cursor++;
    tokens.push_back(Token<TEnum>(_config.default_word_type, wordVal));
}

template <typename TEnum>
bool GenericLexer<TEnum>::skipComment() {
    for (size_t i = 0; i < _config.active_comment_rules.size(); ++i) {
        std::string start = _config.active_comment_rules[i].start_marker;
        std::string end = _config.active_comment_rules[i].end_marker;
        if (startsWith(start)) {
            size_t endPos = _src.find(end, _cursor + start.length());
            _cursor = (endPos == std::string::npos) ? _src.length() : endPos + end.length();
            return true;
        }
    }
    return false;
}

template <typename TEnum>
std::vector<Token<TEnum> > GenericLexer<TEnum>::tokenize() {
    std::vector<Token<TEnum> > tokens;
    while (_cursor < _src.length()) {
        char c = _src[_cursor];
        if (_config.withSpace.find(c) != std::string::npos) { _cursor++; continue; }
        if (isQuote(c)) { handleQuote(c, tokens); continue; }
        if (skipComment()) continue;
        if (_config.separators_table[static_cast<unsigned char>(c)] != _config.null_type) {
            tokens.push_back(Token<TEnum>(_config.separators_table[static_cast<unsigned char>(c)], std::string(1, c)));
            _cursor++; continue;
        }
        std::string word = "";
        while (_cursor < _src.length()) {
            char current = _src[_cursor];
            if (_config.withSpace.find(current) != std::string::npos || isSeparator(current) || isQuote(current) || isCommentStart()) break;
            word += _src[_cursor++];
        }
        if (!word.empty()) tokens.push_back(Token<TEnum>(_config.default_word_type, word));
    }
    tokens.push_back(Token<TEnum>(_config.default_eof_type, ""));
    return tokens;
}

template <typename TEnum>
LexerConfig<TEnum>  GenericLexer<TEnum>::getConfig()
{
    return (_config);
}


template <typename TEnum>
clsParse<TEnum>::clsParse(std::vector< Token<TEnum> > &Tokens, TEnum eofType) 
    : _cursor(0), _Tokens(Tokens), _eofType(eofType) {}

template <typename TEnum>
clsParse<TEnum>::~clsParse() {}

template <typename TEnum>
Token<TEnum> clsParse<TEnum>::peek() {
    if (_cursor >= _Tokens.size()) return Token<TEnum>(_eofType, "");
    return _Tokens[_cursor];
}

template <typename TEnum>
Token<TEnum> clsParse<TEnum>::advance() {
    if (_cursor + 1 >= _Tokens.size()) return Token<TEnum>(_eofType, "");
    return _Tokens[++_cursor];
}

std::string tokenTypeToString(TokenType t) {
    switch (t) {
        case TOKEN_WORD: return "WORD"; case TOKEN_LBRACE: return "LBRACE";
        case TOKEN_RBRACE: return "RBRACE"; case TOKEN_SEMICOLON: return "SEMICOLON";
        case TOKEN_FASSILA: return "TOKEN_FASSILA"; case TOKEN_JOUJNO9ATE: return "TOKEN_JOUJNO9ATE";
        case TOKEN_EOF: return "EOF"; default: return "UNKNOWN";
    }
}

template struct Token<TokenType>;
template class LexerConfig<TokenType>;
template class GenericLexer<TokenType>;
template class clsParse<TokenType>;