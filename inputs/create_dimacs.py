
import sys
import os


if len(sys.argv) != 4:
         print("Usage: python create_dimacs.py input.el no_of_nodes no_of_edges")
         sys.exit()
 
input_file = sys.argv[1]
output_file = (input_file.split('.')[0])+'.dimacs'
 
if not os.path.isfile(input_file):
         print("Input file {} does not exist. Exiting!".format(input_file))
         sys.exit()
 
 
 
 
my_file = open(input_file,'r')
my_o_file = open(output_file,'w')
 
my_o_file.write('p sp '+sys.argv[2]+' '+sys.argv[3]+' \n')




for line in my_file:
	
	line_arr = line.split()
	
	if line_arr[0][0] == '#':
		continue
	
	src = str(int(line_arr[0])+1)
	dst = str(int(line_arr[1])+1)
	
	my_o_file.write('a '+src+' '+ dst+' 1\n')

