#include "gtest/gtest.h"
#include "config_parser.h"

// test fixture that initializes the parser and config
// from a given config file name
class NginxConfigParserTest : public ::testing::Test {
  public:
    void InitParser(const char* config_name) {
      success = parser_.Parse(config_name, &out_config);
    }

    NginxConfig out_config;
    bool success;
  
  protected:
    NginxConfigParser parser_;
    
};

// example config should succeed 
TEST_F(NginxConfigParserTest, ExampleConfigParse) {
  InitParser("example_config");
  EXPECT_TRUE(success);
}

// example config should have the expected string representation
TEST_F(NginxConfigParserTest, ExampleConfigToString) {
  InitParser("example_config");
  EXPECT_TRUE(success);

  std::string expected = "foo \"bar\";\nserver {\n  listen 80;\n  server_name foo.com;\n  root /home/ubuntu/sites/foo/;\n}\n";
  EXPECT_EQ(out_config.ToString(0), expected);
}

// example config should have the expected token statements
TEST_F(NginxConfigParserTest, ExampleConfigStatement) {
  InitParser("example_config");
  EXPECT_TRUE(success);

  NginxConfigStatement* statement = out_config.statements_[0].get();
  EXPECT_EQ(statement->tokens_[0], "foo");
  EXPECT_EQ(statement->tokens_[1], "\"bar\"");
  EXPECT_EQ(statement->child_block_, nullptr);

  statement = out_config.statements_[1].get();
  EXPECT_EQ(statement->tokens_[0], "server");
  EXPECT_EQ(statement->tokens_.size(), 1);
  EXPECT_TRUE(statement->child_block_ != nullptr);

  NginxConfig* child_block = statement->child_block_.get();
  statement = child_block->statements_[0].get();
  EXPECT_EQ(statement->tokens_[0], "listen");
  EXPECT_EQ(statement->tokens_[1], "80");
  EXPECT_EQ(statement->child_block_, nullptr);

  statement = child_block->statements_[1].get();
  EXPECT_EQ(statement->tokens_[0], "server_name");
  EXPECT_EQ(statement->tokens_[1], "foo.com");
  EXPECT_EQ(statement->child_block_, nullptr);

  statement = child_block->statements_[2].get();
  EXPECT_EQ(statement->tokens_[0], "root");
  EXPECT_EQ(statement->tokens_[1], "/home/ubuntu/sites/foo/");
  EXPECT_EQ(statement->child_block_, nullptr);
}

// string input text to parser should fail
TEST_F(NginxConfigParserTest, StringInput) {
  NginxConfigParser parser;
  NginxConfig out_config;

  bool success = parser.Parse("foo \"bar\";\nserver {\n  listen 80;\n  server_name foo.com;\n  root /home/ubuntu/sites/foo/;\n}\n", &out_config);
  EXPECT_FALSE(success);
}

// empty config should succeed and have empty string representation
TEST_F(NginxConfigParserTest, Empty) {
  InitParser("empty_config");
  EXPECT_TRUE(success);

  std::string expected = "";
  EXPECT_EQ(out_config.ToString(0), expected);
}

// not found config should fail
TEST_F(NginxConfigParserTest, Nonexistent) {
  InitParser("na_config");
  EXPECT_FALSE(success);
}

// invalid config statement should fail
TEST_F(NginxConfigParserTest, InvalidStatement) {
  InitParser("invalid_statement_config");
  EXPECT_FALSE(success);
}

// valid config all on one line should succeed
TEST_F(NginxConfigParserTest, OneLine) {
  InitParser("oneline_config");
  EXPECT_TRUE(success);
}

// valid config with one token should succeed
TEST_F(NginxConfigParserTest, OneTokenSemicolon) {
  InitParser("onetokensemicolon_config");
  EXPECT_TRUE(success);
}

// config with empty braces should succeed
TEST_F(NginxConfigParserTest, EmptyBraces) {
  InitParser("empty_braces_config");
  EXPECT_TRUE(success);
}

// config with empty nested blocks should succeed
TEST_F(NginxConfigParserTest, EmptyNested) {
  InitParser("empty_nested_config");
  EXPECT_TRUE(success);
}

// config with double ended braces should fail
TEST_F(NginxConfigParserTest, NestedWithDoubleEndBraces) {
  InitParser("nested_config_double_end_braces_config");
  EXPECT_FALSE(success);
}

// config with unclosed start braces should fail
TEST_F(NginxConfigParserTest, InvalidStartBraces) {
  InitParser("invalid_start_braces_config");
  EXPECT_FALSE(success);
}

// config with unclosed end braces should fail
TEST_F(NginxConfigParserTest, InvalidEndBraces) {
  InitParser("invalid_end_braces_config");
  EXPECT_FALSE(success);
}

// config with escaped characters should succeed
// and have the expected tokens below
TEST_F(NginxConfigParserTest, EscapedChar) {
  InitParser("escaped_char_config");
  EXPECT_TRUE(success);

  NginxConfigStatement* statement = out_config.statements_[0].get();
  EXPECT_EQ(statement->tokens_[0], "foo");
  EXPECT_EQ(statement->tokens_[1], "\"hello\"world\"");
  EXPECT_EQ(statement->child_block_, nullptr);

  statement = out_config.statements_[1].get();
  EXPECT_EQ(statement->tokens_[0], "bar");
  EXPECT_EQ(statement->tokens_[1], "'hello'world'");
  EXPECT_EQ(statement->child_block_, nullptr);
}

// config without whitespace after quoted token should fail
TEST_F(NginxConfigParserTest, QuotedTokenWhitespace) {
  InitParser("quoted_token_whitespace_config");
  EXPECT_FALSE(success);
}

// config with invalid token should fail
TEST_F(NginxConfigParserTest, InvalidToken) {
  InitParser("invalid_token_config");
  EXPECT_FALSE(success);
}

// config with invalid token (only {} in config) should fail
TEST_F(NginxConfigParserTest, InvalidTokenOnlyBraces) {
  InitParser("invalid_token_only_braces_config");
  EXPECT_FALSE(success);
}