import csv
import sys
import statistics as stat

def analyze(raw_data):
    raw_file = open(raw_data, mode='r')
    out_file = open("stat.csv", mode = 'w')

    fieldnames = ["test", "avg", "min", "max", "kbest", "variance", "stddev"]

    raw_csv = list(csv.reader(raw_file, delimiter=','))
    out_csv = csv.DictWriter(out_file, fieldnames = fieldnames, extrasaction='ignore')

    test = "ref"
    iterations = 0
    two_line_test = False

    data = []
    data2 = []
    for i in range(1, len(raw_csv)):
        line = raw_csv[i]

        if(test != line[0].split(" ")[0]):
            test = line[0].split(" ")[0]
            
            if(test == "forkTest" or test == "theadTest"):
                two_line_test = True
                out_csv.writerow({'Test':test+"parent", 'avg':stat.mean(data), 'min':min(data), 'max':max(data), 'kbest': 0, 'variance': stat.variance(data), 'stddev': stat.stdev(data)})
                out_csv.writerow({'Test':test+"child", 'avg':stat.mean(data2), 'min':min(data2), 'max':max(data2), 'kbest': 0, 'variance': stat.variance(data2), 'stddev': stat.stdev(data2)})
            else:
                two_line_test = False
                out_csv.writerow({'Test':test, 'avg':stat.mean(data), 'min':min(data), 'max':max(data), 'kbest': 0, 'variance': stat.variance(data), 'stddev': stat.stdev(data)})
                
            iterations = 0
            data = []
            data2 = []
            i+=1
        elif(two_line_test):
            data[iterations] = float(line[2])
            data2[iterations] = float(raw_csv[i+1][2])
            i+=1
            iterations+=1
        elif(len(line[0].splt(" ")) == 2):
            data[iterations] = float(line[2])
            iterations +=1
        
    raw_file.close()
    out_file.close()      

            
if __name__ == '__main__':

    if len(sys.argv) < 2:
        raise Exception("This script requires an input file")

    else:
        analyze(sys.argv[1])

    print("done")
