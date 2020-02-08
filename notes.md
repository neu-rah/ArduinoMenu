# Notes

## Memory

Memory figures for ATTiny85

2019.11.21 static tree with change and enable/disable control
```text
DATA:    [==        ]  24.6% (used 126 bytes from 512 bytes)
PROGRAM: [======    ]  63.5% (used 5202 bytes from 8192 bytes)
```

2019.11.25 - partial/minimal draw
```text
DATA:    [===       ]  25.6% (used 131 bytes from 512 bytes)
PROGRAM: [=======   ]  67.0% (used 5488 bytes from 8192 bytes)
```

2019.11.26 - partial/minimal draw (bug fixes)
```text
DATA:    [===       ]  25.6% (used 131 bytes from 512 bytes)
PROGRAM: [=======   ]  68.3% (used 5594 bytes from 8192 bytes)
```

2020.02.02 - joing dynamic and static API's
DATA:    [==        ]  23.6% (used 121 bytes from 512 bytes)
PROGRAM: [====      ]  35.3% (used 2894 bytes from 8192 bytes)

2020.02.07 - static&dynamic API's + PathRef length
DATA:    [==        ]  23.8% (used 122 bytes from 512 bytes)
PROGRAM: [=====     ]  54.2% (used 4444 bytes from 8192 bytes)

2020.02.08 - use panels, rewrite std::vector menu
DATA:    [==        ]  23.8% (used 122 bytes from 512 bytes)
PROGRAM: [======    ]  64.6% (used 5294 bytes from 8192 bytes)
