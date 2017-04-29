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
                measure = False
                cntr = 0
                data_list = []
                fromID = "x"
                print "Task switching time" + os.linesep + "Input file: " + filename + os.linesep + "Output file: " + outputFilename
                line = 0
                for row in csvreader:
                    line += 1
                    if row['Sample'] == "0" or row["ID"] in load_ids:
                        measure = False
                        cntr = 0
                        prevSample = "x"
                        # print "Aborted @Line: " + str(line)
                    elif (not measure) and (prevSample == '3' or prevSample == '4' or prevSample == '5') and (row['ID'] == '0' or row['ID'] == '1' or row['ID'] == '2'):
                        measure = True
                        fromID = prevSample
                        # print "Starts @Line: " + str(line)
                    elif measure and (prevSample == '0' or prevSample == '1' or prevSample == '2') and (row['ID'] == '3' or row['ID'] == '4' or row['ID'] == '5'):
                        measure = False
                        # If the switch is real
                        if fromID != row["ID"]:
                            time_us = cntr * time_tick_ns / 1000.0
                            if time_us > 2:
                                print str(time_us) + " us"
                                # print "Ends @Line: " + str(line)
                                outputfile.write(str(time_us) + " us\n")
                                data_list.append(time_us)
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