# Øving 2

Make multi-threaded server in C.

## How to run the program
To compile the program run: 
```
gcc -pthread -o mtwwwd mtwwwd.c sem.c bbuffer.c
```

Then to set up the connection run: 
```
./mtwwwd www-path portnumber #threads #bufferslots
```
In my case, that could be: 
```
./mtwwwd /Applications/OpSys2/os2 8000 30 32
```

## Transfer data from server 
If you want see the HTML generated on you localhost in terminal, open a new terminal and run: 
```
curl --http0.9 localhost:8000/index.html
```
But running the wrong path, for example:
```
curl --http0.9 localhost:8000/index.htm
```
should return the `404page.html`-file. 