
# SDK Tools Introduction   
read/install/build operations of FPK are provided during the compilation or development process   


### prj   
Template creation for the project programming   
```
dimmalex@HMS:~/snake8$ ./config/host/tools/prj
argument help:
      prj create     <project name>                                 create a project template
      prj delete     <project name>                                 delete the project
      prj add_com    <project name> <component name>                create a component template
      prj add_exe    <project name> <execute name>                  create a execute program template
      prj add_lib    <project name> <library name>                  create a library template
      prj add_ko     <project name> <kernel module name>            create a linux kernel module template
      prj add_init   <project name> <init level>   <API>            register a component api at system setup
      prj add_uninit <project name> <uninit level> <API>            register a component api at system shutdown
      prj add_joint  <project name> <joint event>  <API>            register a component api at joint event cast
      prj add_object <project name> <object name> <component name>  create a dynamic component depend on exist component
      prj add_wui    <project name> <webpage name>                  create a web page html template for project
      prj check      <project name>                                 check the a project json format
dimmalex@HMS:~/snake8$
```  

### prj-read   
Get information about the project  
- **FPK information Key** nodes in the configuration file  
- **FPK pathname** is the path of the configuration file, default is prj.info   
```
dimmalex@HMS:~/snake8$ ./config/host/tools/prj-read
argument help: [ <FPK information key>, [FPK pathname] ]
dimmalex@HMS:~/snake8$
```   

### fpk-install    
Install the FPK package   
- **root filesystem** is the root directory of the running system     
- **install directory** is the installation directory of the header file and library   
- **fpk package/directory** fpk package or directory need be install   
```   
dimmalex@HMS:~/snake8$ ./config/host/tools/fpk-install
argument help: [ <root filesystem>, <install directory>  <fpk package/directory, [...] > ]
dimmalex@HMS:~/snake8$
```   

### fpk-indexed   
Make the software repo index file   
- **index file** will be create for repo  
- **FPK directory** is the directory full of FPK file   
```   
dimmalex@HMS:~/snake8$ ./config/host/tools/fpk-indexed
argument help: [ <index file>, <fpk directory> ]
dimmalex@HMS:~/snake8$
```   


### firmware-encode   
Encode the firmware file
- **target file** is the file be encode  
- **source file** is the original file
```   
dimmalex@HMS:~/snake8$ ./config/host/tools/firmware-encode
argument help: [ <target file>, <source file, [...]> ]
dimmalex@HMS:~/snake8$
``` 

### firmware-log   
Create a firmware log from current SDK state  
- **output log pathname**  firmware log file will be create 
- **version** current version identify
- **model** current firmware model identify
- **scope**  current firmware scope identify 
- **upgrade file** current firmware file  
- **source log pathname** SDK log file  
```   
dimmalex@HMS:~/snake8$ ./config/host/tools/firmware-log 
argument help: [ <output log pathname>, <version>, <model>, <scope>, <upgrade file>, <source log pathname, [...] > ]
dimmalex@HMS:~/snake8$ 
```


### repo-upload   
Upload the FPK or firmware to repo from SDK   
- **platform** firmware platform identify   
- **hardware** firmware hardware identify   
- **custom** firmware custom identify   
- **scope** firmware scope identify   
- **dir**  firmware file directory
- **file**  firmware file
```   
dimmalex@HMS:~/snake8$ ./config/host/tools/repo-upload
argument help:
	repo-upload <platform>, <hardware>, <custom>, <scope>, <dir>, fpk        upload all fpk of <dir> to repo
	repo-upload <platform>, <hardware>, <custom>, std, <dir>, <file>         upload file of <dir> to standard repo
	repo-upload <platform>, <hardware>, <custom>, <scope>, <dir>, <file>     upload file of <dir> to custom repo
dimmalex@HMS:~/snake8$
```   

### repo-update   
Update the FPK or source file from repo   
- **platform** update the platfrom identify   
- **hardware** update the hardware identify   
- **custom** update the custom identify   
- **file** update the file   
```   
dimmalex@HMS:~/snake8$ ./config/host/tools/repo-update
argument help:
	repo-update <platform>, <hardware>, <custom>, fpk		update all fpk from repo
	repo-update <platform>, <hardware>, <custom>, <file>		update file from repo
	repo-update <platform>, <hardware>, <custom>, <file>		update file from repo
dimmalex@HMS:~/snake8$
```   
