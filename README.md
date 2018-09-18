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

    $ python3 launch.py in/medium.txt

    Starting Configuration

    0 0 2 4 1 0 0 0 0
    0 8 5 7 6 0 0 0 9
    3 7 0 9 0 0 0 0 0
    4 0 0 0 5 0 0 3 0
    0 1 0 0 9 6 0 0 5
    0 0 0 2 8 4 1 0 0
    0 0 7 5 0 0 0 0 3
    0 0 0 6 0 0 0 0 0
    0 0 9 0 0 1 2 0 7

    Launching 81 agents

    9 6 2 4 1 5 3 7 8
    1 8 5 7 6 3 4 2 9
    3 7 4 9 2 8 5 6 1
    4 9 6 1 5 7 8 3 2
    2 1 8 3 9 6 7 4 5
    7 5 3 2 8 4 1 9 6
    8 2 7 5 4 9 6 1 3
    5 3 1 6 7 2 9 8 4
    6 4 9 8 3 1 2 5 7 
