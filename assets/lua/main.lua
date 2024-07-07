local engineState = {};
table.empty = {};

function engineState.Start()
    print("Lua Start");
end

function engineState.End()
    print("Lua Start");
end

function engineState.Update()
    print("Lua Update");
end

_G.engineState = engineState;

print("Lua initialized!");
return engineState;
