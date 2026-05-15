# SLR1-Parse-Table-Generator
This is an SLR (1) parse table generator aimed at helping educators teach SLR (1) parsing. It generates the table in CSV format as well as the automata in a Graphviz DOT file.

## Example
```
S -> aSa | B
B -> bB | b
```
Generates the following automata\
<br></br>
<img src="https://github.com/Cherry-Trees/SLR1-Parser/blob/main/src/automata.png" width="400" height="640" />
\
\
and table
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
