section text
m1: macro &a
load &a
add one
store &a
endmacro
input v
m1 v
output v
section data
v: space
one: const 1; teste usando 1 macro
