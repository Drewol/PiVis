# PiVis
Display remotely controled data on a Raspberry Pi using grpc, nanovg, and glfw.

## Running
The `servers` folder contains what the Raspberry Pi will run and this can be buily by running `mkdir build && cd build && cmake .. && make` 
from the root directory of the project.

The `clients` folder contains a couple example clients written in C#. The clients are self contained but refer to the same `.proto` file
as the Raspberry Pi server.