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
	# sys.stdout.write(COLORS.HEADER)
	# somefile.result
	rm 		= i.replace(".imp", ".rm")
	result 	= i.replace(".imp", ".result")
	target 	= i.replace(".imp", ".target")
	data 	= i.replace(".imp", ".data")
	# somefile.target
	fNull = open(os.devnull, 'w')	
	testCounter += 1

	if os.path.isfile(rm):
		os.remove(rm)
	if os.path.isfile(result):
		os.remove(result)

	# print("\tTesting: ", i, COLORS.END)
	# assuming main is in the same directory
	cmd = ['./main.exe', i, rm]
	# redirect stdout to somefile.rm
	process = subprocess.Popen(cmd, stdout=fNull, stderr=fNull)
	process.wait()
	
	fIn = open(os.devnull, 'r')
	if os.path.isfile(data):
		fIn =  open(data, "r")
	fOut = open(result, "w")
	cmd = ['./emulator.exe', rm]
	process = subprocess.Popen(cmd, stdin=fIn, stdout=fOut, stderr=fNull)
	process.wait()
	fOut.close()


	fResult = open(result, "r")
	fTarget = open(target, "r")

	resultInts = []
	for val in fResult.read().split():
		resultInts.append(int(val))
	fResult.close()

	targetInts = []
	for val in fTarget.read().split():
		targetInts.append(int(val))
	fTarget.close()

	same = True
	if (len(resultInts) != len(targetInts)):
		same = False
	else:
		for j in range(len(resultInts) - 1):
			if (resultInts[j] != targetInts[j]):
				same = False


	# print what you are testing
	if (not same): 	# files are not the same
		# keep rm file for reference
		# os.remove(rm)
		sys.stdout.write(COLORS.FAIL)
		print("\t\tTest: ", i, " failed", COLORS.END)
	else:									# files are the same
		testPassed += 1
		if (resultInts[-1] > targetInts[-1]):
			sys.stdout.write(COLORS.OK_GREEN)
			# print("\t\tTest: ", i, " passed")
			sys.stdout.write(COLORS.FAIL)
			print("\t\tTest: ", i," increased cost from", targetInts[-1], " to", resultInts[-1], COLORS.END)
		# remove rm file, test passed
		else:
			# os.remove(rm)
			# os.remove(result)
			k = 10
percent = testPassed/float(testCounter)*100
percent = round(percent, 0)
sys.stdout.write(COLORS.OK_BLUE)
print("\t\tPassed:", percent, "%", COLORS.END)