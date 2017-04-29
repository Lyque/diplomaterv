import sys
import os
import re

if len(sys.argv) < 4:
    print "Not enough argument!" + os.linesep + "python27 data_merger.py <input_file_root_name> <index_from> <index_to>"
else:
    filename = sys.argv[1]
    index_from = sys.argv[2]
    index_to = sys.argv[3]
    file_index = index_from
    with open(filename+".txt", 'w') as outputfile:
        with open(filename+file_index+".txt", 'r') as inputfile:
            print "Open file: " + filename + file_index + ".txt"
            for row in inputfile:
                line = re.sub(r"^[ \t]+","", row)
                line = re.sub(r"[ \t]+(?!(ns))",";",line)
                outputfile.write(line)
            print "Finished file: " + filename + file_index + ".txt"

        for index in range(int(index_from)+1,int(index_to)+1):
            with open(filename+str(index)+".txt", 'r') as inputfile:
                print "Open file: " + filename+str(index)+".txt"
                first_row = True
                for row in inputfile:
                    if not first_row:
                        line = re.sub(r"^[ \t]+", "", row)
                        line = re.sub(r"[ \t]+(?!(ns))", ";", line)
                        outputfile.write(line)
                    else:
                        first_row = False
                print "Finished file: " + filename + str(index) + ".txt"