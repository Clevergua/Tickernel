local engine = require("engine")
_G.engine = engine
local gameMath = require("gameMath")
local game = require("game")


local length = 64
local width = 16
local voxelCount = 16
local scale = 1 / voxelCount;
local voxel = {
    dirt = { 150, 90, 40, 255 },
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

-- local floor = game.floor
-- local foundationRoughnessNoiseScaleMap = {
--     { 1.6, 1.6, 0.6 },
--     { 0.6, 1.6, 0.6 },
--     { 2.3, 2.3, 2.3 },
-- }
-- local foundationHeightMap = {
--     { 5, 5, 3 },
--     { 7, 6, 3 },
--     { 3, 7, 7 },
-- }
-- local foundationMap = {
--     { voxel.dirt,         voxel.dirt,         voxel.sand },
--     { voxel.sand,         voxel.dirt,         voxel.sand },
--     { voxel.volcanicRock, voxel.volcanicRock, voxel.volcanicRock },
-- }
-- local surfaceMap = {
--     { voxel.snow, voxel.snow,         voxel.ice },
--     { voxel.sand, voxel.grass,        voxel.water },
--     { voxel.lava, voxel.volcanicRock, voxel.volcanicRock },
-- }
local foundationSeed = 12432
-- local stoneNoiseSeed = 63548
-- local surfaceNoiseSeed = 213456
-- local grassNoiseSeed = 56123
-- local lavaNoiseSeed = 321412


-- function getInterpolationValue(value, step, delta)
--     if value < -step - delta then
--         return 1, 1, 0
--     elseif value < -step then
--         return 1, 2, (value + step + delta) / delta
--     elseif value < step then
--         return 2, 2, 0.5
--     elseif value < step + delta then
--         return 2, 3, (value - step) / delta
--     else
--         return 3, 3, 1
--     end
-- end

-- function getVoxelInterpolation(temperature, humidity, map)
--     local delta = 0.05
--     local x0, x1, dx = getInterpolationValue(temperature, game.temperatureStep, delta)
--     local y0, y1, dy = getInterpolationValue(humidity, game.humidityStep, delta)
--     local r00 = map[x0][y0]
--     local r01 = map[x0][y1]
--     local r10 = map[x1][y0]
--     local r11 = map[x1][y1]
--     local tx = gameMath.SmoothLerp(r00, r01, dx)
--     local ty = gameMath.SmoothLerp(r10, r11, dx)
--     return gameMath.SmoothLerp(tx, ty, dy)
-- end

-- function getVoxel(temperature, humidity, map)
--     if temperature < -game.temperatureStep then
--         if humidity < -game.humidityStep then
--             return map[1][1]
--         elseif humidity < game.humidityStep then
--             return map[1][2]
--         else
--             return map[1][3]
--         end
--     elseif temperature < game.temperatureStep then
--         if humidity < -game.humidityStep then
--             return map[2][1]
--         elseif humidity < game.humidityStep then
--             return map[2][2]
--         else
--             return map[2][3]
--         end
--     else
--         if humidity < -game.humidityStep then
--             return map[3][1]
--         elseif humidity < game.humidityStep then
--             return map[3][2]
--         else
--             return map[3][3]
--         end
--     end
-- end

function engine.start()
    print("Lua Start")

    local modelsPath = engine.assetsPath .. "/"
    print("Loading models..")
    local models = {
        engine.loadModel(modelsPath .. "LargeBuilding01_0.tknvox"),
        engine.loadModel(modelsPath .. "SmallBuilding01_0.tknvox"),
        engine.loadModel(modelsPath .. "SmallBuilding02_0.tknvox"),
        engine.loadModel(modelsPath .. "SmallBuilding03_0.tknvox"),
        engine.loadModel(modelsPath .. "SmallBuilding04_0.tknvox"),
        engine.loadModel(modelsPath .. "TallBuilding01_0.tknvox"),
        engine.loadModel(modelsPath .. "TallBuilding02_0.tknvox"),
        engine.loadModel(modelsPath .. "TallBuilding03_0.tknvox"),
        engine.loadModel(modelsPath .. "Stone_0.tknvox"),
    }
    for index, model in ipairs(models) do
        local count = 15
        if index == 9 then
            count = 20
        end
        local instances = {}
        for i = 1, count do
            local x = math.abs(gameMath.LCGRandom((index + 13251) * 525234532 + i * 42342345)) % length
            local y = math.abs(gameMath.LCGRandom((x - 2317831) * 431513511 + index * 24141312 - i * 2131204)) % width
            local z = 0 * scale
            local angle = 90 * i
            instances[i] = gameMath.createMatrix()
            gameMath.applyTransformations(scale, x, y, z, angle, instances[i])
        end
        engine.drawModel(instances, model)
    end


    local seed = 6345
    -- game.GenerateWorld(seed, length, width)
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

    print("Generating floor foundation..")
    for x = 1, length do
        for y = 1, width do
            for px = 1, voxelCount do
                for py = 1, voxelCount do
                    local dx = (px - (voxelCount + 1) / 2) / voxelCount
                    local dy = (py - (voxelCount + 1) / 2) / voxelCount
                    local foundationHeightNoiseScale = 2.1
                    local foundationHeight = gameMath.perlinNoise2D(foundationSeed,
                        foundationHeightNoiseScale * (x + dx),
                        foundationHeightNoiseScale * (y + dy))
                    foundationHeight = 3 + gameMath.Round(gameMath.Clamp(foundationHeight * 2, -1, 1))
                    local voxelHeightMap = voxelMap[(x - 1) * voxelCount + px][(y - 1) * voxelCount + py]
                    for z = 1, foundationHeight do
                        voxelHeightMap[z] = voxel.dirt
                    end
                end
            end
        end
    end
    -- profiler.stop()
    print("Generating floor surface..")
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
                    if voxelMap[x][y][z] == voxel.water or voxelMap[x][y][z] == voxel.lava then
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
    engine.setNormals(vertices, normals, voxelMap)
    engine.setNormals(waterVertices, waterNormals, voxelMap)
    local model = engine.addModelToOpaqueGeometrySubpass(vertices, colors, normals)
    -- local waterIndex = engine.AddModelToWaterGeometrySubpass(waterVertices, waterColors, waterNormals)
    local modelMatrix = {
        {
            { scale, 0,     0,     0 },
            { 0,     scale, 0,     0 },
            { 0,     0,     scale, -7 * scale },
            { 0,     0,     0,     1 },
        },
    }
    engine.updateInstancesInOpaqueGeometrySubpass(model, modelMatrix)
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
                    range = 2,
                })
            end
        end
    end
    local directionalLight = {
        color = { 0.3, 0.3, 0.8, 0.2 },
        direction = { -0.618, -0.618, -1 },
    }
    engine.updateLightsUniformBuffer(directionalLight, pointLights)
end

function engine.stop()
    print("Lua stop")
end

local globalUniformBuffer = {
    cameraPosition = { 0, 0, 15 },
    cameraRotation = { 0, 0, 0 },
    fov = 30.0,
    near = 0.5,
    far = 32.0,
    time = 0.0,
    frameCount = 0,
    pointSizeScale = 0.618,
}

function engine.update()

    if engine.frameCount == 0 then
        collectgarbage("collect")
        local memoryUsage = collectgarbage("count")
        print("Current memory usage: ", memoryUsage, "KB")
    end
    local cameraPosition = globalUniformBuffer.cameraPosition
    if engine.input[engine.keyCodes.Left] == engine.keyCodeState.UP then
        cameraPosition[1] = cameraPosition[1] - 0.1
    elseif engine.input[engine.keyCodes.Right]== engine.keyCodeState.UP  then
        cameraPosition[1] = cameraPosition[1] + 0.1
    end
    if engine.input[engine.keyCodes.S]== engine.keyCodeState.UP  then
        cameraPosition[3] = cameraPosition[3] + 0.1
    elseif engine.input[engine.keyCodes.W]== engine.keyCodeState.UP  then
        cameraPosition[3] = cameraPosition[3] - 0.1
    end

    if engine.input[engine.keyCodes.Up]== engine.keyCodeState.UP  then
        cameraPosition[2] = cameraPosition[2] + 0.1
    elseif engine.input[engine.keyCodes.Down]== engine.keyCodeState.UP  then
        cameraPosition[2] = cameraPosition[2] - 0.1
    end
    local cameraRotation = globalUniformBuffer.cameraRotation
    if engine.input[engine.keyCodes.D]== engine.keyCodeState.UP  then
        cameraRotation[1] = cameraRotation[1] - 0.01
    elseif engine.input[engine.keyCodes.A]== engine.keyCodeState.UP  then
        cameraRotation[1] = cameraRotation[1] + 0.01
    end
    -- if engine.input[engine.keyCodes.E] == engine.keyCodeState.UP then
    --     engine.loadAudio("laugh.mp3")
    --     local player = engine.getAudioPlayer("laugh.mp3")
    --     engine.setAudioPosition(player, 10000,0,0)
    --     engine.playAudio(player)
    --     engine.unloadAudio("laugh.mp3")
    -- end
    engine.updateGlobalUniformBuffer(globalUniformBuffer)
    engine.frameCount = engine.frameCount + 1

    for index, value in ipairs(engine.input) do
        engine.lastFrameInput[index] = value
    end
end


print("Lua initialized!")
return engine
