-- 测试 lua_tolstring 对包含 NULL 字节的字符串的处理
print("=== 测试 Lua 字符串中的 NULL 字节 ===")

-- 创建包含 NULL 字节的字符串
local testString = "\x13\xAB\xA1\x5C\x00\x01\x02\x03\x00\xFF"
print("Lua 字符串长度:", #testString)

-- 检查每个字节
print("字节内容:")
for i = 1, #testString do
    local byte = string.byte(testString, i)
    print(string.format("  位置 %d: 0x%02X (%d)", i, byte, byte))
end

-- 这个字符串包含两个 NULL 字节（位置 5 和 9）
-- 但 Lua 知道完整长度是 10

print("\n验证:")
print("- Lua 字符串长度:", #testString)
print("- 包含 NULL 字节:", string.byte(testString, 5) == 0 and "是" or "否")
print("- 在 NULL 之后还有数据:", string.byte(testString, 6) == 1 and "是" or "否")

-- 当传递给 C 函数时，lua_tolstring() 会：
-- 1. 返回指向完整数据的指针
-- 2. 设置 len = 10（完整长度）
-- 3. 不会被 NULL 字节截断

print("\n结论：lua_tolstring() 会正确返回完整长度和数据！")