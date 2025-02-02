# SLR1-Parse-Table-Generator
Generates an SLR Parsing table for a given grammar.\
\
ex.\
S -> aSa | B\
B -> bB | b
<br></br>
Automata:\
<img src="https://github.com/Cherry-Trees/SLR1-Parser/blob/main/src/automata.png" width="400" height="640" />
\
\
Table:
<pre>
|---|---------|----|---------|---|---|
|   | a       | b  | $       | S | B |
|---|---------|----|---------|---|---|
| 0 | s3      | s6 |         | 2 | 1 |
|---|---------|----|---------|---|---|
| 1 | rS->B   |    | rS->B   |   |   |
|---|---------|----|---------|---|---|
| 2 |         |    | acc     |   |   |
|---|---------|----|---------|---|---|
| 3 | s3      | s6 |         | 4 | 1 |
|---|---------|----|---------|---|---|
| 4 | s5      |    |         |   |   |
|---|---------|----|---------|---|---|
| 5 | rS->aSa |    | rS->aSa |   |   |
|---|---------|----|---------|---|---|
| 6 | rB->b   | s6 | rB->b   |   | 7 |
|---|---------|----|---------|---|---|
| 7 | rB->bB  |    | rB->bB  |   |   |
|---|---------|----|---------|---|---|
</pre>
