#ifndef LEXER_HPP
#define LEXER_HPP

#include <iostream>
#include <vector>
#include <string>


struct CommentRule {
    std::string start_marker;
    std::string end_marker;
    CommentRule(std::string start, std::string end);
};

template <typename TEnum>
struct Token {
    TEnum type;
    std::string value;
    Token(TEnum t, std::string v);
};


template <typename TEnum>
class LexerConfig {
public:
    std::vector<CommentRule>    active_comment_rules;
    std::vector<TEnum>          separators_table;
    std::string                  withSpace;
    TEnum                        default_word_type;
    TEnum                        default_eof_type;
    TEnum                        null_type;

private:
    LexerConfig(); 

public:
    LexerConfig(TEnum word_id, TEnum eof_id, TEnum null_val);
    void addSeparatorToken(unsigned char separator, TEnum type);
    void addCommentRule(std::string start, std::string end);
    void addWithSpace(std::string WithSpace);
};


template <typename TEnum>
class GenericLexer {
private:
    LexerConfig<TEnum> _config;
    std::string _src;
    size_t _cursor;

    GenericLexer();

    bool isSeparator(char c);
    bool isQuote(char c);
    bool startsWith(const std::string& pattern);
    bool isCommentStart();
    void handleQuote(char quoteType, std::vector< Token<TEnum> >& tokens);
    bool skipComment();

public:
    GenericLexer(std::string source, const LexerConfig<TEnum>& cfg);
    LexerConfig<TEnum>  getConfig();
    std::vector< Token<TEnum> > tokenize();
};


enum TokenType {
    TOKEN_NULL,
    TOKEN_WORD,
    TOKEN_LBRACE,
    TOKEN_RBRACE,
    TOKEN_SEMICOLON,
    TOKEN_FASSILA,
    TOKEN_JOUJNO9ATE,
    TOKEN_EOF
};

std::string tokenTypeToString(TokenType t);

template <typename TEnum>
class clsParse {
    int _cursor;
    std::vector< Token<TEnum> > _Tokens;
    TEnum _eofType;
public:
    clsParse(std::vector< Token<TEnum> > &Tokens, TEnum eofType);
    ~clsParse();
    Token<TEnum> peek();
    Token<TEnum> advance();
};

#endif