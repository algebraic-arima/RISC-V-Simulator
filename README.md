# RISC-V Simulator for PPCA

2024-07-27~2024-08-01

## 分支预测

| test point     | always jump | always stay | 2-bit saturating counter(2) | 2-level adaptive predictor |
|----------------|-------------|-------------|-----------------------------|----------------------------|
| array_test1    |             |             | 0.545455                    |                            |
| array_test2    |             |             | 0.500000                    |                            |
| basicopt1      |             |             | 0.600758                    |                            |
| bulgarian      |             |             | 0.704936                    |                            |
| expr           |             |             | 0.657658                    |                            |
| gcd            |             |             | 0.566667                    |                            |
| hanoi          |             |             | 0.500716                    |                            |
| lvalue         |             |             | 0.666667                    |                            |
| magic          |             |             | 0.613063                    |                            |
| manyarguments  |             |             | 0.800000                    |                            |
| multiarray     |             |             | 0.722222                    |                            |
| pi             |             |             | 0.698737                    |                            |
| qsort          |             |             | 0.794901                    |                            |
| queens         |             |             | 0.495993                    |                            |
| statement_test |             |             | 0.594059                    |                            |
| superloop      |             |             | 0.503642                    |                            |
| tak            |             |             | 0.512987                    |                            |