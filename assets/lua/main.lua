local gameState = require("gameState")
local gameMath = require("gameMath")
local game = require("game")
function gameState.Start()
    print("Lua Start")
    local modelsPath = gameState.assetsPath ..
        gameState.pathSeparator .. "models" .. gameState.pathSeparator;
    local models = {
        gameState.LoadModel(modelsPath .. "LargeBuilding01_0.tvoxel"),
        gameState.LoadModel(modelsPath .. "SmallBuilding01_0.tvoxel"),
        gameState.LoadModel(modelsPath .. "SmallBuilding02_0.tvoxel"),
        gameState.LoadModel(modelsPath .. "SmallBuilding03_0.tvoxel"),
        gameState.LoadModel(modelsPath .. "SmallBuilding04_0.tvoxel"),
        gameState.LoadModel(modelsPath .. "TallBuilding01_0.tvoxel"),
        gameState.LoadModel(modelsPath .. "TallBuilding02_0.tvoxel"),
        gameState.LoadModel(modelsPath .. "TallBuilding03_0.tvoxel"),
    }
    local buildingIndex = 1
    for x = 0, 50 do
        for y = 0, 50 do
            if gameMath.LCGRandom(gameMath.CantorPair(x, y) + 32321) % 100 < 3 then
                buildingIndex = (buildingIndex + 1) % 8;
                gameState.DrawModel(x, y, 0, models[buildingIndex + 1])
            end
        end
    end


    local length = 256
    local width = 256
    game.GenerateWorld(length, width)
    local vertices = {}
    local colors = {}
    local normals = {}
    local stepValue = 0.3
    local pixel = 4

    local rockColor = { 100 / 255.0, 100 / 255.0, 100 / 255.0, 255 / 255.0 }
    local soilColors = {
        { 200 / 255.0, 150 / 255.0, 95 / 255.0, 255 / 255.0 },
        { 150 / 255.0, 200 / 255.0, 100 / 255.0, 255 / 255.0 },
        { 240 / 255.0, 213 / 255.0, 150 / 255.0, 255 / 255.0 },
    }
    local waterColor = { 150 / 255.0, 160 / 255.0, 210 / 255.0, 255 / 255.0 }
    local stoneColor = { 150 / 255.0, 150 / 255.0, 150 / 255.0, 255 / 255.0 }
    local grassColors = {
        { 170 / 255.0, 200 / 255.0, 100 / 255.0, 255 / 255.0 },
        { 191 / 255.0, 178 / 255.0, 106 / 255.0, 255 / 255.0 },
        { 58 / 255.0,  80 / 255.0,  74 / 255.0,  255 / 255.0 },
    }
    local flowerColors = {
        { 200 / 255.0, 170 / 255.0, 100 / 255.0, 255 / 255.0 },
        { 220 / 255.0, 210 / 255.0, 240 / 255.0, 255 / 255.0 },
        { 190 / 255.0, 80 / 255.0,  74 / 255.0,  255 / 255.0 },
    }
    for x = 1, length do
        for y = 1, width do
            for i = 1, pixel do
                for j = 1, pixel do
                    local lx = x * pixel + i - pixel;
                    local ly = y * pixel + j - pixel;
                    -- base rock
                    table.insert(vertices, { lx, ly, 1 })
                    table.insert(colors, rockColor)
                    table.insert(normals, { 0, 0, 0 })

                    local heightNoiseScale = 0.37;
                    local heightNoise = gameMath.PerlinNoise2D(5384, lx * heightNoiseScale,
                        ly * heightNoiseScale)

                    local grassNoiseScale = 0.47
                    local grassNoise = gameMath.PerlinNoise2D(31231, lx * grassNoiseScale,
                        ly * grassNoiseScale)
                    local flowerNoiseScale = 0.37
                    local flowerNoise = gameMath.PerlinNoise2D(2412, lx * flowerNoiseScale,
                        ly * flowerNoiseScale)
                    if game.humidityMap[x][y] < stepValue then
                    else
                        heightNoise = heightNoise - 1.5
                        grassNoise = grassNoise - 0.5
                        flowerNoise = flowerNoise - 1
                        for z = 2, 2 do
                            table.insert(vertices, { lx, ly, z })
                            table.insert(colors, waterColor)
                            table.insert(normals, { 0, 0, 0 })
                        end
                    end
                    local stoneNoiseScale = 0.17

                    local grassHeight = 0
                    if grassNoise > 0.6 then
                        grassHeight = 8
                    elseif grassNoise > 0.5 then
                        grassHeight = 4
                    elseif grassNoise > 0.4 then
                        grassHeight = 2
                    elseif grassNoise > 0.3 then
                        grassHeight = 1
                    else
                        grassHeight = 0
                    end

                    if flowerNoise > 0.4 then
                        table.insert(vertices, { lx, ly, 5 })
                        local flowerIndex = math.abs(gameMath.LCGRandom(gameMath.CantorPair(lx + 512, ly)) % 3) + 1
                        table.insert(colors, flowerColors[flowerIndex])
                        table.insert(normals, { 0, 0, 0 })
                    end

                    for g = 1, grassHeight do
                        table.insert(vertices, { lx, ly, 4 + g })
                        local grassColorIndex = math.abs(gameMath.LCGRandom(gameMath.CantorPair(lx, ly)) % 3) + 1
                        table.insert(colors, grassColors[grassColorIndex])
                        table.insert(normals, { 0, 0, 0 })
                    end
                    local soilColorNoiseScale = 0.037
                    local soilColorNoise = gameMath.PerlinNoise2D(2145125, lx * soilColorNoiseScale,
                        ly * soilColorNoiseScale)
                    local soilColor
                    if soilColorNoise > 0.2 then
                        soilColor = soilColors[1]
                    elseif soilColorNoise > -0.2 then
                        soilColor = soilColors[2]
                    else
                        soilColor = soilColors[3]
                    end
                    if heightNoise < -0.7 then
                        table.insert(vertices, { lx, ly, 2 })
                        table.insert(colors, soilColor)
                        table.insert(normals, { 0, 0, 0 })
                        for k = 3, 5 do
                            local stoneNoise = gameMath.PerlinNoise3D(4236, lx * stoneNoiseScale,
                                ly * stoneNoiseScale, k * stoneNoiseScale)
                            if stoneNoise > 0.5 then
                                table.insert(vertices, { lx, ly, k })
                                table.insert(colors, stoneColor)
                                table.insert(normals, { 0, 0, 0 })
                            end
                        end
                    elseif heightNoise < -0.6 then
                        for z = 2, 3 do
                            table.insert(vertices, { lx, ly, z })
                            table.insert(colors, soilColor)
                            table.insert(normals, { 0, 0, 0 })
                        end
                        for k = 4, 5 do
                            local stoneNoise = gameMath.PerlinNoise3D(4236, lx * stoneNoiseScale,
                                ly * stoneNoiseScale, k * stoneNoiseScale)
                            if stoneNoise > 0.5 then
                                table.insert(vertices, { lx, ly, k })
                                table.insert(colors, stoneColor)
                                table.insert(normals, { 0, 0, 0 })
                            end
                        end
                    else
                        for z = 2, 4 do
                            table.insert(vertices, { lx, ly, z })
                            table.insert(colors, soilColor)
                            table.insert(normals, { 0, 0, 0 })
                        end
                        for k = 5, 5 do
                            local stoneNoise = gameMath.PerlinNoise3D(4236, lx * stoneNoiseScale,
                                ly * stoneNoiseScale, k * stoneNoiseScale)
                            if stoneNoise > 0.5 then
                                table.insert(vertices, { lx, ly, k })
                                table.insert(colors, stoneColor)
                                table.insert(normals, { 0, 0, 0 })
                            end
                        end
                    end
                end
            end
        end
    end

    gameState.SetNormals(vertices, normals);
    local index = gameState.AddModel(vertices, colors, normals);
    local modelScale = 0.0625
    local modelMatrix = {
        { modelScale, 0,          0,          0 },
        { 0,          modelScale, 0,          0 },
        { 0,          0,          modelScale, -4 * modelScale },
        { 0,          0,          0,          1 },
    }
    gameState.UpdateModelUniformBuffer(index, modelMatrix)
end

function gameState.End()
    print("Lua Start")
end

local cameraPosition = { 0, 0, 12 }
local targetPosition = { 32, 32, 0 }
local t = 0;

function gameState.Update()
    print("Lua Update")
    t = t + 0.002
    local distance = gameMath.PingPong(2, 5, t)
    cameraPosition[1] = 32 - distance * math.sin(t)
    cameraPosition[2] = 32 + distance * math.cos(t)
    gameState.SetCamera(cameraPosition, targetPosition)
end

_G.gameState = gameState

print("Lua initialized!")
return gameState
