import sys
import os

import latencyandjitter
import taskswitchingtime
import preemptiontime
import interruptlatencytime
import semaphoreshufflingtime
import deadlockbreakingtime
import datagramthroughputtime

# Not enough arguments
if len(sys.argv) < 3:
    print "Not enough arguments!" + os.linesep + "python2.7 parse_data.py <input.csv> <measurement>" + os.linesep \
    + os.linesep + "Measurements:" + os.linesep + \
    "\t1: Latency & Jitter" + os.linesep + \
    "\t2: Task switching time" + os.linesep + \
    "\t3: Preemption time" + os.linesep + \
    "\t4: Interrupt latency time" + os.linesep + \
    "\t5: Semaphore-shuffling time" + os.linesep + \
    "\t6: Deadlock-breaking time" + os.linesep + \
    "\t7: Datagram-throughput time"
# Number of arguments if OK
else:
    if not sys.argv[2].isdigit():
        print "The measurement argument is invalid! (Not a number)"
    else:
        if int(sys.argv[2]) > 7 or int(sys.argv[2]) < 1:
            print "The measurement argument is invalid! (Out of range)"
        else:
            input_file = sys.argv[1]
            measurement_number = int(sys.argv[2])
            if measurement_number == 1:
                # Latency & Jitter
                print "Latency & Jitter parsing in progress..."
                latencyandjitter.parse(input_file)
            elif measurement_number == 2:
                # Task switching time
                print "Task switching time parsing in progress..."
                taskswitchingtime.parse(input_file)
            elif measurement_number == 3:
                # Preemption time
                print "Preemption time parsing in progress..."
                preemptiontime.parse(input_file)
            elif measurement_number == 4:
                # Interrupt latency time
                print "Interrupt latency time parsing in progress..."
                interruptlatencytime.parse(input_file)
            elif measurement_number == 5:
                # Semaphore-shuffling time
                print "Semaphore-shuffling time parsing in progress..."
                semaphoreshufflingtime.parse(input_file)
            elif measurement_number == 6:
                # Deadlock-breaking time
                print "Deadlock-breaking time parsing in progress..."
                deadlockbreakingtime.parse(input_file)
            elif measurement_number == 7:
                # Datagram-throughput time
                print "Datagram-throughput time parsing in progress..."
                datagramthroughputtime.parse(input_file)
            else:
                print "Something went wrong with the measurement argument!"