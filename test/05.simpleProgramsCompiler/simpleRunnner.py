import re
import subprocess
import filecmp
import os
import sys

class COLORS:
    HEADER = '\033[95m'
    OK_BLUE = '\033[94m'
    OK_GREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    END = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'

# list all files in . directory
onlyfiles = [f for f in os.listdir(".") if os.path.isfile(os.path.join(".", f))]

# filter only .input files
regex = re.compile(r"\.imp")
filtered = [i for i in onlyfiles if regex.search(i)]

# sort tests
filtered.sort()

testPassed = 0
testCounter = 0

for i in filtered:
	sys.stdout.write(COLORS.HEADER)
	# somefile.result
	rm = i.replace("imp", "rm")
	# somefile.target
	target = i.replace("imp", "target")
	fNull = open(os.devnull, 'w')	
	testCounter += 1
	
	print("\tTesting: ", i, COLORS.END)
	# assuming main is in the same directory
	cmd = ['./main.exe', i, rm]
	# redirect stdout to somefile.rm
	process = subprocess.Popen(cmd, stdout=fNull, stderr=fNull)
	process.wait()
	
	result = i.replace("imp", "result")
	data = i.replace("imp", "data")
	fIn =  open(data, "r")
	fOut = open(result, "w")
	cmd = ['./emulator.exe', rm]
	process = subprocess.Popen(cmd, stdin=fIn, stdout=fOut, stderr=fNull)
	process.wait()

	# print what you are testing
	if (not os.path.isfile(rm) or not filecmp.cmp(rm, target)): 	# files are not the same
		# keep rm file for reference
		# os.remove(rm)
		sys.stdout.write(COLORS.FAIL)
		print("\tTest: ", i, " failed", COLORS.END)
	else:									# files are the same
		testPassed += 1
		sys.stdout.write(COLORS.OK_GREEN)
		print("\tTest: ", i, " passed", COLORS.END)
		# remove rm file, test passed
		os.remove(rm)