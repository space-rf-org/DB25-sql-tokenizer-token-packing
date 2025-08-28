/*
 * Copyright (c) 2024 Chiradip Mandal
 * Author: Chiradip Mandal
 * Organization: Space-RF.org
 * 
 * This file is part of DB25 SQL Tokenizer.
 * 
 * Licensed under the MIT License. See LICENSE file for details.
 */

#include <iostream>
#include <cstddef>
#include <cassert>
#include "../include/simd_tokenizer.hpp"

using namespace db25;

void validate_token_structure() {
    std::cout << "=== Token Structure Validation ===\n";
    
    // Verify size is exactly 32 bytes
    constexpr size_t expected_size = 32;
    size_t actual_size = sizeof(Token);
    
    std::cout << "Expected size: " << expected_size << " bytes\n";
    std::cout << "Actual size: " << actual_size << " bytes\n";
    
    assert(actual_size == expected_size && "Token size must be 32 bytes");
    
    // Verify field offsets for optimal packing
    std::cout << "\nField offsets:\n";
    std::cout << "  type:       " << offsetof(Token, type) << " bytes\n";
    std::cout << "  reserved:   " << offsetof(Token, reserved) << " bytes\n";
    std::cout << "  keyword_id: " << offsetof(Token, keyword_id) << " bytes\n";
    std::cout << "  line:       " << offsetof(Token, line) << " bytes\n";
    std::cout << "  column:     " << offsetof(Token, column) << " bytes\n";
    std::cout << "  value:      " << offsetof(Token, value) << " bytes\n";
    
    // Verify string_view is 8-byte aligned for performance
    assert(offsetof(Token, value) % 8 == 0 && "string_view must be 8-byte aligned");
    assert(offsetof(Token, value) == 16 && "string_view should be at offset 16");
    
    std::cout << "\n✅ Token structure is properly packed!\n";
}

void test_memory_savings() {
    std::cout << "\n=== Memory Savings Analysis ===\n";
    
    // Calculate memory usage for different token counts
    const size_t token_counts[] = {1000, 10000, 100000, 1000000};
    const size_t old_token_size = 48; // Previous size
    const size_t new_token_size = 32; // Current packed size
    
    for (size_t count : token_counts) {
        size_t old_memory = old_token_size * count;
        size_t new_memory = new_token_size * count;
        size_t saved = old_memory - new_memory;
        float reduction = (1.0f - (float)new_memory / old_memory) * 100;
        
        std::cout << "Tokens: " << count << "\n";
        std::cout << "  Old: " << old_memory / 1024 << " KB\n";
        std::cout << "  New: " << new_memory / 1024 << " KB\n";
        std::cout << "  Saved: " << saved / 1024 << " KB (" 
                  << reduction << "% reduction)\n";
    }
}

void test_token_creation() {
    std::cout << "\n=== Token Creation Test ===\n";
    
    // Test default constructor
    Token t1;
    assert(t1.type == TokenType::Unknown);
    assert(t1.keyword_id == Keyword::UNKNOWN);
    assert(t1.line == 0);
    assert(t1.column == 0);
    assert(t1.value.empty());
    
    // Test parameterized constructor
    std::string_view test_value = "SELECT";
    Token t2(TokenType::Keyword, test_value, 10, 5, Keyword::SELECT);
    assert(t2.type == TokenType::Keyword);
    assert(t2.keyword_id == Keyword::SELECT);
    assert(t2.line == 10);
    assert(t2.column == 5);
    assert(t2.value == test_value);
    
    // Test large line/column numbers (within uint32_t range)
    Token t3(TokenType::Identifier, "test", 4294967295U, 4294967295U);
    assert(t3.line == 4294967295U);
    assert(t3.column == 4294967295U);
    
    std::cout << "✅ Token creation tests passed!\n";
}

int main() {
    std::cout << "Running Token Packing Tests...\n\n";
    
    validate_token_structure();
    test_memory_savings();
    test_token_creation();
    
    std::cout << "\n=== All Tests Passed! ===\n";
    std::cout << "Token packing optimization successfully implemented.\n";
    std::cout << "Memory reduction: 33% (from 48 to 32 bytes per token)\n";
    
    return 0;
}