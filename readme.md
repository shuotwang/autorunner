# AutoGrader

## Input Format

The AutoGrader takes JSON file named ```input.json``` as an input. 
The ```input.json``` file has 2 parts: Init and Commands

Please go to ```file``` folder, find input.json and modify.

### Init
There are 3 parameters in ```Init``` part: ```TimerUS```, ```VideoMS``` and ```CPUFreq```

- TimerUS: Int, Timer value in microsecond
- VideoMS: Int, Video interrupt every VideoMS in milisecond
- CPUFreq: Int, CPU frequency


### Commands
Commands are arranged in Array format. Each element has 3 members: ```Cycle```, ```Type```, and ```Data```

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
      "Cycle":0,
      "Type":"InsertFW",
      "Data":"/code/autograder/files/firmware"
    },
    {
      "Cycle":0,
      "Type":"InsertCart",
      "Data":"/code/autograder/files/cartridge"
    },
    {
      "Cycle":1000,
      "Type":"DirectionUp",
      "Data":""
    },
    {
      "Cycle":2500,
      "Type":"OutputRegs",
      "Data":""
    },
    {
      "Cycle":2500,
      "Type":"OutputCSRs",
      "Data":""
    },
    {
      "Cycle":2500,
      "Type":"OutputMem",
      "Data":"0x500FE110-0x500FE11F"
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
            "Cycle": 2500,
            "Regs": {
                "a0": "00000000",
                "a1": "700fff10",
                "a2": "00000384",
                "a3": "00000000",
                "a4": "000003e8",
                "a5": "40000010",
                "gp": "70000800",
                "ra": "00000248",
                "s0": "700fff1c",
                "s1": "00000000",
                "sp": "700fff00",
                "t0": "00000000",
                "t1": "00000064",
                "t2": "00000000",
                "tp": "00000000",
                "zero": "00000000"
            }
        },
        {
            "Cycle": 2500,
            "CSRs": {
                "marchid": "00000000",
                "mcause": "00000002",
                "mcounteren": "00000007",
                "mcycle": "00000a86",
                "mcycleh": "00000000",
                "mepc": "70000800",
                "mhartid": "00000000",
                "mie": "00000888",
                "mimpid": "00000000",
                "minstret": "00000a86",
                "minstreth": "00000000",
                "mip": "00000000",
                "misa": "40001010",
                "mscratch": "70000800",
                "mstatus": "00001880",
                "mtval": "00000000",
                "mtvec": "000001d4",
                "mvendorid": "00000000"
            }
        },
        {
            "Cycle": 2500,
            "Mem": {
                "0x500FE110": "282828000000000028287c287c282800"
            }
        }
    ]
}
```

## Design 
We have implemented most of operations in the GUI console, including ```w,a,x,d``` directions key events, ```CMD``` key events, ```u,i,j,k``` key events, as well as ```Firmware``` Insert, ```Cartridge``` Insert and remove, ```Run``` and ```PWR``` key events in debug mode. Each button corresponds to one command in ```input.json``` file. 

The ```memory``` buttons, ```Clear``` button and ```RST``` button were not implemented. 

For functions of memory buttons, users can change the ```OutputMem``` part in ```input.json```.

For ```RST(reset)``` button event, it's hard to count the cycle number after resetting. 
