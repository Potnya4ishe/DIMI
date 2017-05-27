# DIMI

DIMI stands for Distributed Intagration via Mathematical Interpretator.

It can be used as the sole calculator, where all the calculations are arranged on the servers in the local networks. It allows you to calculate the integral on several servers, where the task will be distributed equally in accordance with the number of cores available on each server.

## Client
Client reads the task and sends it to the servers, who interpreter it and calculate (if the calculation is needed). If the task is to calculate the intergal, then the client arranges the broadcast search in the local networks for servers, who are ready to solve the task. 

## Server
Servers, when working, are accepting signals from broadcasts of clients. When some client is asking them to solve some task, they quit accepting other connections and calculating the given task, sending the result back to the client.

# Run options

## Build
Just run `make` to complile the project.

## Visualization
The project requires PyQt 5 for simple graphical visualization, where you have the option to choose whether you want to run the client or the server, and input and output windows are arranged in the natural way. To run this version, use
```
python3 binnaviPlugin.py
```


## From terminal
To run the server, type
```
./server k
```
where `k` equals the number of cores you want to use.

To run the client, use
```
python3 runCLient.py
```
Then you should follow the instuctions on the screen, where you will be asked to type expression you want to calculate.
