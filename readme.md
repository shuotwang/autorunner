# AutoRunner

## To Start
1. open terminal and change directory to ```/autorunner/```
2. launch script with command ```./rvconsole.sh```
3. change directory ```cd autorunner```
4. make file with command ```make```
5. change directory to ```/bin/``` and run the executive ```./autorunner```

* To change **Firmware / Cartridge / input.json** file, or to obtain **output.json** file, go to ```/files/```
* To change paths of **input.json** and **output.json**, input command arguments: ```./bin/autorunner INPUT_JSON_PATH OUTPUT_JSON_PATH```, both are absolute path.
* Default **input.json output.json** folder is: ```/code/autorunner/files/```



## Input Format

The AutoRunner takes JSON file named ```input.json``` as an input. 
The ```input.json``` file has 2 parts: Init and Commands

Please go to ```file``` folder, find input.json and modify.

### Init
There are 3 parameters in ```Init``` part: ```TimerUS```, ```VideoMS``` and ```CPUFreq```

- TimerUS: Int, Timer value in microsecond
- VideoMS: Int, Video interrupt every VideoMS in milisecond
- CPUFreq: Int, CPU frequency


### Commands
Commands are arranged in Array format. Each element has 3 members: ```Cycle```, ```Type```, and ```Data```

All commands are listed:
* InserFW, InsertCart, 
* DirectionUp, DirectionDown, DirectionLeft, DirectionRight
* DirectionUpRelease, DirectionDownRelease, DirectionLeftRelease, DirectionRightRelease
* Button1, Button2, Button3, Button4
* Button1Release, Button2Release, Button3Release, Button4Release
* OutputRegs, OutputCSRs, OutputMem

#### Firmware and Cartridge Insert
The Data fields, or directories in InsertFW and InsertCR are absolute path in Docker. Docker Mounts directory is 
```
/code/
```  

#### Memory Output
Please be noticed that the Data format of ```OutputMem```:
```
"Data":"0x500FE110-0x500FE11F"
```
Both left and right addresses included. 

### Input JSON Example
The JSON file format is shown as follows:

```
{
  "Init" : {
    "TimerUS" : 1000,
    "VideoMS" : 10,
    "CPUFreq" : 10000000
    },
  "Commands": [
    {
        "Cycle": 0,
        "Type": "InsertFW",
        "Data": "/code/autorunner/files/firmware"
    },
    {
        "Cycle": 30723138,
        "Type": "InsertCart",
        "Data": "/code/autorunner/files/cartridge"
    },
    {
        "Cycle": 48216286,
        "Type": "DirectionDown",
        "Data": ""
    },
    {
        "Cycle": 52217471,
        "Type": "DirectionDownRelease",
        "Data": ""
    },
    {
        "Cycle": 55959374,
        "Type": "DirectionUp",
        "Data": ""
    },
    {
        "Cycle": 60240058,
        "Type": "DirectionUpRelease",
        "Data": ""
    },
    {
        "Cycle": 105334517,
        "Type": "DirectionLeft",
        "Data": ""
    },
    {
        "Cycle": 111087977,
        "Type": "DirectionLeftRelease",
        "Data": ""
    },
    {
        "Cycle": 122011872,
        "Type": "DirectionRight",
        "Data": ""
    },
    {
        "Cycle": 127923120,
        "Type": "DirectionRightRelease",
        "Data": ""
    },
    {
        "Cycle": 221897000,
        "Type": "Button1",
        "Data": ""
    },
    {
        "Cycle": 223908009,
        "Type": "Button1Release",
        "Data": ""
    },
    {
        "Cycle": 233803010,
        "Type": "Button2",
        "Data": ""
    },
    {
        "Cycle": 238800008,
        "Type": "Button2Release",
        "Data": ""
    },
    {
        "Cycle": 241595441,
        "Type": "Button3",
        "Data": ""
    },
    {
        "Cycle": 245470119,
        "Type": "Button3Release",
        "Data": ""
    },
    {
        "Cycle": 251773401,
        "Type": "Button4",
        "Data": ""
    },
    {
        "Cycle": 254793624,
        "Type": "Button4Release",
        "Data": ""
    },
    {
        "Cycle": 259382891,
        "Type": "OutputRegs",
        "Data": ""
    },
    {
        "Cycle": 259382891,
        "Type": "OutputCSRs",
        "Data": ""
    },
    {
        "Cycle": 259382891,
        "Type": "OutputMem",
        "Data": "0x500FE110-0x500FE11F"
    }
  ]
}  
```

## Output Format
The content of output depends on the actual commands. 
Here is an example:
```
{
    "Outputs": [
        {
            "Cycle": 259382891,
            "Regs": {
                "a0": "00000103",
                "a1": "000000ee",
                "a2": "7fe494b8",
                "a3": "7fe004b8",
                "a4": "00000103",
                "a5": "00000103",
                "gp": "70000800",
                "ra": "200002d0",
                "s0": "71000000",
                "s1": "00000000",
                "sp": "70ffdfc8",
                "t0": "00000000",
                "t1": "ffffe000",
                "t2": "00000000",
                "tp": "00000000",
                "zero": "00000000"
            }
        },
        {
            "Cycle": 259382891,
            "CSRs": {
                "marchid": "00000000",
                "mcause": "0000000b",
                "mcounteren": "00000007",
                "mcycle": "0f75de6b",
                "mcycleh": "00000000",
                "mepc": "200002d0",
                "mhartid": "00000000",
                "mie": "00000888",
                "mimpid": "00000000",
                "minstret": "0f75de6b",
                "minstreth": "00000000",
                "mip": "00000000",
                "misa": "40001010",
                "mscratch": "70000800",
                "mstatus": "00001888",
                "mtval": "00000000",
                "mtvec": "00000088",
                "mvendorid": "00000000"
            }
        },
        {
            "Cycle": 259382891,
            "Mem": {
                "0x500FE110": "282828000000000028287c287c282800"
            }
        }
    ]
}
```

## Design 
We have implemented most of operations in the GUI console, including ```w(up),a(left),x(down),d(right)``` directions key events, ```CMD``` key events, ```u,i,j,k``` key events, as well as ```Firmware``` Insert, ```Cartridge``` Insert and remove, ```Run``` and ```PWR``` key events in debug mode. Each button corresponds to one command in ```input.json``` file. 

The ```memory``` buttons, ```Clear``` button and ```RST``` button were not implemented. 

For functions of memory buttons, users can change the ```OutputMem``` part in ```input.json```.

For ```RST(reset)``` button event, it's hard to count the cycle number after resetting. 
