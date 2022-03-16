import csv
import os
import sys

fieldnames = ["test", "stat", "difference", "percentage change"]

def find_trends(base, comp, output, all_csv):
    
    base_file = open(base, mode='r')
    comp_file = open(comp, mode = 'r')
    out_file = open(output, mode = 'a')

    base_csv = list(csv.reader(base_file, delimiter=','))
    comp_csv = list(csv.reader(comp_file, delimiter=','))
    out_csv = csv.DictWriter(out_file, fieldnames = fieldnames, extrasaction='ignore')

    for i in range(1, len(base_csv)): 
        if(base_csv[i][0] == comp_csv[i][0]):
                for stat_indx in range(1, len(base_csv[i])-2):
                    diff = float(comp_csv[i][stat_indx]) - float(base_csv[i][stat_indx])
                    percentage = round(((diff/ float(base_csv[i][stat_indx]))*100), 2)
                    out_csv.writerow({'test': base_csv[i][0], 'stat': base_csv[0][stat_indx], 'difference':'{0:.09f}'.format(diff), 'percentage change':str(percentage)})
                    if(base_csv[0][stat_indx] == 'kbest'):
                        all_csv.writerow({'test': base_csv[i][0], 'stat': base_csv[0][stat_indx], 'difference':'{0:.09f}'.format(diff), 'percentage change':str(percentage)})
                     
    base_file.close()
    comp_file.close()
    out_file.close()


if __name__ == '__main__':

    if len(sys.argv) < 3:
        raise Exception("This script requires two input directories")

    else:
        print("running")
        base_dir = "../results/stats/"+sys.argv[1]
        comp_dir = "../results/stats/"+sys.argv[2]
        out_dir = "../results/comp/"+sys.argv[1]+"/"+sys.argv[2]


        if not os.path.exists(out_dir):
            os.makedirs(out_dir)

        all_file = open(os.path.join(out_dir, "all_tests.csv"), mode = 'a')

        all_csv = csv.DictWriter(all_file, fieldnames = fieldnames, extrasaction='ignore')

        all_csv.writeheader()
        for base_file in os.listdir(base_dir):
            if os.path.exists(os.path.join(comp_dir, base_file)):
                print(base_file)
                out_file = os.path.join(out_dir, base_file.replace("stat", "change"))
            
                find_trends(os.path.join(base_dir, base_file), os.path.join(comp_dir, base_file), out_file, all_csv)

        all_file.close() 



    print("done")
