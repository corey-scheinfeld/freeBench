import csv
import sys

def find_trends(base, comp, outname):
    
    base_file = open(base, mode='r')
    comp_file = open(comp, mode = 'r')
    out_file = open("trend_output-"+outname+".csv", mode = 'w')

    fieldnames = ["Test", "Stat", "Difference", "Percentage Change"]

    base_csv = list(csv.reader(base_file, delimiter=','))
    comp_csv = list(csv.reader(comp_file, delimiter=','))
    out_csv = csv.DictWriter(out_file, fieldnames = fieldnames, extrasaction='ignore')
    
    out_csv.writeheader()
    tested = 0
    for i in range(len(base_csv)):
        if i == 0:
            print("\n"+base_csv[0][0]+" analysis:\n")
        
        elif i == 1:
            comp_str = str("Calculating percentage change in latency from test "+base_csv[1][1]+" to test "+ comp_csv[1][1]+".")
            print(comp_str+"\n")
            out_csv.writerow({'': comp_str})

        else:
            if(base_csv[i][0] == comp_csv[i][0]):
                diff = float(comp_csv[i][1]) - float(base_csv[i][1])
                percentage = round(((diff/ float(base_csv[i][1]))*100), 2)

                res_string = " decreased by " if diff < 0 else " remained constant " if diff == 0 else" increased by "
                test = base_csv[i][0].split()

                if(len(test) ==  3):
                    test[0] = test[0]+"-"+test[1]
                    test[1] = test[2]
                elif(len(test) == 4):
                    test[0] = test[0]+"-"+test[1]+"-"+test[2]
                    test[1] = test[3]


                out_csv.writerow({'Test':test[0], 'Stat':test[1], 'Difference':'{0:.09f}'.format(diff), "Percentage Change":str(percentage)})
                print("Test "+test[0]+" "+test[1]+" latency"+res_string+'{0:.09f}'.format(diff)+", "+str(percentage)+"% \n")
                tested+=1
    
    base_file.close()
    comp_file.close()
    out_file.close()

    print("Completed "+str(tested)+" total comparisons.\n")
    print("Output saved to trend_output-"+outname+".csv")


if __name__ == '__main__':

    if len(sys.argv) < 3:
        raise Exception("This script requires two input files")

    else:
        find_trends(sys.argv[1], sys.argv[2], sys.argv[3])
