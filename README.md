# Kosaraju Server

## Introduction

This project implements a multi-threaded server that manages a directed graph and performs Strongly Connected Components (SCC) analysis using Kosaraju's algorithm. It demonstrates advanced concurrent programming concepts, including the Reactor and Proactor design patterns, POSIX mutexes, and condition variables.

## Authors

This project was developed collaboratively by:
- [Gal Ben Ami](https://github.com/benami171)
- [Barak Rozenkvit](https://github.com/BarakRozenkvit)
- [Hagit Dahan](https://github.com/hagitdahan)


## Project Background

This server was implemented as an assignment for the [Operating Systems] course at [Ariel University]. The assignment required progressive development through multiple levels, each adding new functionalities and demonstrating various concurrent programming concepts.

The full project instructions are available in the `assignment_instructions.pdf` file in this repository.


## Features

- TCP server supporting multiple concurrent clients
- Dynamic graph manipulation (add/remove edges, create new graphs)
- Kosaraju's algorithm implementation for SCC analysis
- Reactor and Proactor design pattern implementations
- Thread-safe graph operations
- Automatic detection and notification of significant SCC changes

## Technologies Used

- C++
- POSIX Threads
- TCP/IP Networking
- Custom Reactor and Proactor libraries

## Installation

1. Clone the repository:
```
git clone https://github.com/benami171/Synchronization_Kosaraju.git
```

2. Navigate to the project directory:
   
```
cd Synchronization_Kosaraju
```

3. Compile the project and go to final directory:
   
```
   make all
   cd Level_10
```
## Usage

1. Run the main to start the server:
   
   ```
   ./main
   ```
The server will start and listen on port 9034.

2. Connect a client using netcat:

   ```
   nc localhost 9034
   ```

3. Initialize a new graph with the desired number of nodes `n` and edges `m`:

   ```
   Newgraph
   3 3
   ```

4. Add edges to the graph:
   ```
   1 2
   2 3
   3 1
   ```
   
Repeat `m` times, where `a` and `b` are node indices.

5. Available commands:
- `Kosaraju`: Compute and display SCCs, sends the result to the asking client.
- `Newedge a b`: Add a new edge from node `a` to `b`
- `Removeedge a b`: Remove the edge from node `a` to `b`
- `Newgraph`: Initialize a new graph (follows steps 3-4)

## Architecture

The server utilizes a multi-threaded architecture with the following components:

- Main thread: Accepts new client connections
- Client threads: Handle individual client requests
- Kosaraju thread: Performs SCC analysis
- Monitor thread: Detects significant changes in SCC structure

Thread synchronization is managed using POSIX mutexes and condition variables to ensure thread-safe operations on the shared graph data structure.

## Performance Considerations

- The graph implementation uses efficient data structures optimized for Kosaraju's algorithm.
- The Proactor pattern is employed to handle I/O operations asynchronously, improving server responsiveness.

> see Level_02 and Level_02b to see profiling tests we did in order to determine what will be the most efficient data structures to use in Kosaraju algorithm and the Graph.

## Future Improvements

- Implement more graph algorithms
- Add support for weighted graphs
- Develop a graphical user interface for graph visualization

