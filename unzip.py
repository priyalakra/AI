import zipfile
import shutil
import os

path = "A2F"
dir_list = os.listdir(path)

for i in dir_list:
    with zipfile.ZipFile(path+"/"+i, 'r') as unzipObject:
        unzipObject.extractall(path)


