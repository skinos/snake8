---
title: "Readme for Ubuntu to be misc server"
author: dimmalex@gmail.com
date: February 3, 2024
output:
    word_document:
        path: D:/tmp/install_misc.docx
---


# Install some common servers on ubuntu    

## 1. Ubuntu preset  

```shell
sudo apt-get update
sudo apt-get upgrade
sudo apt-get install -y make git

```

## 2. Language prest   
```shell
sudo dpkg-reconfigure locales                                             # Select the zh_CN.UTF-8 when not UTF8 Code
cat ~/snake8/config/slave/x86/ubuntu2004/bashrc | sudo tee -a ~/.bashrc   # Force to english when not english
```

## 3. Git preset  

```shell
git config --global core.quotepath false
git config --global user.name "ubuntu"
git config --global user.email "ubuntu@gmail.com"
```

## 4. Vim install   

```shell
sudo apt-get install -y vim ctags
cp ~/snake8/config/slave/x86/ubuntu2004/vimrc ~/.vimrc
```

## 5. SSH server install   

```shell
sudo apt-get install -y openssh-server
```

## 6. Samba server install

```shell
sudo apt-get install -y samba samba-common
cat ~/snake8/config/slave/x86/ubuntu2004/smb.conf | sudo tee -a /etc/samba/smb.conf
sudo smbpasswd -a ubuntu
sudo service smbd restart
```

## 7. FTP server install

#### a. install the vsftpd
```shell
sudo apt-get install -y vsftpd
cat ~/snake8/config/slave/x86/ubuntu2004/vsftpd.conf | sudo tee -a /etc/vsftpd.conf
cat cp ~/snake8/config/slave/x86/ubuntu2004/vsftpd.chroot_list /etc
cat cp ~/snake8/config/slave/x86/ubuntu2004/vsftpd.userlist /etc
cat ~/snake8/config/slave/x86/ubuntu2004/shells | sudo tee -a /etc/shells
```

#### b. add the user for vsftpd
```shell
sudo useradd -M -s /bin/false -d /srv/ftp repo
sudo passwd repo
sudo useradd -M -s /bin/false -d /srv/ftp dl
sudo passwd dl
```

#### c. add the directory for vsftpd
```shell
sudo mkdir /srv/ftp/test
sudo mkdir -p /srv/ftp/custom/d218 /srv/ftp/custom/d228 /srv/ftp/custom/d228p /srv/ftp/custom/r600 /srv/ftp/custom/r606 /srv/ftp/custom/r607 /srv/ftp/custom/v520 /srv/ftp/custom/v520b /srv/ftp/custom/v520e
sudo mkdir -p /srv/ftp/pub/d218 /srv/ftp/pub/d228 /srv/ftp/pub/d228p /srv/ftp/pub/r600 /srv/ftp/pub/r606 /srv/ftp/pub/r607 /srv/ftp/pub/v520 /srv/ftp/pub/v520b /srv/ftp/pub/v520e
sudo mkdir -p /srv/ftp/pub/sdk/host/x86/ubuntu
sudo mkdir -p /srv/ftp/pub/sdk/slave/x86/ubuntu2004
sudo mkdir -p /srv/ftp/sdk/mtk2/mt7621/d228 /srv/ftp/sdk/mtk2/mt7621/d228p /srv/ftp/sdk/mtk2/mt7621/r606 /srv/ftp/sdk/mtk2/mt7621/v520 /srv/ftp/sdk/mtk2/mt7621/v520e
sudo mkdir -p /srv/ftp/sdk/mtk2/mt7628/d218 /srv/ftp/sdk/mtk2/mt7628/r600
sudo mkdir -p /srv/ftp/sdk/mtk3/mt7981/r607
sudo chown -R repo:repo /srv/ftp/custom /srv/ftp/pub /srv/ftp/sdk /srv/ftp/test
```

#### d. restart the vsftpd
```shell
sudo service vsftpd restart
```






