import csv
import sys
import statistics as stat

INPERCISION = 0.05
K = 5

def kBest(timeArray, size):
    timeArray.sort()
    k_closest = [None] * K

    prev = timeArray[0]
    j = 0
    k_closest[0] = prev
    j+=1
    for i in range(1, size-2):
        curr = timeArray[i]
        if(curr >= 1 or prev >= 1):
            print("[warn] test run greater than 1 second")
            j = 0
        else:
            diff = float(curr - prev)
            ratioDiff = float( diff  / float(prev))
            if(ratioDiff > INPERCISION):
                j = 0
                k_closest = [None] * K
            else:
                k_closest[j] = curr
                j+=1
        if(j == K): break
        prev = curr

    result = k_closest[0]
    return result

def analyze(raw_data):
    raw_file = open(raw_data, mode='r')
    out_file = open("stat.csv", mode = 'a')

    fieldnames = ["test", "avg", "min", "max", "kbest", "variance", "stddev"]

    raw_csv = list(csv.reader(raw_file, delimiter=','))
    out_csv = csv.DictWriter(out_file, fieldnames = fieldnames, extrasaction='ignore')

    file = raw_data.split("/")
    test = file[len(file)-1].split("_")[0]

    two_line_test = ("forkTest" in test or "thread" in test)

    data = []
    data2 = []
    for i in range(2, len(raw_csv)-1):
        line = raw_csv[i]
         
        if(two_line_test):
            data.append(float(line[2]))
            data2.append(float(raw_csv[i+1][2]))
            i+=1
        else:
            data.append(float(line[2]))

    out_csv.writeheader()

    out_csv.writerow({'test': (("parent"+str(test)) if two_line_test else str(test)), 'avg':'{0:.09f}'.format(stat.mean(data)), 'min':'{0:.09f}'.format(min(data)), \
        'max':'{0:.09f}'.format(max(data)), 'kbest': '{0:.09f}'.format(kBest(data, len(data))), 'variance':'{0:.09f}'.format(stat.variance(data)), 'stddev': '{0:.09f}'.format(stat.stdev(data))})

    if(two_line_test):
        out_csv.writerow({'Test':"child"+str(test), 'avg':'{0:.09f}'.format(stat.mean(datai2)), 'min':'{0:.09f}'.format(min(data2)), \
        'max':'{0:.09f}'.format(max(data2)), 'kbest': kBest(data, len(data)), 'variance': '{0:.09f}'.format(stat.variance(data2)), 'stddev': '{0:.09f}'.format(stat.stdev(data2))})
                
            
    raw_file.close()
    out_file.close()      

            
if __name__ == '__main__':

    if len(sys.argv) < 3:
        raise Exception("This script requires an input file and test specification.")

    else:
        analyze(sys.argv[1])

    print("done")
