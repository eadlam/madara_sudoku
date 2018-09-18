#include <algorithm>
#include <future>
#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
#include "madara/knowledge/KnowledgeBase.h"
#include "madara/knowledge/containers/FlexMap.h"
#include "madara/knowledge/containers/Integer.h"
#include "madara/utility/Utility.h"


/**
 *  @file sudoku_agent.cpp
 *  @author Eric Adlam <eadlam@gmail.com>
 *
 *  This is the entry point and contains all functions and definitions
 *  for Madara Sudoku
 */


namespace engine = madara::knowledge;
namespace containers = engine::containers;
typedef engine::EvalSettings EvalSettings;
typedef madara::transport::QoSTransportSettings QoSTransportSettings;


// global variables
std::string host ("");
std::string host_prefix("tcp://127.0.0.1:");
int starting_port = 30000;
int num_agents = 81;


/**
 *  Construct [protocol]://[ip]:[port] and push to settings.hosts
 *  
 *  @param[in]  id       Add id to starting port to get port number
 *  @param[out] settings The address is pushed to settings.hosts
 */ 
void add_agent(int id, QoSTransportSettings& settings){
  std::stringstream ss;
  ss << host_prefix << (starting_port + id);
  settings.hosts.push_back(ss.str());
}


/**
 *  Add peers to settings.hosts (skipping this agent)
 *  
 *  @brief  The transport layer expects the current agent to be the first item
 *          in settings.hosts, and for peers to follow. This adds all peers, 
 *          skipping the current agent
 *  
 *  @param[in]  id        The id of the current agent to skip
 *  @param[out] settings  The peers are pushed to settings.hosts
 */ 
void add_peers(int id, QoSTransportSettings& settings){
  for(int i = 0; i < id; i++){
    add_agent(i, settings);
  }
  for(int i = id + 1; i < num_agents; i++){
    add_agent(i, settings);
  }
}


/**
 *  Check peers in same row/col/quad to find value by process of elimination
 *
 *  @param[in]      id      The id of the current agent
 *  @param[in,out]  agents  The object to search and update
 */
int find_value(containers::Integer id, containers::FlexMap& agents){
  int possible [9] = {1, 1, 1, 1, 1, 1, 1, 1, 1};
  //  TODO: divide knowledge/messages into own domains by row/col/quad, 
  //        so we don't iterate over all agents
  /// Search over all agents, ignoring those that are not in same row/col/quad
  for(int i = 0; i < 81; i++){
    if((
          agents[i]["row"].to_integer() == agents[*id]["row"].to_integer() ||
          agents[i]["col"].to_integer() == agents[*id]["col"].to_integer() ||
          agents[i]["quad"].to_integer() == agents[*id]["quad"].to_integer()
       ) && agents[i]["value"].to_integer() != 0)
    {
      possible[agents[i]["value"].to_integer() - 1] = 0;
    }  
  }
  /// Check if all but one were eliminated
  int last_match = 0, total_possible = 0;
  for(int i = 0; i < 9; i++){
    if(possible[i] == 1){
      last_match = i;
      total_possible += 1; 
    }
  }
  if(total_possible == 1){
    /// values are 1-9, so add 1 to the index
    return last_match + 1;
  }
  return 0;
} 


/** 
 * Check that all agents have a non-zero value
 *
 * @param[in,out] agents The object to search and update
 */
bool is_complete(containers::FlexMap& agents){
  int missing = 0;
  for(int i = 0; i < 81; i++){
    if(agents[i]["value"].to_integer() == 0){
      missing += 1;
      break;
    }
  }
  if(missing == 0){
    return true;
  }
  return false;
}


/**
 * Entry point to sudoku agent
 */
int main (int argc, char * argv[])
{
  /// For simplicity, I'm just assuming both arguments are passed as expected
  int id_arg, value_arg;
  std::stringstream ss1(argv[1]), ss2(argv[2]);
  ss1 >> id_arg;
  ss2 >> value_arg;

  /// Setup ZMQ network
  QoSTransportSettings settings;
  settings.type = madara::transport::ZMQ;
  add_agent(id_arg, settings);
  add_peers(id_arg, settings);

  /// Initialize knowledgebase and agent id
  engine::KnowledgeBase knowledge (host, settings);
  engine::CheckpointSettings output;
  output.filename = "out/sudoku.kkb";

  /// setup private knowledge variables / calculate row, col, quadrant
  containers::Integer id(".id", knowledge, id_arg);
  containers::Integer row(".row", knowledge, *id / 9);
  containers::Integer col(".col", knowledge, *id % 9);
  containers::Integer quad(
    ".quad", knowledge, ((*row / 3) * 3) + ((*col / 3) % 3)
  );

  /// setup global knowledge variables 
  containers::FlexMap agents("agents", knowledge);
  containers::Integer complete("complete", knowledge, 0);
  
  /// until complete, keep propogating/deducing info
  while(complete.to_integer() == 0)
  {
    /// Propogate row, col, quad, value
    // TODO: instead of publishing this continuously, maybe just wait for all
    //       agents to come online and then publish location once...
    agents[*id]["row"] = *row; 
    agents[*id]["col"] = *col; 
    agents[*id]["quad"] = *quad;
    agents[*id]["value"] = std::max(
      (int) agents[*id]["value"].to_integer(), value_arg
    ); 
    knowledge.send_modifieds(
      "KnowledgeBase::send_modifieds", EvalSettings::SEND
    );

    /// If our value is 0, check peers and try to update value
    if(agents[*id]["value"].to_integer() == 0){
      agents[*id]["value"] = find_value(id, agents);
      knowledge.send_modifieds(
        "KnowledgeBase::send_modifieds", EvalSettings::SEND
      ); 
    }

    /// If solution is complete, set complete flag and save to file
    if(is_complete(agents)){
      knowledge.lock();
      complete = 1;
      knowledge.save_as_karl(output);
      knowledge.unlock();
      knowledge.send_modifieds(
        "KnowledgeBase::send_modifieds", EvalSettings::SEND
      ); 
    }

    madara::utility::sleep (1);
  }

  return 0;
}
