import sys
import re

amount = 0.0
arr = re.split('(\\D+$)', sys.argv[1])[:-1]

if (len(arr) == 2):
	amount = float(arr[0])

if (amount == 0.0):
	print(0)
elif (arr[1] == 'Kb'):
	print(amount*1024)
elif (arr[1] == 'Mb'):
	print(amount*1024*1024)
elif (arr[1] == 'Gb'):
	print(amount*1024*1024*1024)
else: 
	print(amount)
