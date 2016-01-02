function getCall()
	local info = debug.getinfo(2, "n")
	if not info then return end
	return info.name
end

function serialize(...)
	local result = ""
	local s = #{...}
	for i, o in pairs{...} do
		if type(o) == "number" then
			result = result .. o
		elseif type(o) == "string" then
			result = result .. string.format("%q", o)
		elseif type(o) == "table" then
			result = result .. "{"
			for k, v in pairs(o) do
				if type(k) == "string" then
					result = result  .. k .. "="
				end
				result = result .. serialize(v)
				result = result .. ","
			end
			result = result .. "}"
		end
		if i ~= s then
			result = result .. ","
		end
	end
	return result
end
