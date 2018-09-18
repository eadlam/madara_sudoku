## Intro

Madara Sudoku is a distributed-agent sudoku solver. It's purpose is to 
experiment with the madara platform and it's api. (Note: this is not meant to 
be a fast, efficient, or complete sudoku solver)

It showcases the following features:

- 81 agents communicating over ZMQ TCP
- use of the madara::knowledge::containers api to read and write to the knowledgebase
- use of KnowledgeBase::send_modifieds to send data
- saving the knowledgebase to file using the karl format

The important files are:

- src/sudoku_agent.cpp
- ./launcher.py
  
## Installation

This project depends on Madara. Follow the [Madara Installation 
Instructions](https://github.com/jredmondson/madara/wiki/Installation). 
Build with ZMQ and set the $MADARA_ROOT environment variable.

Clone this repository to your system.


## Compile on Linux:

    ./action.sh compile 
   

## Run the Solver:

    python3 launch.py ./in/easy.txt
    python3 launch.py ./in/medium.txt 
    
