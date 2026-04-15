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


private:
	std::vector<CommentRule>	active_comment_rules;
	std::vector<TEnum>			separators_table;
	std::string					withSpace;
	TEnum						default_word_type;
	TEnum						default_eof_type;
	TEnum						null_type;

	LexerConfig(); 

public:
	LexerConfig(TEnum word_id, TEnum eof_id, TEnum null_val);
	void addSeparatorToken(unsigned char separator, TEnum type);
	void addCommentRule(std::string start, std::string end);
	void addWithSpace(std::string WithSpace);

	const std::vector<CommentRule>&	getActiveCommentRules()	const;
	const std::vector<TEnum>&		getSeparatorsTable()	const;
	const std::string&				getWithSpace()			const;
	TEnum							getDefaultWordType()	const;
	TEnum							getDefaultEofType()		const;
	TEnum							getNullType()			const;
};


template <typename TEnum>
class GenericLexer {
private:
	const LexerConfig<TEnum>	&_config;
	const std::string			&_src;
	size_t						_cursor;
	std::vector<Token<TEnum> >	tokens;


public:
	GenericLexer(std::string &source, const LexerConfig<TEnum>& cfg);
	std::vector< Token<TEnum> > tokenize();

	bool isSeparator(char c);
	bool isQuote(char c);
	bool startsWith(const std::string& pattern);
	bool isCommentStart();
	void handleQuote(char quoteType, std::vector< Token<TEnum> >& tokens);
	bool skipComment();
};


enum TokenType {
	TOKEN_NULL,
	TOKEN_WORD,
	TOKEN_LBRACE,
	TOKEN_RBRACE,
	TOKEN_SEMICOLON,
	TOKEN_NEW_LINE,
	TOKEN_EOF
};

std::string tokenTypeToString(TokenType t);

template <typename TEnum>
class clsParse {
	size_t                      _cursor;
	std::vector< Token<TEnum> > _Tokens;
	TEnum                       _eofType;
	Token<TEnum>				_empty;

public:
	clsParse(std::vector< Token<TEnum> > &Tokens, TEnum eofType);
	~clsParse();
	Token<TEnum> &peek();
	Token<TEnum> &advance();
};

#endif