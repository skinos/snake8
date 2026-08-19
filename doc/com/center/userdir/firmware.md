## <username>/firmware — Per-user firmware library

### Overview

Directory that stores `.zz` firmware images for one username.

- Path: `{device_path}/<username>/firmware/`
- HE/dbs path: `center@heport/<username>/firmware/`
- Created with the user tree; files added/removed by `center@api.firmware_add` / `firmware_delete`
- Listed by `center@api.firmware_list`; pushed to online gateways by `center@api.firmware_push`
- Each file is a full firmware image; there is no extra sidecar JSON beside the `.zz`


### Concepts

Filename convention produced by `firmware_add`:

```
{hardware}_{custom}_{scope}_{version}[_{oem}].zz
```

- Parsed from the uploaded image contents (not from the upload filename alone)
- Optional `_oem` suffix when an OEM marker file exists inside the image unpack dir
- Push uses the stored `.zz` path; no additional on-disk state is written for push progress


### Configuration reference ( <username>/firmware )

```json
// Attributes introduction 
{
    "file":                                     // [ string ]: { json }, one entry per .zz in the directory (API list shape)
    {
        "dir": "absolute directory",            // [ string ], firmware directory path
        "custom": "custom identify",            // [ string ], from filename
        "scope": "scope identify",              // [ string ], from filename
        "version": "firmware version",          // [ string ], from filename
        "oem": "oem identify",                  // [ string ], optional; present when filename has oem suffix
        "zz": "filename"                        // [ string ], e.g. mt7628_r600_std_v8.0.0.zz
    }
    // "...":{ ... }  How many images show how many properties
}
```

#### Configuration example

Example, list firmware library for user ashyelf

```shell
center@api.firmware_list[ ashyelf ]
{                                               # return this
    "mt7628_r600_std_v8.0.0.zz":
    {
        "dir": "/var/…/ashyelf/firmware",
        "custom": "r600",
        "scope": "std",
        "version": "v8.0.0",
        "zz": "mt7628_r600_std_v8.0.0.zz"
    }
}
```



### Other

Related HE APIs (see `api.md`): `firmware_add`, `firmware_delete`, `firmware_list`, `firmware_path`, `firmware_push`.  
Do not rename `.zz` files by hand; use the API so the `{hardware}_{custom}_{scope}_{version}` convention stays valid.
