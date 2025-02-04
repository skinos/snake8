---
title: "Readme for Ubuntu to be Server"
author: dimmalex@gmail.com
date: February 3, 2024
output:
    word_document:
        path: D:/tmp/Readme.docx
---


# Skinos Server download and install

#### 1. SDK download   
Under Ubuntu ( 20.04 or 18.04 recommended ) run the following command to download the SDK ( please install git and make first )   

```shell
git clone https://github.com/skinos8/snake8.git
```

*Or download it from gitee*

```shell
git clone https://gitee.com/snake8/snake8.git
```

#### 2. Install the necessary development tools   

```shell
cd snake8
make preset
```

#### 3. Specify the model to be installed   

```shell
make pid gBOARDID=<server model>

# For products such as Ubuntu enter the following instructions:
make pid gBOARDID=slave-x86-ubuntu2004
```

#### 4. Download the SDK corresponding   

```shell
make update
```

#### 5. Compile   

```shell
make dep
make
```


#### 6. Install   

```shell
make install
```


#### 7. Run   

```shell
make start
```
