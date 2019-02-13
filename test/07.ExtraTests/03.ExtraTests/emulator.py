import re
import subprocess
import filecmp
import os
import sys

#check arguments
if len(sys.argv) == 1:
    sys.exit()

# print(str(sys.argv))

with open(sys.argv[1]) as f:
    content = f.readlines()

content = [x.strip() for x in content] 

variables = {}
writeValues = []
currentLine = 0

def isInteger(s):
    try:
        int(s)
        return True
    except ValueError:
        return False

def getTab(tab, variables):
    if "(" not in tab:
        return tab
    index = tab[tab.find("(")+1:tab.find(")")]
    if isInteger(index):
        return tab
    value = variables[index]
    tab = tab.replace("(" + index + ")", "(" + str(value) + ")")
    return tab

def getJumpLocation(target, content):
    i = 0
    for line in content:
        if line == target:
            return i
        i+=1
    return -1

simulatedInstructions = 0

while 1:
    line = content[currentLine]
    values = line.split()
    simulatedInstructions+=1
    if (simulatedInstructions > 2000000):
        sys.exit(1)
    if (values[0] == "HALT"):
        break
    elif (values[0] == "CONST"):
        variables[getTab(values[1], variables)] = int(values[2])
        currentLine+=1
    elif (values[0] == "COPY"):
        variables[getTab(values[1], variables)] = variables[getTab(values[2], variables)]
        currentLine+=1
    elif (values[0] == "INC"):
        variables[getTab(values[1], variables)]+= 1
        currentLine+=1
    elif (values[0] == "HALF"):
        variables[getTab(values[1], variables)] = variables[getTab(values[1], variables)] // 2
        currentLine+=1        
    elif (values[0] == "DEC"):
        option = getTab(values[1], variables)
        variables[option] -= 1
        if (variables[option] < 0):
            variables[option] = 0
        currentLine+=1
    elif (values[0] == "ADD"):
        variables[getTab(values[1], variables)]+= variables[getTab(values[2], variables)]
        currentLine+=1
    elif (values[0] == "MUL"):
        variables[getTab(values[1], variables)]*= variables[getTab(values[2], variables)]
        currentLine+=1
    elif (values[0] == "WRITE"):
        writeValues.append(variables[getTab(values[1], variables)])
        currentLine+=1           
    elif (values[0] == "DIV"):
        option = getTab(values[1], variables)
        if (variables[getTab(values[2], variables)] == 0):
            variables[option] = 0
        else:        
            variables[option] = variables[option] // variables[getTab(values[2], variables)]
        currentLine+=1
    elif (values[0] == "MOD"):
        option = getTab(values[1], variables)
        if getTab(values[2], variables) in variables:
            if (variables[getTab(values[2], variables)] == 0):
                variables[option] = 0
            else:        
                variables[option] = variables[option] % variables[getTab(values[2], variables)]
        else:
            variables[option] = variables[option] % 2
        currentLine+=1                    
    elif (values[0] == "SUB"):
        option = getTab(values[1], variables)
        variables[option] -= variables[getTab(values[2], variables)]
        if (variables[option] < 0):
            variables[option] = 0
        currentLine+=1
    elif (values[0] == "JUMP"):
        # print ("JUMP to ", values[1])
        currentLine = getJumpLocation(values[1], content)
    elif (values[0] == "JZERO"):
        # print ("JZERO value=", variables[getTab(values[1], variables)])
        if (variables[getTab(values[1], variables)] == 0):
            currentLine = getJumpLocation(values[2], content)
            # print("JUMPING TO", values[2])
        else:
            currentLine+= 1      
    else:
        currentLine+=1


# print(variables)
# print(writeValues)
# print(content)

# create outputfile
tmpFileName = "tmp2.ir"


f = open(tmpFileName, "w")
f.write("DECLARE\n")
f.write("a;\n")
f.write("IN\n")
for writeValue in writeValues:
    f.write("\tWRITE " + str(writeValue) + ";\n")

if (len(writeValues) == 0):
    f.write("a:=0;\n")
f.write("END\n")

f.close()

fNull = open(os.devnull, 'w')	
cmd = ['./kompilator', tmpFileName, sys.argv[2], "-em"]
# print(cmd)
process = subprocess.Popen(cmd)
process.wait()    
print()

os.remove(tmpFileName)
os.remove(sys.argv[1])

sys.exit(0)




