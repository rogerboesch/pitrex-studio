10 REM --- BIORHYTHM
15 REM --- COPYRIGHT 1984 DILITHIUM PRESS
20 POKE 82,0
30 DIM U$(40),X(12),C$(40),L$(40),R$(40),A$(40), L$(40),R$(40),A3$(40)
31 DIM X1$(40),X2$(40),X3$(40),W$(40)
40 FOR J=1 TO 12
41 READ A
42 X(J)=A
43 NEXT J
50 DATA 31,28,31,30,31,30,31,31,30,31,30,31
60 L=0:Z=0.99999:T=14:P=3.14159265
70 FOR J=1 TO 29:U$(J,J)=CHR$(18):W$(J,J)=CHR$(32):NEXT J
80 U$(15,15)=CHR$(23):W$(15,15)=CHR$(124)
90 GRAPHICS 0:PRINT "BIORHYTHM" : PRINT :PRINT
100 PRINT "ENTER BIRTHDATE:"
110 GOSUB 240
111 JB=JD
120 PRINT :PRINT "ENTER START DATE FOR CHART"
130 GOSUB 240:JC=JD:IF JC>=JB THEN 150
140 PRINT " CHART DATE CAN'T BE EARL!ER THAN BIRTHDATE. TRY AGAIN"
145 GOTO 100
150 POKE 752,1:FOR J=L TO 1000:NEXT J
160 GOSUB 330
170 N=JC-JB:V=23:GOSUB 350:V=28:GOSUB 350:V=33:GOSUB 350
180 GOSUB 490:L=L+1:POKE 752,1:PRINT C$:POSITION 10,L+3:PRINT L$:JC=JC+1
190 IF L<18 THEN 170
200 POKE 764,255:PRINT : PRINT "PRESS 'E' TO EN O OR SPACE TO CONTINUE"
210 A=PEEK(764):IF A=255 THEN 210
220 IF A-42 THEN POKE 764,255:POKE 752,0:POKE 82 ,2:END
230 L=O:GOTO 160
240 X(2) =28: PRINT : PRINT "YEAR";: INPUT V
245 IF Y<0 THEN GOSUB 540:GOTO 240
250 IF (Y/100)-(INT(Y/100))<>0 THEN 270
260 IF (Y/400)-(INT(Y/400))<>O THEN 280
270 IF (Y/4)-(INT(Y/4))-0 THEN
275 IF Y<=99 THEN Y=Y+1900:PRINT" ";Y;"0"
280 PRINT"MONTH (1 TO 12) ";:INPUT M:M=INT(M):IF M<1 OR M>12 THEN GOSUB 540:GOTO 280
290 PRINT"DAY (1 TO "; X(M); ") ";: INPUT D: IF D <1 OR D>X(M) THEN GOSUB 540:GOTO 290
300 W=INT((M-14)/12+Z):JD=INT(1461*(Y+4900+W)/100):B=367*(M-2-W*12)/12
310 IF B<O THEN B=B+Z
320 B=INT(B):JD=JD+B:B=INT(INT(3*(V+4900+W)/100) /4):JO=JO+O-32075-B:RETURN
330 GRAPHICS 0: POSITION 16,1: POKE 752,1: PRINT "BIORHYTHM": POSITION 4,2:PRINT "DATE"
340 POSITION 13,2:PRINT "D 0 W N    O    U P":FOR J=L TO 10:PRINT CHR$(18);:NEXT J:POSITION 10, 3:PRINT U$:RETURN
350 W=INT(N/V):R=N-W*V
360 IF V<>23 THEN 390
370 L$=W$
380 IF V=23 THEN C$="P"
390 IF V=28 THEN C$="E"
400 IF V=33 THEN C$="I"
410 W=R/V:W=W*2*P:W=T*SIN(W):W=W+T+1.5:W=INT(W): A$=L$(W,W)
420 IF A$="P" OR A$="E" OR A$="*" THEN C$="."
430 IF W=L THEN 470
440 IF W=T+T+1 THEN 480
450 X1$=L$(1,W-1):X2$-L$(LEN(L$)-(T+T-W),LEN(L$)):C=LEN(C$):X1=LEN(X1$):X2=LEN(X2$)
460 L$(1,X1)=X1$:L$(X1+1,XL+C)-C$:L$(X1+C+1,X1+C+X2)=X2$:RETURN
470 X1$=L$(LEN(L$)-(T+T-1),LEN(L$):C=LEN(C$):X1=LEN(X1$):L$(1,C)=C$:L$(C+1,C+X1)=X1$:RETURN
480 X1$=L$(1,T+T):XL-LEN(X1$):C-LEN(C$):L$(1,X1) =XL$:L$(XL+1,XL+C)=C$:RETURN
490 W=JC+68569:R=INT(4*W/146097):W=W-INT(146097*R+3)/4):Y=INT(4000*(W+1)/1461001):W=W-INT(1461* Y/4)+31
500 M=INT(80*W/2447):D=W-INT(2447*M/80):W=INT(M/11):M=M+2-12*W:Y=100*(R-49)+Y+W
510 X1$=STR$(M):X2$=STR$(D):X3$=STR$(Y):X1=LEN(X1$):X2=LEN(X2$):X3=LEN(X3$)
520 C$(1,XL1)=X1$: C$(X1+1, X1+1)=" /": C$(X1+2, X1+X2 +1)=X2$:C$(X1+2+X2, X1+2+X2) = "/"
530 C$(X1+3+X2,X1+2+X2+X3)=X3$:RETURN
540 PRINT:PRINT "** ILLEGAL ENTRY, TRY AGAIN **":PRINT:RETURN

