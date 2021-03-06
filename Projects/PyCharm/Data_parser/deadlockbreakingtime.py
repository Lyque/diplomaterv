import os
import csv

time_tick_ns = 20
load_ids = ["6", "7", "8", "9", "A", "B", "C", "D", "E", "F"]

def parse(filename):
    # Open files and parse the csv file
    fileSplitted = filename.split('.')
    if len(fileSplitted) != 2:
        print "The input filename is wrong!"
    else:
        outputFilename = fileSplitted[0] + "_OUTPUT.txt"
        with open(outputFilename, 'w') as outputfile:
            with open(filename, 'r') as inputfile:
                csvreader = csv.DictReader(inputfile, delimiter=';')
                prevSample = "x"
                state = "Start"
                measure = False
                cntrTemp = 0
                cntr = 0
                data_list = []
                print "Deadlock-breaking time" + os.linesep + "Input file: " + filename + os.linesep + "Output file: " + outputFilename
                line = 0
                for row in csvreader:
                    line += 1
                    if row['Sample'] == "0" or row["ID"] in load_ids:
                        measure = False
                        cntr = 0
                        prevSample = "x"
                        state = "Start"
                        # print "Aborted @Line: " + str(line)
                    # Task initialization section
                    if state ==  "Start":
                        if row["ID"] == "3":
                            state = "ThirdTaskStarted"
                    elif state == "ThirdTaskStarted":
                        if row["ID"] == "4":
                            state = "FourthTaskStarted"
                    elif state == "FourthTaskStarted":
                        if row["ID"] == "5":
                            state = "FifthTaskStarted"
                    # Third task run until the fourth task is delayed
                    elif state == "FifthTaskStarted":
                        if row["ID"] == "3":
                            state = "ThirdTaskRunning"
                    elif state == "ThirdTaskRunning":
                        if (prevSample == "3") and (row["ID"] == "0" or row['ID'] == '1' or row['ID'] == '2'):
                            state = "ThirdToFourth"
                    elif state == "ThirdToFourth":
                        # Fourth task is running
                        if (prevSample == "0" or prevSample == '1' or prevSample == '2') and (row["ID"] == "4"):
                            state = "FourthTaskRunning"
                        # Third task is running
                        elif row["ID"] == "3":
                            state = "ThirdTaskRunning"
                    # The fourth task is running
                    elif state == "FourthTaskRunning":
                        if (prevSample == "4") and (row["ID"] == "0" or row['ID'] == '1' or row['ID'] == '2'):
                            state = "FourthToFifth"
                    elif state == "FourthToFifth":
                        if (prevSample == "0" or prevSample == '1' or prevSample == '2') and (row["ID"] == "5"):
                            state = "FifthTaskWaitForMutex"
                    elif state == "FifthTaskWaitForMutex":
                        if (prevSample == "5") and (row["ID"] == "0" or row['ID'] == '1' or row['ID'] == '2'):
                            state = "DeadlockBreakingMeasure"
                            measure = True
                            # print "Starts @Line: " + str(line)
                    elif state ==  "DeadlockBreakingMeasure":
                        if (prevSample == "0" or prevSample == '1' or prevSample == '2') and (row["ID"] == "3"):
                            # Pause counting
                            state = "BlockingTaskRunning"
                            measure = False
                    elif state == "BlockingTaskRunning":
                        if (prevSample == "3") and (row["ID"] == "0" or row['ID'] == '1' or row['ID'] == '2'):
                            state = "DeadlockBreakingSecondPart"
                            measure = True
                            cntrTemp = cntr
                    elif state == "DeadlockBreakingSecondPart":
                        if (prevSample == "0" or prevSample == '1' or prevSample == '2') and (row["ID"] == "5"):
                            measure = False
                            time_us = cntr * time_tick_ns / 1000.0
                            print str(time_us) + " us"
                            # print "Ends @Line: " + str(line)
                            outputfile.write(str(time_us) + " us\n")
                            data_list.append(time_us)
                            cntr = 0
                        elif (prevSample == "0" or prevSample == '1' or prevSample == '2') and (row["ID"] == "3"):
                            state = "BlockingTaskRunning"
                            measure = False
                            cntr = cntrTemp

                    if measure:
                        cntr += 1

                    prevSample = row['ID']


                data_length = len(data_list)
                sum_value = sum(data_list)
                mean_value = sum_value/data_length
                deviation_value = (sum([(_-mean_value)**2 for _ in data_list])/(data_length-1))**0.5

                outputfile.write(os.linesep)
                outputfile.write("Number of samples: " + str(data_length) + "\n")
                outputfile.write("Mean= " + str(mean_value) + " us\n")
                outputfile.write("Deviation= " + str(deviation_value) + " us\n")

                print
                print "Number of samples: " + str(data_length)
                print "Mean= " + str(mean_value)
                print "Deviation= " + str(deviation_value)