## Intro
 In this assignment we wanted to implement and use the Reactor and Proactor design patterns, POSIX mutex and POSIX cond.
 In each level we added functionality to the program, so level10 contains the complete functionality.

 ## How to run
- Go to folder `Level10`
   inside the folder run `make`
   to run the program, type `./main`.
- the main will start a TCP server on Port 9034 thats waiting for a clients to connect.
- In order to connect using netcat, open a seperate terminal and type `nc localhost 9034`
- At this point the server is waiting to get a graph from the client, type in `Newgraph +LF`.

 Type the number of nodes (n) and edges (m) you want the graph to have in this format `n m +LF`.
 
 Type in the edges you want from edge a to edge b in this way `a b +LF`, do it `m` times (as the number of edges stated previously).


- Now the user can do several Actions on the graph:


   1.`Kosaraju` - returns the SCC components of the graph.

   2.`Removeedge` - waits for a edge in `a b` format, and removes it from the Graph.
  
   3.`Newedge` - gets an edge to add to the graph.
  
   4.`Newgraph` - gets a new graph that will replace the existing graph.

