* Time: 70050-70170
    [9403, 9404] - split

A <-- SU-8001 ( 9404> ) - SU-8000 ( 9403> ) --> B

  A    Current      B

"51"  - "31"   -  "59"



**9403**
WAIT
"startTime": "70170",
"endTime": "70842",

MOVE
"startTime": "70842",
"endTime": "71712",  


**9404**
WATI
"startTime": "70170",
"endTime": "75432",
            
MOVE
"startTime": "75432",
"endTime": "76122



**Isue**


A <-- SU-8001 ( 9404> ) - SU-8000 ( 9403> ) --> B

Declared as:
```bash
"trainUnitIds": [
					"9403",
					"9404"
				],
```

A <-- SU-4000 ( 2602 - 2403> ) --> B
Declared as:
```bash
"trainUnitIds": [
						[
							"2403",
							"2602"
						]
					]
```

A <-- SU-2000 ( 2402> ) - SU-3000 ( 2601> ) --> B
Declared as:
```bash
"trainUnitIds": [
					"2403",
					"2602"
				],
```

**TODO**
In `process_plan.py` in "Combine" action remove `[]` from `trainUnitIdsrom` 



	Spoor63            	|  A <-- SU-1000 ( 2401> ) - SU-4000 ( 2403 - 2602> ) --> B
	Spoor63            	|  A <-- SU-4000 ( 2602 - 2403 - 2401> ) --> B
