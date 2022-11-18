import os
import csv

path = "A2FO"
dir_list = os.listdir(path)
mycsv = open('result.csv','a')
wr = csv.writer(mycsv, quoting=csv.QUOTE_ALL)
for j in range(41,55):
    result = []
    i = 0
    for f in dir_list:
        file1 = os.popen(str("./bin/Desdemona ./A2FO/"+dir_list[j]+" ./A2FO/"+f))
        temp = list((file1.read()).split())
        if len(temp)>=2:
            if(dir_list[j] == f):
                temp[0] = 0
                temp[1] = 0
            if temp[0]>temp[1]:
                temp[0] = 64+int(temp[0])
                temp[1] = int(temp[1])
            elif temp[0]<temp[1]:
                temp[1] = 64+int(temp[1])
                temp[0] = int(temp[0])
            result+= [temp[0],temp[1],1000]
        else:
            result += [999,999,1000]
        i += 1
        print((j-2)*55+i)
    wr.writerow([dir_list[j]]+result)
mycsv.close()
file1.close()