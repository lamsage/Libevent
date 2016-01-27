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
end

function getCall()
	local info = debug.getinfo(2, "n")
	if not info then return end
	return info.name
end

function getMessage(...)
	t = {...}
	proto = dofile(t[#t])
	arg_list = proto.arg_list
	for k, v in pairs(arg_list) do
		if type(t[k]) ~= 'table' then
			GetField(t[k], table.unpack(v))
		else 
			for index, value in pairs(t[k]) do
				GetField(value, table.unpack(v))
			end
		end
	end
end

function getTable(...)
	t = {...}
	proto = dofile(t[#t])
	arg_list = proto.arg_list
	for k, v in pairs(arg_list) do
		if type(t[k]) ~= 'table' then
			GetTable(t[k], table.unpack(v))
		else 
			for index, value in pairs(t[k]) do
				GetTable(value, table.unpack(v))
			end
		end
	end
end

function postMessage(proto)
	local arg_list = proto.arg_list
	local t = {}
	for k, v in pairs(arg_list) do
		if #v == 2 then
			result = PostField(table.unpack(v))
			table.insert(t, result)
		else
			temp = {}
			v[4] = temp
			PostField(table.unpack(v))
			for k,v in pairs(temp) do
				print(k,v)
			end
			table.insert(t, temp)
		end
	end
	print_r(t)
end
