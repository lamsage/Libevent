caller.c_print = Call
function test(fd, x, y, z)
	print(fd, x, y, z)
	caller.c_print(fd, x + y)
end
caller.s_sum = test
function test2(fd, i, j)
	print(fd, i, j)
	caller.c_print(fd, i - j)
end
caller.s_test = test2
function test3(fd)
	print(fd)
	caller.c_print(fd, fd)
end
caller.s_vfd = test3
function test4(fd, s1, i, s2 )
	print(s1, i, s2)
	for k, v in pairs(s2) do
		print(k, v)
	end
end
caller.s_string = test4
