
## Syslog Management   
Manage system syslog, object name is land@syslog

#### Configuration( land@syslog )   
```json
// Attributes introduction 
{
    "status":"syslog status",          // [ "disable", "enable", "tui", "file", "both" ]
                                            // "enable" for syslog
                                            // "disable" for no log
                                            // "tui" for output to terminal
                                            // "both" for output to terminal and syslog
                                            // "file" for output to file
    "trace":"record code information", // [ "disable", "enable" ]

    "level":"log level",               // [ "verb", "debug", "info", "warn", "fault" ]
                                            // "verb for debug business data 
                                            // "debug for debug code
                                            // "info for normal infomation
                                            // "warn for warnningr infomation
                                            // "fault for error infomation
    "fault":"debug type",              // [ string ], Multiple type with ";" separation, vaild when "level" no exist
    "warn":"warn type",                // [ string ], Multiple type with ";" separation, vaild when "level" no exist 
    "info":"info type",                // [ string ], Multiple type with ";" separation, vaild when "level" no exist 
    "debug":"debug type",              // [ string ], Multiple type with ";" separation, vaild when "level" no exist , ex. "arch@usb;arch@pci"
    "verb":"verb type",                // [ string ], Multiple type with ";" separation, vaild when "level" no exist 

    "klog":"kernel log status",         // [ "disable", "enable" ]
    "size":"log size",                  // [ number ], The unit is kbyte. logs will be overwritten when they exceed the size of the recording pool
    "location":"location of logs",      // [ "memory", "interval", "storage", "sdaxxx" ]
                                            // "memory" for stored in the memory and will be lost upon restart
                                            // "interval" for stored in the internal flash. Strong columns are not recommended in the internal program area, which may cause corruption  
                                            // "storage" for stored in external storage
                                            // "sdaxxx" for sda starts with a specified mount disk
                                            // stored in xxxx-syslog.txt file in the root directory when "location" is internal, storage, starts with sda
    "remote":"Remote Log server",     // [ string ], If this parameter is left blank, the remote log server is disabled
    "port":"Remote Log port"          // [ number ], default is 514
}
```

Examples, show all the syslog configure   
```shell
land@syslog
{
    "status":"enable",                # enable the syslog functions
    "location":"memory",              # logs stored in memory, restart will be lost
    "type":"arch|land|default",       # log the arch, land, default log
    "level":"info",                   # log level is normal infomation
    "trace":"disable",                # disable the code information
    "size":"100",                     # log buffer is 100k
    "remote":"192.168.8.250",         # send the syslog to remote server 192.168.8.250
    "port":"514"                      # send the syslog to remote server port 514
}
```  

Examples, set the remote syslog server be 112.43.230.74   
```shell
land@syslog:remote=112.43.230.74
```  

Examples, clear the remote syslog server
```shell
land@syslog:remote=
```  

Examples, set the syslog level to debug 
```shell
land@syslog:level=debug
```  





#### **Methods**

+ `path[]` **show log location and max size**, *succeed return talk, failed return NULL, error return terror*   
    ```json
    // Attributes introduction of talk by the method return
    {
        "path":"log file pathname",   // [ string ]
        "size":"log file size limit"  // [ number ], the unit is Kbyte
    }    
    ```

    ```shell
    # examples, show current log file pathname and size limit
    land@syslog.path
    {
        "path":"/var/3618760113-syslog.txt",    # current the syslog save at /var/3618760113-syslog.txt
        "size":"100"                            # file limit is 100k
    }
    ```

+ `clear[]` **clear the log**, *succeed return ttrue, failed return tfalse, error return terror*   
    ```shell
    # examples
    land@syslog.clear
    ttrue
    ```

+ `show[]` **show the log**, *succeed return ttrue, failed return tfalse, error return terror*   
    ```shell
    # examples
    land@syslog.show
    May 12 01:58:16 ASHYELF-12ADB0 daemon.debug modem@lte2: 6 >>66>> (AT+QNWINFO^M^M +QNWINFO: "FDD LTE","46001","LTE BAND 3",1650^M ^M OK^M )
    May 12 01:58:16 ASHYELF-12ADB0 daemon.debug modem@lte2: 6 <<< (at+qeng="servingcell"^M )
    May 12 01:58:16 ASHYELF-12ADB0 daemon.debug modem@lte2: 6 >>22>> (at+qeng="servingcell"^M)
    May 12 01:58:16 ASHYELF-12ADB0 daemon.debug modem@lte2: 6 <<< (AT+CREG?^M )
    May 12 01:58:16 ASHYELF-12ADB0 daemon.debug modem@lte2: 6 >>9>> (AT+CREG?^M)
    May 12 01:58:16 ASHYELF-12ADB0 daemon.debug modem@lte2: 6 >>48>> (AT+CREG?^M^M +CREG: 2,1,"2604","6DA5A09",7^M ^M OK^M )
    May 12 01:58:16 ASHYELF-12ADB0 daemon.debug modem@lte2: 6 <<< (AT+CSQ^M )
    May 12 01:58:16 ASHYELF-12ADB0 daemon.debug modem@lte2: 6 >>28>> (AT+CSQ^M^M +CSQ: 18,99^M ^M OK^M )    
    ttrue
    ```

+ `info[]` **record a level of info log**, *succeed return ttrue, failed return tfalse, error return terror*   
    ```shell
    # examples
    land@syslog.info[ "this is log for info level" ]
    ttrue
    ```

+ `warn[]` **record a level of warn log**, *succeed return ttrue, failed return tfalse, error return terror*   
    ```shell
    # examples
    land@syslog.info[ "this is log for warn level" ]
    ttrue
    ```

+ `fault[]` **record a level of fault log**, *succeed return ttrue, failed return tfalse, error return terror*   
    ```shell
    # examples
    land@syslog.info[ "this is log for fault level" ]
    ttrue
    ```
