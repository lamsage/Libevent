function getSum(vfd, a, b, c)
	s = a + b + c
	print(s)
	for_caller.c_print(vfd, s)
end

function getMul(vfd, a, b, c)
	s = a * b * c
	print(s)
	for_caller.c_print(vfd, s)
end

function getValue(vfd, key, table)
	for k, v in pairs(table) do
		if k == key then
			print(v)
			for_caller.c_print(vfd, v)
		end
	end
end

function getVfd(vfd)
	print(vfd)
	for_caller.c_print(vfd,vfd)
end

for_caller.s_sum	=	getSum
for_caller.s_mul 	= 	getMul
for_caller.s_getValue 	=	getValue
for_caller.s_getVfd	=	getVfd
