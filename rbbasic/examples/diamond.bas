10 LINES=17
20 FOR I=1 TO LINES/2+1
30 FOR J=1 TO (LINES+1)/2-I+1:PRINT " ";:NEXT
40 FOR J=1 TO I*2-1:PRINT "*";:NEXT
50 PRINT
60 NEXT I
70 FOR I=1 TO LINES/2
80 FOR J=1 TO I+1:PRINT " ";:NEXT
90 FOR J=1 TO ((LINES+1)/2-I)*2-1:PRINT "*";:NEXT
100 PRINT
110 NEXT I
