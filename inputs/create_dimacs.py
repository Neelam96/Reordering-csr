my_file = open('parmat_99999_1000000.el','r')
my_o_file = open('parmat_99999_1000000.dimacs','w')
#875713 5105039
#4847571 68993773
my_o_file.write('p sp 99999 1000000\n')

for line in my_file:
	
	line_arr = line.split()
	
	if line_arr[0][0] == '#':
		continue
	
	src = str(int(line_arr[0])+1)
	dst = str(int(line_arr[1])+1)
	
	my_o_file.write('a '+src+' '+ dst+' 1\n')

