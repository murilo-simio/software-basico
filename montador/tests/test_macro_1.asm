SECTION TEXT
M1: MACRO &A
OUTPUT &A
ENDMACRO
M2: MACRO &B
M1 &B
ENDMACRO
M2 V
SECTION DATA
V: CONST 4

; teste usando macro chamando outra macro
