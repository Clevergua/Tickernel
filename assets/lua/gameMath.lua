local gameMath = {}
function gameMath.Round(v)
    return math.floor(0.5 + v)
end

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

local rotationMatrix = {
    { 0, 0, 0, 0 },
    { 0, 0, 0, 0 },
    { 0, 0, 0, 0 },
    { 0, 0, 0, 0 }
}

local scaleMatrix = {
    { 0, 0, 0, 0 },
    { 0, 0, 0, 0 },
    { 0, 0, 0, 0 },
    { 0, 0, 0, 0 }
}

local translateMatrix = {
    { 1, 0, 0, 0 },
    { 0, 1, 0, 0 },
    { 0, 0, 1, 0 },
    { 0, 0, 0, 1 }
}

local modelMatrix = {
    { 0, 0, 0, 0 },
    { 0, 0, 0, 0 },
    { 0, 0, 0, 0 },
    { 0, 0, 0, 0 }
}

function RotateAroundZ(angle)
    local rad = math.rad(angle)
    rotationMatrix[1][1] = math.cos(rad)
    rotationMatrix[1][2] = -math.sin(rad)
    rotationMatrix[1][3] = 0
    rotationMatrix[1][4] = 0

    rotationMatrix[2][1] = math.sin(rad)
    rotationMatrix[2][2] = math.cos(rad)
    rotationMatrix[2][3] = 0
    rotationMatrix[2][4] = 0

    rotationMatrix[3][1] = 0
    rotationMatrix[3][2] = 0
    rotationMatrix[3][3] = 1
    rotationMatrix[3][4] = 0

    rotationMatrix[4][1] = 0
    rotationMatrix[4][2] = 0
    rotationMatrix[4][3] = 0
    rotationMatrix[4][4] = 1

    return rotationMatrix
end

function ScaleModel(scale)
    scaleMatrix[1][1] = scale
    scaleMatrix[1][2] = 0
    scaleMatrix[1][3] = 0
    scaleMatrix[1][4] = 0

    scaleMatrix[2][1] = 0
    scaleMatrix[2][2] = scale
    scaleMatrix[2][3] = 0
    scaleMatrix[2][4] = 0

    scaleMatrix[3][1] = 0
    scaleMatrix[3][2] = 0
    scaleMatrix[3][3] = scale
    scaleMatrix[3][4] = 0

    scaleMatrix[4][1] = 0
    scaleMatrix[4][2] = 0
    scaleMatrix[4][3] = 0
    scaleMatrix[4][4] = 1

    return scaleMatrix
end

function TranslateModel(x, y, z)
    translateMatrix[1][4] = x
    translateMatrix[2][4] = y
    translateMatrix[3][4] = z
    return translateMatrix
end

function MatrixMultiply(A, B, result)
    for i = 1, 4 do
        for j = 1, 4 do
            result[i][j] = 0
            for k = 1, 4 do
                result[i][j] = result[i][j] + A[i][k] * B[k][j]
            end
        end
    end
    return result
end

function ApplyTransformations(scale, x, y, z, angle)
    local rotationMatrix = RotateAroundZ(angle)
    local scaleMatrix = ScaleModel(scale)
    local translateMatrix = TranslateModel(x, y, z)
    return MatrixMultiply(translateMatrix, MatrixMultiply(rotationMatrix, scaleMatrix, modelMatrix), modelMatrix)
end

local Grad2D = function(hash, x, y)
    local h = hash & 7 -- 仅保留哈希的低三位
    local u = h < 4 and x or y
    local v = h < 4 and y or x
    local u_sign = (h & 1) == 0 and 1 or -1
    local v_sign = (h & 2) == 0 and 1 or -1
    return u * u_sign + v * v_sign
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

-- local Grad3D = function(hash, x, y, z)
--     local h = hash & 15
--     local u = h < 8 and x or y
--     local v = h < 4 and y or (h == 12 or h == 14) and x or z
--     return ((h & 1) == 0 and u or -u) + ((h & 2) == 0 and v or -v)
-- end

-- local DotGridGradient3D = function(ix, iy, iz, x, y, z, seed)
--     local dx = x - ix
--     local dy = y - iy
--     local dz = z - iz
--     local hash = gameMath.LCGRandom(gameMath.CantorPair(gameMath.CantorPair(gameMath.CantorPair(ix, iy), iz), seed))
--     hash = hash & 0xFF
--     return Grad3D(hash, dx, dy, dz)
-- end

-- ---comment
-- ---@param seed number
-- ---@param x number
-- ---@param y number
-- ---@param z number
-- ---@return number
-- function gameMath.PerlinNoise3D(seed, x, y, z)
--     -- Determine grid cell coordinates
--     local x0 = math.floor(x)
--     local x1 = x0 + 1
--     local y0 = math.floor(y)
--     local y1 = y0 + 1
--     local z0 = math.floor(z)
--     local z1 = z0 + 1
--     -- Determine interpolation weights
--     -- Could also use higher order polynomial/s-curve here
--     local sx = x - x0
--     local sy = y - y0
--     local sz = z - z0
--     -- Interpolate between grid point gradients
--     local x00 = gameMath.SmoothLerp(DotGridGradient3D(x0, y0, z0, x, y, z, seed),
--         DotGridGradient3D(x1, y0, z0, x, y, z, seed), sx)
--     local x10 = gameMath.SmoothLerp(DotGridGradient3D(x0, y1, z0, x, y, z, seed),
--         DotGridGradient3D(x1, y1, z0, x, y, z, seed), sx)
--     local x01 = gameMath.SmoothLerp(DotGridGradient3D(x0, y0, z1, x, y, z, seed),
--         DotGridGradient3D(x1, y0, z1, x, y, z, seed), sx)
--     local x11 = gameMath.SmoothLerp(DotGridGradient3D(x0, y1, z1, x, y, z, seed),
--         DotGridGradient3D(x1, y1, z1, x, y, z, seed), sx)

--     local y0 = gameMath.SmoothLerp(x00, x10, sy)
--     local y1 = gameMath.SmoothLerp(x01, x11, sy)

--     return gameMath.SmoothLerp(y0, y1, sz)
-- end

-- -- 开始时间
-- local startTime = os.clock()

-- -- 要测量的代码块

-- local ns = 0.17
-- local intervalCount = 20
-- local intervals = {}
-- for i = 1, intervalCount do
--     intervals[i] = 0
-- end
-- local min = 0
-- local max = 0
-- local totalCount = 0
-- for x = 1, 100 do
--     for y = 1, 10000 do
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
-- end
-- for i = 1, intervalCount do
--     local rangeStart = -1 + (i - 1) * (2 / intervalCount)
--     local rangeEnd = rangeStart + (2 / intervalCount)
--     local percentage = (intervals[i] / totalCount) * 100
--     print(string.format("Interval %d (%.2f to %.2f): %.2f%%", i, rangeStart, rangeEnd, percentage))
-- end
-- print("Min value: ", min)
-- print("Max value: ", max)

-- -- 结束时间
-- local endTime = os.clock()

-- -- 计算并打印执行耗时
-- local elapsedTime = endTime - startTime
-- print(string.format("执行耗时: %.4f 秒", elapsedTime))
local rotationMatrix = {
    { 0, 0, 0, 0 },
    { 0, 0, 0, 0 },
    { 0, 0, 0, 0 },
    { 0, 0, 0, 0 }
}

local scaleMatrix = {
    { 0, 0, 0, 0 },
    { 0, 0, 0, 0 },
    { 0, 0, 0, 0 },
    { 0, 0, 0, 0 }
}

local translateMatrix = {
    { 1, 0, 0, 0 },
    { 0, 1, 0, 0 },
    { 0, 0, 1, 0 },
    { 0, 0, 0, 1 }
}

function RotateAroundZ(angle)
    local rad = math.rad(angle)
    rotationMatrix[1][1] = math.cos(rad)
    rotationMatrix[1][2] = -math.sin(rad)
    rotationMatrix[1][3] = 0
    rotationMatrix[1][4] = 0

    rotationMatrix[2][1] = math.sin(rad)
    rotationMatrix[2][2] = math.cos(rad)
    rotationMatrix[2][3] = 0
    rotationMatrix[2][4] = 0

    rotationMatrix[3][1] = 0
    rotationMatrix[3][2] = 0
    rotationMatrix[3][3] = 1
    rotationMatrix[3][4] = 0

    rotationMatrix[4][1] = 0
    rotationMatrix[4][2] = 0
    rotationMatrix[4][3] = 0
    rotationMatrix[4][4] = 1

    return rotationMatrix
end

function ScaleModel(scale)
    scaleMatrix[1][1] = scale
    scaleMatrix[1][2] = 0
    scaleMatrix[1][3] = 0
    scaleMatrix[1][4] = 0

    scaleMatrix[2][1] = 0
    scaleMatrix[2][2] = scale
    scaleMatrix[2][3] = 0
    scaleMatrix[2][4] = 0

    scaleMatrix[3][1] = 0
    scaleMatrix[3][2] = 0
    scaleMatrix[3][3] = scale
    scaleMatrix[3][4] = 0

    scaleMatrix[4][1] = 0
    scaleMatrix[4][2] = 0
    scaleMatrix[4][3] = 0
    scaleMatrix[4][4] = 1

    return scaleMatrix
end

function TranslateModel(x, y, z)
    translateMatrix[1][1] = 1
    translateMatrix[1][2] = 0
    translateMatrix[1][3] = 0
    translateMatrix[1][4] = x

    translateMatrix[2][1] = 0
    translateMatrix[2][2] = 1
    translateMatrix[2][3] = 0
    translateMatrix[2][4] = y

    translateMatrix[3][1] = 0
    translateMatrix[3][2] = 0
    translateMatrix[3][3] = 1
    translateMatrix[3][4] = z

    translateMatrix[4][1] = 0
    translateMatrix[4][2] = 0
    translateMatrix[4][3] = 0
    translateMatrix[4][4] = 1
    return translateMatrix
end

function MatrixMultiply(A, B, result)
    for i = 1, 4 do
        for j = 1, 4 do
            result[i][j] = 0
            for k = 1, 4 do
                result[i][j] = result[i][j] + A[i][k] * B[k][j]
            end
        end
    end
end

local modelMatrix = {
    { 0, 0, 0, 0 },
    { 0, 0, 0, 0 },
    { 0, 0, 0, 0 },
    { 0, 0, 0, 0 }
}
function gameMath.ApplyTransformations(scale, x, y, z, angle, matrix)
    local rotationMatrix = RotateAroundZ(angle)
    local scaleMatrix = ScaleModel(scale)
    local translateMatrix = TranslateModel(x, y, z)
    MatrixMultiply(rotationMatrix, scaleMatrix, modelMatrix)
    MatrixMultiply(translateMatrix, modelMatrix, matrix)
end

function gameMath.CreateMatrix()
    return {
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
    }
end

return gameMath
