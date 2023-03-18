local http = require("socket.http")
local ltn12 = require("ltn12")
local threads = {}
local mutex = require("mutex")
for i = 1, 10 do
    table.insert(threads, coroutine.create(function()
        local lock = mutex.create()
        local url = "https://example.com/file" .. i .. ".txt"
        local filename = "file" .. i .. ".txt"
        local body = {}
        local _, status, headers = http.request{
            url = url,
            sink = ltn12.sink.file(io.open(filename, "wb")),
        }
        lock:lock()
        if status ~= 200 then
            print("Failed to download " .. url .. ": status code " .. status)
        else
            print("Downloaded " .. url)
        end
        lock:unlock()
    end))
end
for _, thread in ipairs(threads) do
    coroutine.resume(thread)
end