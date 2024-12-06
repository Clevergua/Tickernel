local profiler = {}
local profilerData = {}

local function hook(event)
    local info = debug.getinfo(2, "nfS")
    if info.name then
        local functionName = string.format("%s (%s:%d)", info.name, info.short_src, info.linedefined)
        if event == "call" then
            if not profilerData[functionName] then
                profilerData[functionName] = {
                    callCount = 0,
                    totalTime = 0,
                }
            end
            profilerData[functionName].startTime = os.clock()
            profilerData[functionName].callCount = profilerData[functionName].callCount + 1
        elseif event == "return" then
            if profilerData[functionName] and profilerData[functionName].startTime then
                local elapsedTime = os.clock() - profilerData[functionName].startTime
                profilerData[functionName].totalTime = profilerData[functionName].totalTime + elapsedTime
            end
        end
    end
end

function profiler.start(maxDepth)
    maxDepth = maxDepth or math.huge
    depth = 1
    debug.sethook(hook, "cr")
end

function profiler.stop()
    debug.sethook()

    local sortedData = {}
    for functionName, data in pairs(profilerData) do
        table.insert(sortedData, { name = functionName, callCount = data.callCount, totalTime = data.totalTime })
    end

    table.sort(sortedData, function(a, b) return a.totalTime > b.totalTime end)

    for _, data in ipairs(sortedData) do
        print(string.format("Function: %s, Calls: %d, Total Time: %.6f",
            data.name, data.callCount, data.totalTime))
    end
    profilerData = {}
end

return profiler
