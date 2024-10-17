# bms-wireless-and-mobile-networks



```

0000000000000001

00000000000000010110111001
```

```
initial data :    00010010001101000000000000
initial divisor:                 10110111001
===================================
Divisor Shift: 15
Current Bit:   25
Data:    00010010001101000000000000
Divisor: 10110111001000000000000000
SKIP
===================================
Divisor Shift: 14
Current Bit:   24
Data:    00010010001101000000000000
Divisor: 01011011100100000000000000
SKIP
===================================
Divisor Shift: 13
Current Bit:   23
Data:    00010010001101000000000000
Divisor: 00101101110010000000000000
SKIP
===================================
Divisor Shift: 12
Current Bit:   22
Data:    00010010001101000000000000
Divisor: 00010110111001000000000000
Result:  00000100110100000000000000
===================================
Divisor Shift: 11
Current Bit:   21
Data:    00000100110100000000000000
Divisor: 00001011011100100000000000
SKIP
===================================
Divisor Shift: 10
Current Bit:   20
Data:    00000100110100000000000000
Divisor: 00000101101110010000000000
Result:  00000001011010010000000000
===================================
Divisor Shift: 9
Current Bit:   19
Data:    00000001011010010000000000
Divisor: 00000010110111001000000000
SKIP
===================================
Divisor Shift: 8
Current Bit:   18
Data:    00000001011010010000000000
Divisor: 00000001011011100100000000
Result:  00000000000001110100000000
===================================
Divisor Shift: 7
Current Bit:   17
Data:    00000000000001110100000000
Divisor: 00000000101101110010000000
SKIP
===================================
Divisor Shift: 6
Current Bit:   16
Data:    00000000000001110100000000
Divisor: 00000000010110111001000000
SKIP
===================================
Divisor Shift: 5
Current Bit:   15
Data:    00000000000001110100000000
Divisor: 00000000001011011100100000
SKIP
===================================
Divisor Shift: 4
Current Bit:   14
Data:    00000000000001110100000000
Divisor: 00000000000101101110010000
SKIP
===================================
Divisor Shift: 3
Current Bit:   13
Data:    00000000000001110100000000
Divisor: 00000000000010110111001000
SKIP
===================================
Divisor Shift: 2
Current Bit:   12
Data:    00000000000001110100000000
Divisor: 00000000000001011011100100
Result:  00000000000000101111100100
===================================
Divisor Shift: 1
Current Bit:   11
Data:    00000000000000101111100100
Divisor: 00000000000000101101110010
Result:  00000000000000000010010110
===================================
Divisor Shift: 0
Current Bit:   10
Data:    00000000000000000010010110
Divisor: 00000000000000010110111001
SKIP
===================================
Result:  0010010110
Now, just add one of the Bulgarian constants depending on which block it is; for the first block, use Bulgarian constant A.
FINAL RESULT (result XOR A (0xFC)):  0001101010
Result:  0010010110
```
