local min, max = 0, 0
local engine = require("engine")
local gameMath = require("gameMath")
local game = require("game")
-- local profiler = require("profiler")
local voxelCount = 32
local modelScale = 1 / voxelCount;
local voxel = {
    dirt = { 210, 150, 95, 255 },
    snow = { 250, 250, 250, 255 },
    ice = { 190, 190, 245, 255 },
    sand = { 245, 212, 163, 255 },
    grass = { 170, 225, 115, 255 },
    water = { 86, 98, 185, 255 },
    lava = { 225, 100, 67, 255 },
    volcanicRock = { 51, 41, 41, 255 },
    volcanicAsh = { 96, 96, 96, 255 },
    stone = { 128, 128, 128, 255 }
}

local terrain = game.terrain
local temperatureMap = {
    { -1, -1, -1 },
    { 0,  0,  0 },
    { 1,  1,  1 },
}
local humidityMap = {
    { -1, 0, 1 },
    { -1, 0, 1 },
    { -1, 0, 1 },
}
local foundationRoughnessNoiseScaleMap = {
    { 1.7,  1.7, 0.47 },
    { 0.47, 1.7, 0.47 },
    { 3.3,  3.3, 3.3 },
}
local foundationHeightMap = {
    { 5, 5, 3 },
    { 5, 5, 3 },
    { 3, 3, 5 },
}
local foundationMap = {
    { voxel.dirt,         voxel.dirt,         voxel.sand },
    { voxel.sand,         voxel.dirt,         voxel.sand },
    { voxel.volcanicRock, voxel.volcanicRock, voxel.volcanicRock },
}

-- local terrainViewMap = {
--     {
--         {
--             terrain = terrain.snow,
--             foundationRoughnessNoiseScale = 1.84,
--             foundationRoughnessStep = 0.27,
--             foundationVoxel = voxel.dirt,
--             foundationDeltaHeight = 0,
--         },
--         {
--             terrain = terrain.snow,
--             foundationRoughnessNoiseScale = 1.84,
--             foundationRoughnessStep = 0.27,
--             foundationVoxel = voxel.dirt,
--             foundationDeltaHeight = 0,
--         },
--         {
--             terrain = terrain.ice,
--             foundationRoughnessNoiseScale = 0.48,
--             foundationRoughnessStep = 0.41,
--             foundationVoxel = voxel.sand,
--             foundationDeltaHeight = -3,

--         },
--     },
--     {
--         {
--             terrain = terrain.sand,
--             foundationRoughnessNoiseScale = 0.48,
--             foundationRoughnessStep = 0.31,
--             foundationVoxel = voxel.sand,
--             foundationDeltaHeight = 2,
--         },
--         {
--             terrain = terrain.grass,
--             foundationRoughnessNoiseScale = 3.68,
--             foundationRoughnessStep = 0.27,
--             foundationVoxel = voxel.dirt,
--             foundationDeltaHeight = 0,
--         },
--         {
--             terrain = terrain.water,
--             foundationRoughnessNoiseScale = 0.48,
--             foundationRoughnessStep = 0.41,
--             foundationVoxel = voxel.sand,
--             foundationDeltaHeight = -3,
--         },
--     },
--     {
--         {
--             terrain = terrain.lava,
--             foundationRoughnessNoiseScale = 2.48,
--             foundationRoughnessStep = 0.2,
--             foundationVoxel = voxel.volcanicRock,
--             foundationDeltaHeight = -3,
--         },
--         {
--             terrain = terrain.volcanic,
--             foundationRoughnessNoiseScale = 2.48,
--             foundationRoughnessStep = 0.2,
--             foundationVoxel = voxel.volcanicRock,
--             foundationDeltaHeight = 2,
--         },
--         {
--             terrain = terrain.volcanic,
--             foundationRoughnessNoiseScale = 2.48,
--             foundationRoughnessStep = 0.2,
--             foundationVoxel = voxel.volcanicRock,
--             foundationDeltaHeight = 2,
--         },
--     },
-- }

local foundationSeed = 124321
local stoneNoiseSeed = 63548
local surfaceNoiseSeed = 213456
local grassNoiseSeed = 56123
local lavaNoiseSeed = 321412

-- function GetVoxelInterpolation(temperature, humidity)
--     local x0, x1, y0, y1
--     local dx = 0
--     local dy = 0
--     local delta = 0.05
--     if temperature < -game.temperatureStep - delta then
--         x0 = 1
--         x1 = 1
--     elseif temperature < -game.temperatureStep then
--         x0 = 1
--         x1 = 2
--         dx = (temperature - (-game.temperatureStep - delta)) / delta
--     elseif temperature < game.temperatureStep then
--         x0 = 2
--         x1 = 2
--     elseif temperature < game.temperatureStep + delta then
--         x0 = 2
--         x1 = 3
--         dx = (temperature - game.temperatureStep) / delta
--     else
--         x0 = 3
--         x1 = 3
--     end
--     if humidity < -game.humidityStep - delta then
--         y0 = 1
--         y1 = 1
--     elseif humidity < -game.humidityStep then
--         y0 = 1
--         y1 = 2
--         dy = (humidity - (-game.humidityStep - delta)) / delta
--     elseif humidity < game.humidityStep then
--         y0 = 2
--         y1 = 2
--     elseif humidity < game.humidityStep + delta then
--         y0 = 2
--         y1 = 3
--         dy = (humidity - game.humidityStep) / delta
--     else
--         y0 = 3
--         y1 = 3
--     end

--     local r00 = terrainViewMap[x0][y0]
--     local r01 = terrainViewMap[x0][y1]
--     local r10 = terrainViewMap[x1][y0]
--     local r11 = terrainViewMap[x1][y1]

--     return r00, r01, r10, r11, dx, dy
-- end

-- function GetVoxelViewInterpolatdData(r00, r01, r10, r11, dx, dy, key)
--     local tx = gameMath.SmoothLerp(r00[key], r01[key], dx)
--     local ty = gameMath.SmoothLerp(r10[key], r11[key], dx)
--     local result = gameMath.SmoothLerp(tx, ty, dy)
--     return result
-- end

-- function GetVoxelViewData(temperature, humidity)
--     if temperature < -game.temperatureStep then
--         if humidity < -game.humidityStep then
--             return terrainViewMap[1][1]
--         elseif humidity < game.humidityStep then
--             return terrainViewMap[1][2]
--         else
--             return terrainViewMap[1][3]
--         end
--     elseif temperature < game.temperatureStep then
--         if humidity < -game.humidityStep then
--             return terrainViewMap[2][1]
--         elseif humidity < game.humidityStep then
--             return terrainViewMap[2][2]
--         else
--             return terrainViewMap[2][3]
--         end
--     else
--         if humidity < -game.humidityStep then
--             return terrainViewMap[3][1]
--         elseif humidity < game.humidityStep then
--             return terrainViewMap[3][2]
--         else
--             return terrainViewMap[3][3]
--         end
--     end
-- end

function GetInterpolationValue(value, step, delta)
    if value < -step - delta then
        return 1, 1, 0
    elseif value < -step then
        return 1, 2, (value + step + delta) / delta
    elseif value < step then
        return 2, 2, 0.5
    elseif value < step + delta then
        return 2, 3, (value - step) / delta
    else
        return 3, 3, 1
    end
end

function GetVoxelInterpolation(temperature, humidity, map)
    local delta = 0.05
    local x0, x1, dx = GetInterpolationValue(temperature, game.temperatureStep, delta)
    local y0, y1, dy = GetInterpolationValue(humidity, game.humidityStep, delta)
    local r00 = map[x0][y0]
    local r01 = map[x0][y1]
    local r10 = map[x1][y0]
    local r11 = map[x1][y1]
    local tx = gameMath.SmoothLerp(r00, r01, dx)
    local ty = gameMath.SmoothLerp(r10, r11, dx)
    return gameMath.SmoothLerp(tx, ty, dy)
end

function GetVoxel(temperature, humidity, map)
    if temperature < -game.temperatureStep then
        if humidity < -game.humidityStep then
            return map[1][1]
        elseif humidity < game.humidityStep then
            return map[1][2]
        else
            return map[1][3]
        end
    elseif temperature < game.temperatureStep then
        if humidity < -game.humidityStep then
            return map[2][1]
        elseif humidity < game.humidityStep then
            return map[2][2]
        else
            return map[2][3]
        end
    else
        if humidity < -game.humidityStep then
            return map[3][1]
        elseif humidity < game.humidityStep then
            return map[3][2]
        else
            return map[3][3]
        end
    end
end

local length = 16
local width = 16
function engine.Start()
    print("Lua Start")

    -- local modelsPath = engine.assetsPath ..
    --     engine.pathSeparator .. "models" .. engine.pathSeparator
    -- print("Loading models..")
    -- local models = {
    --     engine.LoadModel(modelsPath .. "LargeBuilding01_0.tknvox"),
    --     engine.LoadModel(modelsPath .. "SmallBuilding01_0.tknvox"),
    --     engine.LoadModel(modelsPath .. "SmallBuilding02_0.tknvox"),
    --     engine.LoadModel(modelsPath .. "SmallBuilding03_0.tknvox"),
    --     engine.LoadModel(modelsPath .. "SmallBuilding04_0.tknvox"),
    --     engine.LoadModel(modelsPath .. "TallBuilding01_0.tknvox"),
    --     engine.LoadModel(modelsPath .. "TallBuilding02_0.tknvox"),
    --     engine.LoadModel(modelsPath .. "TallBuilding03_0.tknvox"),
    -- }
    -- for index, model in ipairs(models) do
    --     local count = 6
    --     local instances = {}
    --     for i = 1, count do
    --         local x = math.abs(gameMath.LCGRandom((index + 13251) * 525234532 + i * 42342345)) % length
    --         local y = math.abs(gameMath.LCGRandom((x - 2317831) * 431513511 + index * 24141312 - i * 2131204)) % width
    --         instances[i] = {
    --             { modelScale, 0,          0,          x },
    --             { 0,          modelScale, 0,          y },
    --             { 0,          0,          modelScale, 4 * modelScale },
    --             { 0,          0,          0,          1 },
    --         }
    --     end
    --     engine.DrawModel(instances, model)
    -- end


    local seed = 6345
    game.GenerateWorld(seed, length, width)
    print("Generating world..")

    local voxelMap = {}
    local height = 16
    for x = 0, length * voxelCount + 1 do
        voxelMap[x] = {}
        for y = 0, width * voxelCount + 1 do
            voxelMap[x][y] = {}
            for z = 0, height + 1 do
                voxelMap[x][y][z] = nil
            end
        end
    end

    print("Generating terrain foundation..")
    for x = 1, length do
        for y = 1, width do
            local temperature = game.GetTemperature(x, y)
            local humidity = game.GetHumidity(x, y)
            for px = 1, voxelCount do
                for py = 1, voxelCount do
                    local dx = (px - (voxelCount + 1) / 2) / voxelCount
                    local dy = (py - (voxelCount + 1) / 2) / voxelCount
                    local deltaNoise = math.max(math.abs(dx), math.abs(dy)) * 2
                    deltaNoise = deltaNoise ^ 1
                    local voxelTemperature = game.GetTemperature((x + dx), (y + dy))
                    local voxelHumidity = game.GetHumidity((x + dx), (y + dy))
                    voxelTemperature = gameMath.Lerp(temperature, voxelTemperature, deltaNoise)
                    voxelHumidity = gameMath.Lerp(humidity, voxelHumidity, deltaNoise)
                    local foundationRoughnessNoiseScale = GetVoxelInterpolation(voxelTemperature, voxelHumidity,
                        foundationRoughnessNoiseScaleMap)
                    local foundationRoughnessNoise = gameMath.PerlinNoise2D(foundationSeed,
                        foundationRoughnessNoiseScale * (x + dx),
                        foundationRoughnessNoiseScale * (y + dy))
                    local foundationRoughnessStep = 0.27
                    local foundationHeight = GetVoxelInterpolation(voxelTemperature, voxelHumidity,
                        foundationHeightMap)
                    foundationHeight = foundationHeight +
                        gameMath.Clamp(foundationRoughnessNoise // foundationRoughnessStep, -1, 1)
                    -- local foundationVoxel = foundationMap
                    for z = 1, foundationHeight do
                        voxelMap[(x - 1) * voxelCount + px][(y - 1) * voxelCount + py][z] = GetVoxel(temperature,
                        humidity, foundationMap)
                    end
                    -- local r00, r01, r10, r11, tx, ty = GetVoxelInterpolation(voxelTemperature, voxelHumidity)
                    -- local foundationRoughnessNoiseScale = GetVoxelViewInterpolatdData(r00, r01, r10, r11, tx, ty,
                    --     "foundationRoughnessNoiseScale")
                    -- local foundationRoughnessNoise = gameMath.PerlinNoise2D(foundationSeed,
                    --     foundationRoughnessNoiseScale * (x + dx),
                    --     foundationRoughnessNoiseScale * (y + dy))
                    -- local foundationRoughnessStep = GetVoxelViewInterpolatdData(r00, r01, r10, r11, tx, ty,
                    --     "foundationRoughnessStep")
                    -- local foundationDeltaHeight = GetVoxelViewInterpolatdData(r00, r01, r10, r11, tx, ty,
                    --     "foundationDeltaHeight")
                    -- local deltaHeight = gameMath.Clamp(foundationRoughnessNoise // -foundationRoughnessStep, -2, 2)

                    -- if foundationRoughnessNoise < -foundationRoughnessStep then
                    --     deltaHeight = -1
                    -- elseif foundationRoughnessNoise < foundationRoughnessStep then
                    --     deltaHeight = 0
                    -- else
                    --     deltaHeight = 1
                    -- end

                    -- local voxelHeightMap = voxelMap[(x - 1) * voxelCount + px][(y - 1) * voxelCount + py]
                    -- local voxelViewData = GetVoxelViewData(voxelTemperature, voxelHumidity);
                    -- for z = 1, 5 + deltaHeight + foundationDeltaHeight do
                    --     voxelHeightMap[z] = voxelViewData.foundationVoxel
                    -- end

                    -- local stoneNoiseScale = 3.86
                    -- local stoneNoise = gameMath.PerlinNoise2D(stoneNoiseSeed, stoneNoiseScale * (x + dx),
                    --     stoneNoiseScale * (y + dy))
                    -- local stoneNoiseStep = 0.618
                    -- local maxHeight
                    -- if stoneNoise > stoneNoiseStep then
                    --     maxHeight = 6 + deltaHeight + foundationDeltaHeight
                    --     for z = 5, 6 do
                    --         voxelHeightMap[z] = voxel.stone
                    --     end
                    -- else
                    --     maxHeight = 5 + deltaHeight + foundationDeltaHeight
                    -- end
                    -- if voxelViewData.terrain == terrain.snow then
                    --     local snowNoiseScale = 0.48
                    --     local snowNoise = gameMath.PerlinNoise2D(surfaceNoiseSeed,
                    --         snowNoiseScale * (x + dx),
                    --         snowNoiseScale * (y + dy))
                    --     local snowHeight = maxHeight + snowNoise // 0.33
                    --     -- if snowNoise < -0.66 then
                    --     --     snowHeight = 5 + deltaHeight + foundationDeltaHeight
                    --     -- elseif snowNoise < -0.33 then
                    --     --     snowHeight = 6 + deltaHeight + foundationDeltaHeight
                    --     -- else
                    --     --     snowHeight = 7 + deltaHeight + foundationDeltaHeight
                    --     -- end
                    --     for z = maxHeight + 1, snowHeight do
                    --         voxelHeightMap[z] = voxel.snow
                    --     end
                    -- elseif voxelViewData.terrain == terrain.water then
                    --     for z = maxHeight + 1, maxHeight + 4 do
                    --         if z < 6 then
                    --             voxelMap[(x - 1) * voxelCount + px][(y - 1) * voxelCount + py][z] = voxel.water
                    --         end
                    --     end
                    -- elseif voxelViewData.terrain == terrain.ice then
                    --     for z = maxHeight + 1, maxHeight + 5 do
                    --         if z < 7 then
                    --             voxelMap[(x - 1) * voxelCount + px][(y - 1) * voxelCount + py][z] = voxel.ice
                    --         end
                    --     end
                    -- elseif voxelViewData.terrain == terrain.grass then
                    --     local grassNoiseScale = 1.35
                    --     local grassNoise = gameMath.PerlinNoise2D(grassNoiseSeed, grassNoiseScale * (x + dx),
                    --         grassNoiseScale * (y + dy))
                    --     if grassNoise < -0.27 then

                    --     elseif grassNoise < 0.27 then
                    --         for z = maxHeight + 1, maxHeight + 1 do
                    --             if z < 7 then
                    --                 voxelMap[(x - 1) * voxelCount + px][(y - 1) * voxelCount + py][z] = voxel.grass
                    --             end
                    --         end
                    --     else

                    --     end
                    -- elseif voxelViewData.terrain == terrain.lava then
                    --     local lavaNoiseScale = 0.35
                    --     local lavaNoise = gameMath.PerlinNoise2D(lavaNoiseSeed, lavaNoiseScale * (x + dx),
                    --         lavaNoiseScale * (y + dy))
                    --     if lavaNoise < -0.31 then

                    --     elseif lavaNoise < 0.31 then
                    --         for z = maxHeight + 1, maxHeight + 1 do
                    --             if z < 7 then
                    --                 voxelMap[(x - 1) * voxelCount + px][(y - 1) * voxelCount + py][z] = voxel.lava
                    --             end
                    --         end
                    --     else
                    --         for z = maxHeight + 1, maxHeight + 2 do
                    --             if z < 7 then
                    --                 voxelMap[(x - 1) * voxelCount + px][(y - 1) * voxelCount + py][z] = voxel.lava
                    --             end
                    --         end
                    --     end
                    -- end
                end
            end
        end
    end
    -- profiler.stop()
    print("Generating terrain surface..")
    print(min, max);
    local vertices = {}
    local colors = {}
    local normals = {}

    local waterVertices = {}
    local waterColors = {}
    local waterNormals = {}
    for x = 1, length * voxelCount do
        for y = 1, width * voxelCount do
            for z = 1, height do
                if voxelMap[x][y][z] ~= nil then
                    if voxelMap[x][y][z] == voxel.water then
                        table.insert(waterVertices, { x, y, z })
                        table.insert(waterColors, voxelMap[x][y][z])
                        table.insert(waterNormals, { 0, 0, 0 })
                    else
                        local isSurrounded = true
                        for dx = -1, 1 do
                            for dy = -1, 1 do
                                for dz = -1, 1 do
                                    if dx ~= 0 or dy ~= 0 or dz ~= 0 and voxelMap[x + dx][y + dy][z + dz] == nil then
                                        isSurrounded = false
                                        break
                                    end
                                end
                            end
                        end
                        if isSurrounded then
                            -- continue
                        else
                            table.insert(vertices, { x, y, z })
                            table.insert(colors, voxelMap[x][y][z])
                            table.insert(normals, { 0, 0, 0 })
                        end
                    end
                end
            end
        end
    end

    print("Drawing models..")
    engine.SetNormals(vertices, normals, voxelMap)
    engine.SetNormals(waterVertices, waterNormals, voxelMap)
    local index = engine.AddModelToOpaqueGeometrySubpass(vertices, colors, normals)
    -- local waterIndex = engine.AddModelToWaterGeometrySubpass(waterVertices, waterColors, waterNormals)
    local modelMatrix = {
        {
            { modelScale, 0,          0,          0 },
            { 0,          modelScale, 0,          0 },
            { 0,          0,          modelScale, 0 },
            { 0,          0,          0,          1 },
        },
    }
    engine.UpdateInstancesInOpaqueGeometrySubpass(index, modelMatrix)
    -- engine.UpdateInstancesInWaterGeometrySubpass(waterIndex, modelMatrix)
    print("Generating lightings..")
    local pointLights = {}
    for x = 1, length do
        for y = 1, width do
            local random = gameMath.LCGRandom(gameMath.CantorPair(x, y)) % 100
            if random < 5 and #pointLights < 256 then
                table.insert(pointLights, {
                    color = { 0.8, 0.4, 0, 1 },
                    position = { x, y, 0.5 },
                    range = 4,
                })
            end
        end
    end
    local directionalLight = {
        color = { 0.3, 0.3, 0.8, 1 },
        direction = { -0.618, -0.618, -1 },
    }
    engine.UpdateLightsUniformBuffer(directionalLight, pointLights)
end

function engine.End()
    print("Lua Start")
end

local cameraPosition = { 0, 0, 6 }
local targetPosition = { 0, 3, 0 }
local t = 0

function engine.Update()
    print("Lua Update")
    if engine.frameCount == 0 then
        collectgarbage("collect")
    end
    t = t + 0.001
    local distance = gameMath.PingPong(0, length, t * 0.2)
    cameraPosition[1] = distance
    targetPosition[1] = distance
    -- collectgarbage("collect")
    local memoryUsage = collectgarbage("count")
    print("Current memory usage: ", memoryUsage, "KB")
    engine.UpdateGlobalUniformBuffer(cameraPosition, targetPosition, t * 100)
    engine.frameCount = engine.frameCount + 1
end

_G.engine = engine
print("Lua initialized!")
return engine
