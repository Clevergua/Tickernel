local gameState = {};
table.empty = {};

function gameState.Start()
    print("Lua Start");
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
