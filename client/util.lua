Pto = {}
caller = {}
function print_r ( t )  
	local print_r_cache={}
	local function sub_print_r(t,indent)
		if (print_r_cache[tostring(t)]) then
			print(indent.."*"..tostring(t))
		else
			print_r_cache[tostring(t)]=true
			if (type(t)=="table") then
				for pos,val in pairs(t) do
					if (type(val)=="table") then
						print(indent.."["..pos.."] => "..tostring(t).." {")
						sub_print_r(val,indent..string.rep(" ",string.len(pos)+8))
						print(indent..string.rep(" ",string.len(pos)+6).."}")
					elseif (type(val)=="string") then
						print(indent.."["..pos..'] => "'..val..'"')
					else
						print(indent.."["..pos.."] => "..tostring(val))
					end
				end
			else
				print(indent..tostring(t))
			end
		end
	end
	if (type(t)=="table") then
		print(tostring(t).." {")
		sub_print_r(t,"  ")
		print("}")
	else
		sub_print_r(t,"  ")
	end
	print()
end

function buildField(arg)
	BuildField(table.unpack(arg))
end

function buildProto(proto)
	local arg_list = proto.arg_list
	for k, v in pairs(arg_list) do
		buildField(v)
	end
	return proto
end

function getPto()
	local info = debug.getinfo(2, "n")
	if not info then return end
	return info.name
end

function setMessage(...)
	local t = {...}
	proto = Pto[t[#t]]
	arg_list = proto.arg_list
	for k, v in pairs(arg_list) do
		if type(t[k]) ~= 'table' or (v[2] ~= 'number' and v[2] ~= 'string') then
			if v[3] == nil then
				SetField(t[k], table.unpack(v))
			else
				for index, value in pairs(t[k]) do
					SetField(value, table.unpack(v))
				end
			end
		else
			for index, value in pairs(t[k]) do
				SetField(value, table.unpack(v))
			end
		end
	end
end

function setInnerMessage(...)
	local t = {...}
	proto = Pto[t[#t]]
	arg_list = proto.arg_list
	for k, v in pairs(arg_list) do
		SetInnerField(t[1][v[1]], table.unpack(v))
	end
end

function getMessage(proto)
	local arg_list = proto.arg_list
	local t = {}
	for k, v in pairs(arg_list) do
		if #v == 2 and (v[2] == 'number' or v[2] == 'string') then
			result = GetField(table.unpack(v))
			table.insert(t, result)
		else
			local temp = {}
			table.insert(v, temp)
			GetField(table.unpack(v))
			table.insert(t, temp)
		end
	end
	return table.unpack(t)
end

function getInnerMessage(...)
	local arg = {...}
	local n = #arg
	local arg_list = Pto[arg[n]].arg_list
	if n == 3 then
		for k, v in pairs(arg_list) do
			result = GetInnerField(table.unpack(v))
			table.insert(arg[n-1], result)
		end
	else
		local t = {}
		for k, v in pairs(arg_list) do
			result = GetInnerField(table.unpack(v))
			t[v[1]] = result
		end
		table.insert(arg[n-1], t)
	end
	table.remove(arg, n)
	return table.unpack(arg)
end
