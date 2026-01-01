
***
## LTE/NR SMS Management
Management LTE modem SMS

#### Configuration( modem@sms )
**modem@sms** is first LTE modem SMS
**modem@sms2** is second LTE modem SMS

```json
// Attributes introduction 
{
    "center":"SMS center number",                             // [ number ]
    "he":"enable or disable the he command",                  // [ "disable", "enable" ]
    "he_contact":"set a contact to send the he command",      // [ number ]
    "he_prefix":"set a prefix at the he command"              // [ string ]
}
```

#### **API**
**modem@sms** is first LTE modem SMS
**modem@sms2** is second LTE modem SMS


+ `send[ contact, content ]` **send a SMS**
    - contact ----------- [ string ], contact number
    - content ----------- [ string ], text of SMS     
    - failed return tfalse
    - error return terror    
    - succeed return ttrue
    
    Example, send a "i miss you" to 17688704240
    ```shell
    modem@sms.send[ 8617688704240, i\ miss\ you ]
    ttrue
    ```

+ `list[]` **list all received SMS**  
    - failed return NULL
    - error return terror    
    - succeed return json to describes SMS list   
    ```json
    // Attributes introduction of json by the method return
    {
        "SMS Identify name":           // [ string ]:{}
        {
            "id":"SMS Identify name",    // [ string ]
            "contact":"SMS sender",      // [ string ]
            "date":"receipt time",       // [ string ], format is "YY-MM-DD HH-MM-SS"
            "content":"SMS content"      // [ string ], UTF8 support
        }
        // ... more SMS
    }
    ```
    
    Example, list all received SMS
    ```shell
    modem@sms.list
    {
        "sms2.X54gbI":
        {
            "id":"sms2.X54gbI",
            "contact":"17266972742",
            "date":"25-11-28 01:43:27",
            "content":"那就是说他"
        },
        "sms2.UcSHRq":
        {
            "id":"sms2.UcSHRq",
            "contact":"10000",
            "date":"25-11-28 10:57:11",
            "content":"广东省森林防灭火指挥部办公室提醒您：当前，天气持续晴朗干燥，全省所有县区均发布森林火险红色预警，高火险天气极易引发森林火灾。请广大市民群众积极配合，进山不带火、入林不吸烟，严格火源管控，不因一时疏忽大意引发山火而触犯法律法规。绿美广东，人人有责！（省预警中心11月28日发布）"
        },
        "sms2.mnw1mj":
        {
            "id":"sms2.mnw1mj",
            "contact":"17688704240",
            "date":"25-11-28 21:36:26",
            "content":"fuck you"
        },
        "sms2.qiS9pF":
        {
            "id":"sms2.qiS9pF",
            "contact":"10692682810037173",
            "date":"25-11-28 12:44:34",
            "content":"【口味王】您账户有1500积分即将清零，过期无效！立即登录查看→ l.kwwblcj.xyz/k/?A 拒收请回复R"
        },
        "sms2.ugFCOH":
        {
            "id":"sms2.ugFCOH",
            "contact":"10001",
            "date":"25-11-28 11:22:46",
            "content":"温馨提醒：您所订购的套餐月基本费扣费规则为每月1日凌晨一次性扣取，为避免扣费后手机余额不足被停机，请及时缴费。更多详情请访问“中国电信”APP( http://a.189.cn/FnnBmF )“我们百倍努力，期待您的十分满意评价”。【中国电信】"
        }
    }
    ```

+ `delete[ SMS Identify name ]` **delete a SMS**
    - SMS Identify name ----------- [ string ]
    - failed return tfalse
    - error return terror    
    - succeed return ttrue
    
    Example, delete the sms2.ugFCOH SMS
    ```shell
    modem@sms.list                        # List first 
    {
        "sms2.X54gbI":
        {
            "id":"sms2.X54gbI",
            "contact":"17266972742",
            "date":"25-11-28 01:43:27",
            "content":"那就是说他"
        },
        "sms2.UcSHRq":
        {
            "id":"sms2.UcSHRq",
            "contact":"10000",
            "date":"25-11-28 10:57:11",
            "content":"广东省森林防灭火指挥部办公室提醒您：当前，天气持续晴朗干燥，全省所有县区均发布森林火险红色预警，高火险天气极易引发森林火灾。请广大市民群众积极配合，进山不带火、入林不吸烟，严格火源管控，不因一时疏忽大意引发山火而触犯法律法规。绿美广东，人人有责！（省预警中心11月28日发布）"
        },
        "sms2.mnw1mj":
        {
            "id":"sms2.mnw1mj",
            "contact":"17688704240",
            "date":"25-11-28 21:36:26",
            "content":"fuck you"
        },
        "sms2.qiS9pF":
        {
            "id":"sms2.qiS9pF",
            "contact":"10692682810037173",
            "date":"25-11-28 12:44:34",
            "content":"【口味王】您账户有1500积分即将清零，过期无效！立即登录查看→ l.kwwblcj.xyz/k/?A 拒收请回复R"
        },
        "sms2.ugFCOH":
        {
            "id":"sms2.ugFCOH",
            "contact":"10001",
            "date":"25-11-28 11:22:46",
            "content":"温馨提醒：您所订购的套餐月基本费扣费规则为每月1日凌晨一次性扣取，为避免扣费后手机余额不足被停机，请及时缴费。更多详情请访问“中国电信”APP( http://a.189.cn/FnnBmF )“我们百倍努力，期待您的十分满意评价”。【中国电信】"
        }
    }
    modem@sms.delete[ sms2.ugFCOH ]     # delete the sms2.ugFCOH
    ttrue
    modem@sms.list                      # List again 
    {
        "sms2.X54gbI":
        {
            "id":"sms2.X54gbI",
            "contact":"17266972742",
            "date":"25-11-28 01:43:27",
            "content":"那就是说他"
        },
        "sms2.UcSHRq":
        {
            "id":"sms2.UcSHRq",
            "contact":"10000",
            "date":"25-11-28 10:57:11",
            "content":"广东省森林防灭火指挥部办公室提醒您：当前，天气持续晴朗干燥，全省所有县区均发布森林火险红色预警，高火险天气极易引发森林火灾。请广大市民群众积极配合，进山不带火、入林不吸烟，严格火源管控，不因一时疏忽大意引发山火而触犯法律法规。绿美广东，人人有责！（省预警中心11月28日发布）"
        },
        "sms2.mnw1mj":
        {
            "id":"sms2.mnw1mj",
            "contact":"17688704240",
            "date":"25-11-28 21:36:26",
            "content":"fuck you"
        },
        "sms2.qiS9pF":
        {
            "id":"sms2.qiS9pF",
            "contact":"10692682810037173",
            "date":"25-11-28 12:44:34",
            "content":"【口味王】您账户有1500积分即将清零，过期无效！立即登录查看→ l.kwwblcj.xyz/k/?A 拒收请回复R"
        }
    }    
    ```
