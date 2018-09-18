#!/usr/bin/python3

import multiprocessing
import multiprocessing.connection
import re
import subprocess
import sys
import time


# launch an agent in it's own process
def launch_agent(id, v):
  p = subprocess.run(["./bin/sudoku_agent", str(id), v])


# launch 81 agents in parallel
def launch_agents(starting_values):
  sentinels = []
  for i, v in enumerate(starting_values):
    p = multiprocessing.Process(target=launch_agent, args=(i, v.strip()))
    p.start()
    sentinels.append(p.sentinel)
  return sentinels


# print the values  as a 9x9 grid
def print_board(values):
  for i in range(9):
    for j in range(9):
      x = i * 9 + j
      print(values[x], end=" ")
    print("")
  print("")


# use distributed madara agents to solve sudoku
def main():
  filename = sys.argv[1]

  # read in the problem, print board, launch agents
  with open(filename, "r") as problem:
    starting_values = [v.strip() for v in problem.readlines()]

    print("\nStarting Configuration\n")
    print_board(starting_values)

    print(f"Launching {len(starting_values)} agents\n")
    sentinels = launch_agents(starting_values)

    while(len(multiprocessing.connection.wait(sentinels)) < len(sentinels)):
      time.sleep(1)
 
  # read in the solution (saved by an agent), print board
  with open('out/sudoku.kkb', 'r') as karl:
    text = karl.read()
    data = re.findall("agents.([0-9]+).value=([0-9])", text)
    solution = [0] * 81
    for i, j in data:
      solution[int(i)] = j
    print_board(solution) 
  

if __name__ == "__main__":
  assert len(sys.argv) is 2, "You must supply a filename"
  main()
