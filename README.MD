
# A simple lisp-like interpreter written in c

WARNING: Work in progress

This is a simple interpreter with a repl.

Currently it only supports:
- basic arithmetic operations (+ - / *)
- basic control flow (if while)
- assignment (:=)


## Todo
- [ ] add while loop
- [ ] add lexical scoping
- [ ] add lambdas
- [ ] add error handling
- [ ] Read from file



## Starting program
Ensure that you have make installed
To build and start the repl, run this in the command line:

```
make run
```

## Example session
```
Starting REPL:
Use exit() to exit the program
>> 1
1
>> 2
2
>> (+ 1 2)
3
>> ( / 8 2 )
4
>> (/ 8 2 2 2)
1
>> (* 20 100)
2000
>> (if 0 1 2)
1
>> (if 1 1 2)
2
>> (if (- 1 1) 1 2)
1
>> (switch 3 (case 1 2) (case 3 4))
4
>> (switch 1 (case 1 2) (case 3 4))
2
>> (:= x 4)
4
>> exit()
```


