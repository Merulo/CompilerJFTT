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
regex = re.compile(r".input")
filtered = [i for i in onlyfiles if regex.search(i)]

# sort tests
filtered.sort()

testPassed = 0
testCounter = 0


testsToRun = -1
ignoreWhiteDiff = False
for i in range(1, len(sys.argv)):
	if sys.argv[i] == "-iw":
		ignoreWhiteDiff = True
	elif sys.argv[i] == "-n":
		testsToRun = int(sys.argv[i+1])
		print(testsToRun)

os.remove("log.txt")

for i in filtered:
	sys.stdout.write(COLORS.HEADER)
	# somefile.result
	result = i.replace("input", "result")
	# somefile.target
	target = i.replace("input", "target")
	fOut = open(result, "w")
	fIn = open(i)
	testCounter += 1
	
	print("Testing: ", i, COLORS.END)
	# assuming main is in the same directory
	# ./main.exe
	cmd = ['./main.exe']
	# redirect stdout to somefile.result
	process = subprocess.Popen(cmd, stdin=fIn, stdout=fOut)
	process.wait()

	if ignoreWhiteDiff: #ignoring whitespace diffs
		log = open("logfile", "w")
		diff = ['diff', str(result), str(target), '-w', '-B']
		p = subprocess.Popen(diff, stdout=log)
		p.wait()
		if (os.stat("logfile").st_size != 0): 	# files are not the same
			# keep result file for reference
			# os.remove(result)
			sys.stdout.write(COLORS.FAIL)
			print("Test: ", i, " failed", COLORS.END)
		else:					# files are the same
			testPassed += 1
			sys.stdout.write(COLORS.OK_GREEN)
			print("Test: ", i, " passed", COLORS.END)
			# remove result file, test passed
			os.remove(result)
		os.remove("logfile")
	else: #not ignoring whitespace diffs
		# print what you are testing
		if (not filecmp.cmp(result, target)): 	# files are not the same
			# keep result file for reference
			# os.remove(result)
			sys.stdout.write(COLORS.FAIL)
			print("Test: ", i, " failed", COLORS.END)
		else:									# files are the same
			testPassed += 1
			sys.stdout.write(COLORS.OK_GREEN)
			print("Test: ", i, " passed", COLORS.END)
			# remove result file, test passed
			os.remove(result)

	if testCounter == testsToRun:
		sys.exit()

print("Passed: ", testPassed, " out of ", testCounter)
