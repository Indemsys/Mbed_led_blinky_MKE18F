import os
import string

prog_file_name = "Mbed_led_blinky.ewp"
proj_dir = os.getcwd() # Файл должен находится в корневой директории проекта


def removeEmptyFolders(path, removeRoot=True):
  'Function to remove empty folders'
  if not os.path.isdir(path):
    return

  # remove empty subfolders
  files = os.listdir(path)
  if len(files):
    for f in files:
      fullpath = os.path.join(path, f)
      if os.path.isdir(fullpath):
        removeEmptyFolders(fullpath)

  # if folder empty, delete it
  files = os.listdir(path)
  if len(files) == 0 and removeRoot:
    print ("Removing empty folder:", path)
    os.rmdir(path)





with open(os.path.join(proj_dir,prog_file_name)) as pfile:
  proj_file = pfile.read()

# Удаляем все .c .cpp .s .a .json файлы, которые не используются в проекте IAR
for dirpath, dirs, files in os.walk(proj_dir): 
  for filename in files:
    rname = os.path.join(dirpath,filename)
    path = dirpath.replace(proj_dir,'$PROJ_DIR$') 
    fname = os.path.join(path,filename)
    if fname.endswith('.c') or fname.endswith('.cpp') or fname.endswith('.s') or fname.endswith('.S') or fname.endswith('.a') or fname.endswith('.json'):
      if fname not in proj_file:
        print("Removing unused folder:", fname)
        os.remove(rname)

# Удаляем ставшие пустыми после удаления файлов директории
removeEmptyFolders(proj_dir)        
        

      
      
          
