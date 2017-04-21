import os
import csv

time_tick_ns = 20

def parse(filename):
    # Open files and parse the csv file
    with open("SEMAPHORESHUFFLING_OUTPUT.txt", 'w') as outputfile:
        with open(filename, 'r') as inputfile:
            csvreader = csv.DictReader(inputfile, delimiter=';')
            prevSample = "x"
            state = "Start"
            measure = False
            cntr = 0
            data_list = []
            print "Semaphore-shuffling time" + os.linesep + "Input file: " + filename + os.linesep + "Output file: SEMAPHORESHUFFLING_OUTPUT.txt"
            line = 0
            for row in csvreader:
                line += 1
                if row['Sample'] == "0":
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
                # Third task run until the fourth task is delayed
                elif state == "FourthTaskStarted":
                    if row["ID"] == "3":
                        state = "ThirdTaskRunning"
                elif state == "ThirdTaskRunning":
                    if (prevSample == "3") and (row["ID"] == "0" or row['ID'] == '1' or row['ID'] == '2'):
                        state = "ThirdToFourth"
                elif state == "ThirdToFourth":
                    # Fourth task started -> Wait for semaphore
                    if (prevSample == "0" or prevSample == '1' or prevSample == '2') and (row["ID"] == "4"):
                        state = "FourthTaskWaitForSemaphore"
                    # Third task started -> The fourth task is delayed
                    elif row["ID"] == "3":
                        state = "ThirdTaskRunning"
                # The fourth task is waiting for the semaphore
                elif state == "FourthTaskWaitForSemaphore":
                    if (prevSample == "4") and (row["ID"] == "0" or row['ID'] == '1' or row['ID'] == '2'):
                        state = "SemaphorePended"
                elif state == "SemaphorePended":
                    # Third task running
                    if (prevSample == "0" or prevSample == '1' or prevSample == '2') and (row["ID"] == "3"):
                        state = "SemaphoreBlocking"
                elif state == "SemaphoreBlocking":
                    if (prevSample == "3") and (row["ID"] == "0" or row['ID'] == '1' or row['ID'] == '2'):
                        state = "SemaphoreShufflingMeasure"
                        measure = True
                        # print "Starts @Line: " + str(line)
                elif state ==  "SemaphoreShufflingMeasure":
                    # Fourth task running -> The semaphore released
                    if (prevSample == "0" or prevSample == '1' or prevSample == '2') and (row["ID"] == "4"):
                        measure = False
                        time_us = cntr * time_tick_ns / 1000.0
                        print str(time_us) + " us"
                        # print "Ends @Line: " + str(line)
                        outputfile.write(str(time_us) + " us\n")
                        data_list.append(time_us)
                        cntr = 0
                    # Third task running -> The semaphore not released
                    elif row["ID"] == "3":
                        state = "SemaphoreBlocking"
                        measure = False
                        cntr = 0

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