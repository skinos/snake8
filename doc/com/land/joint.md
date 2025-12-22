
***

## Management of joint event component
Administration of equipment joint event task
Each project can register its own joint event task (either through the project production tool or directly by modifying the prj.json file), which requires the system to execute the task when joint event happen

There are multiple joint event, you can register a task for executed when joint event happen:

1. system event
- machine/status
- date/modify

2. ifname ipv4 up event
- network/on
- network/onvpn
- network/onextern
- network/online

3. ifname ipv4 down event
- network/off
- network/offvpn
- network/offextern
- network/offline

4. ifname ipv6 up event
- network/up
- network/upvpn
- network/upextern
- network/upline

5. ifname ipv6 down event
- network/down
- network/downvpn
- network/downextern
- network/downline

6. client event
- station/appear
- station/disappear



#### **Configuration( land@joint )**

```json
// Attributes introduction 
{
    "task name":                             // [ string ], you can custom the name
    {
        "joint":"joint event name",                 // [ string ]
        "call":"component API or program"           // [ string ]
    }
    // "...":{ ... }     How many joint event task show how many properties
}
// Examples
{
    "online_talk_to_me":                               // execute tui@telnet.reset when the network/online joint event happen
    {
        "joint":"network/onlne",
        "call":"tui@telnet.reset"
    },
    "offline_talk_to_me":                              // execute tui@ssh.shut when the network/onliofflinene joint event happen
    {
        "joint":"network/offline",
        "call":"tui@ssh.shut"
    }
}
```  



#### **API( land@joint )** 


+ `register[ joint event name, call ]` **register a joint event task, lost when reboot**  
    - joint event name ----------- [ string ] 
    - call ----------------------- [ string ], component API or program 
    - failed return tfalse
    - succeed return ttrue

    Example, register executing wui@admin.reset when the network/online joint event happen
    ```shell
    land@joint.register[ network/online, wui@admin.reset ]
    ttrue
    ```
    Example, register executing tui@ssh.reset when the network/up joint event happen
    ```
    land@joint.register[ network/up, tui@ssh.reset ]
    ttrue    
    ```   

+ `unregister[ joint event name, call ]` **delete a joint event task**  
    - joint event name ------- [ string ]
    - call ------------------- [ string ], component API or program
    - failed return tfalse
    - succeed return ttrue

    Example, delete executing tui@ssh.reset when network/up joint event happen
    ```shell
    land@joint.unregister[ network/up, tui@ssh.reset ]
    ttrue
    ```

+ `list[ [joint event name] ]` **list joint event task**  
    - joint event name ----------- [ string ], default will list all registered joint event task in the system
    - return json to describes the event task   
    ```json
    // Attributes introduction of json by the method return
    {
        "joint event name":                 // [ string ]:{},  joint event name 
        {
            "component API or program":"",
            "component API or program":""
            // "...":"..."     How many executing at that joint event show how many properties
        }
        // "...":{ ... }     How many joint event name show how many properties
    }    
    ```  

    Example, show all the joint event task
    ```shell
    land@joint.list
    {
        "storage/insert":
        {
            "land@syslog.setup":""
        },
        "storage/remove":
        {
            "land@syslog.setup":""
        },
        "network/online":
        {
            "agent@heclient.setup":"",
            "client@dhcps.reset":""
        },
        "machine/status":
        {
            "agent@heclient.update":""
        },
        "network/onextern":
        {
            "arch@ethernet.hwnat":"",
            "forward@rule.on":"",
            "forward@nat.on":"",
            "forward@firewall.on":""
        },
        "network/on":
        {
            "client@dhcps.reset":"",
            "forward@rule.on":"",
            "forward@dnat.on":""
        }
        // ... more joint event
    }
    ```  

    Example, show the network/online joint event task 
    ```shell
    land@joint.list[ network/online ]
    {
        "agent@heclient.setup":"",
        "client@dhcps.reset":""
    }   
    ```



+ `add[ task name, call, joint event name ]` **add a joint event task**
    - task name ------------- [ string ], task name, you can custom the name
    - call ------------------ [ string ], component API or program
    - joint event name ------ [ string ]
    - failed return tfalse
    - succeed return ttrue

    Example, add a task named webreset, that executing wui@admin.reset when the network/online joint event happen 
    ```shell
    land@joint.add[ webreset, wui@admin.reset, network/online ]
    ttrue
    ```
    Example, add a task named sshreset, that executing tui@ssh.reset when the network/up joint event happen 
    ```
    land@joint.add[ sshreset, tui@ssh.reset, network/up ]
    ttrue    
    ```

+ `delete[ task name ]` **delete a joint event task**
    - task name ---- [ string ], task name
    - failed return tfalse
    - succeed return ttrue

    Example, delete task named webreset
    ```shell
    land@joint.delete[ webreset ]
    ttrue
    ```

