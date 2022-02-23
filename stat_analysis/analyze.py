import csv
import sys
import statistics as stat

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
        'max':'{0:.09f}'.format(max(data)), 'kbest': 0, 'variance':'{0:.09f}'.format(stat.variance(data)), 'stddev': '{0:.09f}'.format(stat.stdev(data))})

    if(two_line_test):
        out_csv.writerow({'Test':"child"+str(test), 'avg':'{0:.09f}'.format(stat.mean(datai2)), 'min':'{0:.09f}'.format(min(data2)), \
        'max':'{0:.09f}'.format(max(data2)), 'kbest': 0, 'variance': '{0:.09f}'.format(stat.variance(data2)), 'stddev': '{0:.09f}'.format(stat.stdev(data2))})
                
            
    raw_file.close()
    out_file.close()      

            
if __name__ == '__main__':

    if len(sys.argv) < 3:
        raise Exception("This script requires an input file and test specification")

    else:
        analyze(sys.argv[1])

    print("done")
