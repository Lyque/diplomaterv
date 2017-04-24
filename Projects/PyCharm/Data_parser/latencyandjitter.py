import os
import csv

time_tick_ns = 20
load_ids = []#["6", "7", "8", "9", "A", "B", "C", "D", "E", "F"]

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
                prevSampleIn = "x"
                prevSampleOut = "x"
                measure = False
                cntr = 0
                data_list = []
                print "Latency and Jitter" + os.linesep + "Input file: " + filename + os.linesep + "Output file: " + outputFilename
                line = 0
                for row in csvreader:
                    line += 1
                    if row['Sample'] == "0" or row["ID"] in load_ids:
                        measure = False
                        cntr = 0
                        prevSampleIn = "x"
                        prevSampleOut = "x"
                        # print "Aborted @Line: " + str(line)
                    elif (not measure) and ((prevSampleIn == '0' and row['IN'] == '1') or (prevSampleIn == '1' and row['IN'] == '0')):
                        measure = True
                        # print "Starts @Line: " + str(line)
                    elif measure and ((prevSampleOut == '0' and row['OUT'] == '1') or (prevSampleOut == '1' and row['OUT'] == '0')):
                        measure = False
                        time_us = cntr * time_tick_ns / 1000.0
                        print str(time_us) + " us"
                        # print "Ends @Line: " + str(line)
                        outputfile.write(str(time_us) + " us\n")
                        data_list.append(time_us)
                        cntr = 0

                    if measure:
                        cntr += 1

                    prevSampleIn = row['IN']
                    prevSampleOut = row['OUT']

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