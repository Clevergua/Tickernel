local gameMath = {}

function gameMath.Clamp(v, min, max)
    if v < min then
        return min
    elseif v > max then
        return max
    else
        return v
    end
end

function gameMath.Lerp(a, b, t)
    t = gameMath.Clamp(t, 0, 1)
    return a + (b - a) * t
end

function gameMath.CantorPair(a, b)
    return (a + b) * (a + b + 1) // 2 + b
end

function gameMath.LCGRandom(v)
    return 114067148579 * v + 728201631
end

function gameMath.PingPong(a, b, t)
    local floor = math.floor(t)
    local remainder = t - floor
    if floor % 2 == 0 then
        return a + (b - a) * remainder
    else
        return a + (b - a) * (1 - remainder)
    end
end

function gameMath.SmoothLerp(a, b, t)
    t = t * t * t * (6 * t * t - 15 * t + 10)
    return a + (b - a) * t
end

local Grad2D = function(hash, x, y)
    local gradIndex = hash % 8 + 1
    local gradients = {
        { 1, 1 }, { -1, 1 }, { 1, -1 }, { -1, -1 },
        { 1, 0 }, { -1, 0 }, { 0, 1 }, { 0, -1 }
    }
    local grad = gradients[gradIndex]
    return grad[1] * x + grad[2] * y
end

local DotGridGradient2D = function(ix, iy, x, y, seed)
    local dx = x - ix
    local dy = y - iy
    local hash = gameMath.LCGRandom(gameMath.CantorPair(gameMath.CantorPair(ix, iy), seed))
    hash = hash & 0xFF
    return Grad2D(hash, dx, dy)
end

---comment
---@param seed integer
---@param x number
---@param y number
---@return number
function gameMath.PerlinNoise2D(seed, x, y)
    -- Determine grid cell coordinates
    local x0 = math.floor(x)
    local x1 = x0 + 1
    local y0 = math.floor(y)
    local y1 = y0 + 1
    -- Determine interpolation weights
    -- Could also use higher order polynomial/s-curve here
    local sx = x - x0
    local sy = y - y0
    -- Interpolate between grid point gradients
    local n0, n1, ix0, ix1, value
    n0 = DotGridGradient2D(x0, y0, x, y, seed)
    n1 = DotGridGradient2D(x1, y0, x, y, seed)
    ix0 = gameMath.SmoothLerp(n0, n1, sx)
    n0 = DotGridGradient2D(x0, y1, x, y, seed)
    n1 = DotGridGradient2D(x1, y1, x, y, seed)
    ix1 = gameMath.SmoothLerp(n0, n1, sx)
    value = gameMath.SmoothLerp(ix0, ix1, sy)
    return value
end

local Grad3D = function(hash, x, y, z)
    local h = hash & 15
    local u = h < 8 and x or y
    local v = h < 4 and y or (h == 12 or h == 14) and x or z
    return ((h & 1) == 0 and u or -u) + ((h & 2) == 0 and v or -v)
end

local DotGridGradient3D = function(ix, iy, iz, x, y, z, seed)
    local dx = x - ix
    local dy = y - iy
    local dz = z - iz
    local hash = gameMath.LCGRandom(gameMath.CantorPair(gameMath.CantorPair(gameMath.CantorPair(ix, iy), iz), seed))
    hash = hash & 0xFF
    return Grad3D(hash, dx, dy, dz)
end

---comment
---@param seed number
---@param x number
---@param y number
---@param z number
---@return number
function gameMath.PerlinNoise3D(seed, x, y, z)
    -- Determine grid cell coordinates
    local x0 = math.floor(x)
    local x1 = x0 + 1
    local y0 = math.floor(y)
    local y1 = y0 + 1
    local z0 = math.floor(z)
    local z1 = z0 + 1
    -- Determine interpolation weights
    -- Could also use higher order polynomial/s-curve here
    local sx = x - x0
    local sy = y - y0
    local sz = z - z0
    -- Interpolate between grid point gradients
    local x00 = gameMath.SmoothLerp(DotGridGradient3D(x0, y0, z0, x, y, z, seed),
        DotGridGradient3D(x1, y0, z0, x, y, z, seed), sx)
    local x10 = gameMath.SmoothLerp(DotGridGradient3D(x0, y1, z0, x, y, z, seed),
        DotGridGradient3D(x1, y1, z0, x, y, z, seed), sx)
    local x01 = gameMath.SmoothLerp(DotGridGradient3D(x0, y0, z1, x, y, z, seed),
        DotGridGradient3D(x1, y0, z1, x, y, z, seed), sx)
    local x11 = gameMath.SmoothLerp(DotGridGradient3D(x0, y1, z1, x, y, z, seed),
        DotGridGradient3D(x1, y1, z1, x, y, z, seed), sx)

    local y0 = gameMath.SmoothLerp(x00, x10, sy)
    local y1 = gameMath.SmoothLerp(x01, x11, sy)

    return gameMath.SmoothLerp(y0, y1, sz)
end

-- local ns = 0.0099

-- local intervalCount = 40  -- 将间隔数量改为 20
-- local intervals = {}
-- for i = 1, intervalCount do
--     intervals[i] = 0
-- end

-- local min = 0
-- local max = 0
-- local totalCount = 0

-- for x = 1, 100 do
--     for y = 1, 100000 do
--         local n = gameMath.PerlinNoise2D(43214, x * ns, y * ns)
--         if n < min then
--             min = n
--         end
--         if n > max then
--             max = n
--         end
--         if n >= -1 and n <= 1 then
--             local index = math.floor(((n + 1) / 2) * intervalCount) + 1
--             if index < 1 then index = 1 end
--             if index > intervalCount then index = intervalCount end
--             intervals[index] = intervals[index] + 1
--             totalCount = totalCount + 1
--         end
--     end
--     for i = 1, intervalCount do
--         local rangeStart = -1 + (i - 1) * (2 / intervalCount)
--         local rangeEnd = rangeStart + (2 / intervalCount)
--         local percentage = (intervals[i] / totalCount) * 100
--         print(string.format("Interval %d (%.2f to %.2f): %.2f%%", i, rangeStart, rangeEnd, percentage))
--     end
-- end

-- print("Min value: ", min)
-- print("Max value: ", max)


return gameMath
