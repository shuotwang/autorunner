# AutoGrader

## Input Format

The AutoGrader takes JSON file named ```input.json``` as an input. 
The ```input.json``` file has 2 parts: Init and Commands

### Init
There are 3 parameters in ```Init``` part: ```TimerUS```, ```VideoMS``` and ```CPUFreq```

- TimerUS: Int, Timer value in microsecond
- VideoMS: Int, Video interrupt every VideoMS in milisecond
- CPUFreq: Int, CPU frequency


### Commands
Commands are arranged in Array format. Each element has 3 members: ```Cycle```, ```Type```, and ```Data```




The JSON file format is shown as follows:

```
{
  "Init" : {
    "TimerUS" : INT,
    "VideoMS" : INT,
    "CPUFreq" : INT
    },
  "Commands": [ 
    {
      "Cycle": CYCLE_NUMBER, INT,
      "Type":"InsertFW",
      "Data":"path to firmware"
    },
    ...
  ]
}  
```

## Output Format
The content of output depends on the actual commands. 