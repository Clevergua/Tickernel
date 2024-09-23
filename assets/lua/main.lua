local gameState = require("gameState");

function gameState.Start()
    print("Lua Start");
    local vertices = {
        { 0, 0, 0 },
        { 1, 0, 0 },
        { 2, 0, 0 },
        { 3, 0, 0 },
        { 4, 0, 0 },
        { 5, 0, 0 },
    };
    local colors = {
        { 0.0, 0, 0.5, 1 },
        { 0.1, 0, 0.4, 1 },
        { 0.2, 0, 0.3, 1 },
        { 0.3, 0, 0.2, 1 },
        { 0.4, 0, 0.1, 1 },
        { 0.5, 0, 0.0, 1 },
    };
    local index = gameState.AddModel(vertices, colors);
    local model = {
        { 1, 0, 0, 0 },
        { 0, 1, 0, 0 },
        { 0, 0, 1, 0 },
        { 0, 0, 0, 1 },
    };
    gameState.UpdateModel(index, model);
end

function gameState.End()
    print("Lua Start");
end

function gameState.Update()
    print("Lua Update");
end

_G.gameState = gameState;

print("Lua initialized!");
return gameState;
