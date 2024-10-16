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
    return (1140671485 * v + 12820163) % 16777216
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

local SmoothLerp = function(a, b, t)
    t = t * t * t * (6 * t * t - 15 * t + 10)
    return a + (b - a) * t
end



local gradient2Ds = {
    { x = -1, y = -1 },
    { x = 1,  y = -1 },
    { x = -1, y = 1 },
    { x = 1,  y = 1 },
    { x = 0,  y = -1 },
    { x = -1, y = 0 },
    { x = 0,  y = 1 },
    { x = 1,  y = 0 },
}

local GetGradient2D = function(x, y, seed)
    local hash = gameMath.LCGRandom(gameMath.CantorPair(gameMath.CantorPair(x, y), seed))
    hash = hash > 0 and hash or math.abs(hash)
    local index = hash % 8 + 1
    local gradient = gradient2Ds[index]
    return gradient.x, gradient.y
end

local DotGridGradient2D = function(ix, iy, x, y, seed)
    local dx = x - ix
    local dy = y - iy

    local gx, gy = GetGradient2D(ix, iy, seed)
    return dx * gx + dy * gy
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
    ix0 = SmoothLerp(n0, n1, sx)
    n0 = DotGridGradient2D(x0, y1, x, y, seed)
    n1 = DotGridGradient2D(x1, y1, x, y, seed)
    ix1 = SmoothLerp(n0, n1, sx)
    value = SmoothLerp(ix0, ix1, sy)
    return value
end

local gradient3Ds =
{
    { x = 1,  y = 1,  z = 0 },
    { x = -1, y = 1,  z = 0 },
    { x = 1,  y = -1, z = 0 },
    { x = -1, y = -1, z = 0 },
    { x = 1,  y = 0,  z = 1 },
    { x = -1, y = 0,  z = 1 },
    { x = 1,  y = 0,  z = -1 },
    { x = -1, y = 0,  z = -1 },
    { x = 0,  y = 1,  z = 1 },
    { x = 0,  y = -1, z = 1 },
    { x = 0,  y = 1,  z = -1 },
    { x = 0,  y = -1, z = -1 },
    { x = 1,  y = 1,  z = 0 },
    { x = 0,  y = -1, z = 1 },
    { x = -1, y = 1,  z = 0 },
    { x = 0,  y = -1, z = -1 },
}

local GetGradient3D = function(x, y, z, seed)
    local hash = gameMath.LCGRandom(gameMath.CantorPair(gameMath.CantorPair(gameMath.CantorPair(x, y), z), seed))
    hash = hash > 0 and hash or math.abs(hash)
    local index = hash % 16 + 1
    local gradient = gradient3Ds[index]
    return gradient.x, gradient.y, gradient.z
end

local DotGridGradient3D = function(ix, iy, iz, x, y, z, seed)
    local dx = x - ix
    local dy = y - iy
    local dz = z - iz
    local gx, gy, gz = GetGradient3D(ix, iy, iz, seed)
    return dx * gx + dy * gy + dz * gz
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
    local x00 = SmoothLerp(DotGridGradient3D(x0, y0, z0, x, y, z, seed), DotGridGradient3D(x1, y0, z0, x, y, z, seed), sx);
    local x10 = SmoothLerp(DotGridGradient3D(x0, y1, z0, x, y, z, seed), DotGridGradient3D(x1, y1, z0, x, y, z, seed), sx);
    local x01 = SmoothLerp(DotGridGradient3D(x0, y0, z1, x, y, z, seed), DotGridGradient3D(x1, y0, z1, x, y, z, seed), sx);
    local x11 = SmoothLerp(DotGridGradient3D(x0, y1, z1, x, y, z, seed), DotGridGradient3D(x1, y1, z1, x, y, z, seed), sx);

    local y0 = SmoothLerp(x00, x10, sy);
    local y1 = SmoothLerp(x01, x11, sy);

    return SmoothLerp(y0, y1, sz);
end
return gameMath
