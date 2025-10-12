local input = {}
input.keyCodeStates = {}
-- Key codes (Lua style with lowercase)
input.keyCode = {
    -- Letters
    a = 0,
    s = 1,
    d = 2,
    f = 3,
    h = 4,
    g = 5,
    z = 6,
    x = 7,
    c = 8,
    v = 9,
    b = 10,
    q = 11,
    w = 12,
    e = 13,
    r = 14,
    y = 15,
    t = 16,
    u = 17,
    i = 18,
    o = 19,
    p = 20,
    l = 21,
    j = 22,
    k = 23,
    n = 24,
    m = 25,

    -- Numbers
    num1 = 26,
    num2 = 27,
    num3 = 28,
    num4 = 29,
    num5 = 30,
    num6 = 31,
    num7 = 32,
    num8 = 33,
    num9 = 34,
    num0 = 35,

    -- Special Characters
    minus = 36,
    equal = 37,
    left_bracket = 38,
    right_bracket = 39,
    backslash = 40,
    semicolon = 41,
    apostrophe = 42,
    grave = 43,
    comma = 44,
    period = 45,
    slash = 46,

    -- Control Keys
    space = 47,
    enter = 48,
    tab = 49,
    backspace = 50,
    delete = 51,
    escape = 52,

    -- Arrow Keys
    left = 53,
    right = 54,
    up = 55,
    down = 56,

    -- Function Keys
    f1 = 57,
    f2 = 58,
    f3 = 59,
    f4 = 60,
    f5 = 61,
    f6 = 62,
    f7 = 63,
    f8 = 64,
    f9 = 65,
    f10 = 66,
    f11 = 67,
    f12 = 68,

    -- Navigation Keys
    home = 69,
    ["end"] = 70, -- 'end' is Lua keyword, need brackets
    page_up = 71,
    page_down = 72,

    -- Numpad
    numpad0 = 73,
    numpad1 = 74,
    numpad2 = 75,
    numpad3 = 76,
    numpad4 = 77,
    numpad5 = 78,
    numpad6 = 79,
    numpad7 = 80,
    numpad8 = 81,
    numpad9 = 82,
    numpad_period = 83,
    numpad_multiply = 84,
    numpad_add = 85,
    numpad_subtract = 86,
    numpad_divide = 87,
    numpad_enter = 88,
}

function input.getKeyCodeDown(key)
    return input.keyCodeStates[key]
end

return input