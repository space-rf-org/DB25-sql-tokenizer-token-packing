/*
 * Copyright (c) 2024 Chiradip Mandal
 * Author: Chiradip Mandal
 * Organization: Space-RF.org
 * 
 * This file is part of DB25 SQL Tokenizer.
 * 
 * Licensed under the MIT License. See LICENSE file for details.
 */

#pragma once

// ============================================================================
// PROTECTED FILE - DO NOT MODIFY
// ============================================================================
// SIMD-optimized SQL tokenizer - Foundation of the DB25 SQL Parser.
// This tokenizer has been thoroughly tested against complex SQL queries.
// 
// MODIFICATION RESTRICTION: This file is frozen for parser development.
// The parser must work with tokens as produced by this tokenizer.
// Any changes here would require revalidating all parser logic.
// ============================================================================

#include "simd_architecture.hpp"
#include "keywords.hpp"
#include <string_view>
#include <vector>

namespace db25 {

enum class TokenType : uint8_t {
    Unknown,
    Keyword,
    Identifier,
    Number,
    String,
    Operator,
    Delimiter,
    Whitespace,
    Comment,
    EndOfFile
};

// Packed token structure - optimized from 48 bytes to 32 bytes (33% reduction)
struct Token {
    // Small fields grouped together to minimize padding
    TokenType type;              // 1 byte @ offset 0
    uint8_t reserved;            // 1 byte @ offset 1 (for future use)
    Keyword keyword_id;          // 2 bytes @ offset 2-3
    uint32_t line;               // 4 bytes @ offset 4-7 (max 4 billion lines)
    uint32_t column;             // 4 bytes @ offset 8-11 (max 4 billion columns)
    // 4 bytes padding @ offset 12-15 (automatic alignment for string_view)
    std::string_view value;      // 16 bytes @ offset 16-31 (8-byte aligned)
    
    // Constructor for easy initialization
    Token() : type(TokenType::Unknown), reserved(0), keyword_id(Keyword::UNKNOWN), 
              line(0), column(0), value() {}
    
    Token(TokenType t, std::string_view v, uint32_t l, uint32_t c, 
          Keyword k = Keyword::UNKNOWN)
        : type(t), reserved(0), keyword_id(k), line(l), column(c), value(v) {}
};

// Verify token packing at compile time
static_assert(sizeof(Token) == 32, "Token structure should be 32 bytes for optimal packing");
static_assert(offsetof(Token, value) == 16, "string_view should be 8-byte aligned");

class SimdTokenizer {
private:
    SimdDispatcher dispatcher_;
    const std::byte* input_;
    size_t input_size_;
    size_t position_;
    uint32_t line_;     // Changed from size_t to uint32_t for packing
    uint32_t column_;   // Changed from size_t to uint32_t for packing
    
public:
    SimdTokenizer(const std::byte* input, size_t size);
    [[nodiscard]] std::vector<Token> tokenize();
    [[nodiscard]] const char* simd_level() const noexcept;
    
private:
    Token next_token();
    Token scan_identifier_or_keyword(size_t start, uint32_t start_line, uint32_t start_column);
    Token scan_number(size_t start, uint32_t start_line, uint32_t start_column);
    Token scan_string(size_t start, uint32_t start_line, uint32_t start_column, uint8_t quote);
    Token scan_comment(size_t start, uint32_t start_line, uint32_t start_column);
    Token scan_block_comment(size_t start, uint32_t start_line, uint32_t start_column);
    Token scan_operator_or_delimiter(size_t start, uint32_t start_line, uint32_t start_column);
    void update_position(size_t count);
};

}  // namespace db25