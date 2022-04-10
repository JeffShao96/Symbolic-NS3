#include <iostream>
#include <fstream>
#include <string>
#include <cassert>

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "/home/s2e/s2e/s2e.h"
#include "ns3/symbolic-module.h"

//Network Topology
//
// snda -------rcv------- sndb
//    point-to-point
//
 
using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("SymNS3ScriptExample");

int
main (int argc, char *argv[])
{
  Time::SetResolution (Time::MS); 
  
  Ptr<Symbolic> d0 = CreateObject<Symbolic>();
  d0->SetAttribute("Min",UintegerValue(1));
  // We have Muitlple way to set a Max or Min.
  // d0->SetAttribute("Min",UintegerValue(Time(1ms).GetTimeStep()));
  // d0->SetMin(1);
  // d0->SetMin(Second(1));
  // d0->SetMin(Time("1ms"));
  d0->SetAttribute("Max",UintegerValue(1000));

  Ptr<Symbolic> d1 = CreateObject<Symbolic>();
  d1->SetAttribute("Min",UintegerValue(1));
  d1->SetAttribute("Max",UintegerValue(1000));


  std::vector<PointToPointHelper> p2p (2);
  p2p[0].SetChannelAttribute ("SymbolicDelay", PointerValue (d0));
  p2p[1].SetChannelAttribute ("SymbolicDelay", PointerValue (d1));

  NodeContainer nodes;
  nodes.Create (3); 

  std::vector<NodeContainer> nodeAdjacencyList (2);
  nodeAdjacencyList[0] = NodeContainer (nodes.Get (0), nodes.Get (2));
  nodeAdjacencyList[1] = NodeContainer (nodes.Get (1), nodes.Get (2));

  std::vector<NetDeviceContainer> devices (2);
  devices[0] = p2p[0].Install (nodeAdjacencyList[0]);
  devices[1] = p2p[1].Install (nodeAdjacencyList[1]);

  InternetStackHelper stack;
  stack.Install (nodes);

  Ipv4AddressHelper address;
  std::vector<Ipv4InterfaceContainer> interfaces (2);
  for (uint32_t i = 0; i < 2; i++)
    {
      std::ostringstream subset;
      subset << "10.1." << i + 1 << ".0";
      address.SetBase (subset.str().c_str (), "255.255.255.0"); 
      interfaces[i] =
          address.Assign (devices[i]); 
    }

  UdpServerHelper server (2333);

  ApplicationContainer rcv = server.Install (nodes.Get (2));
  rcv.Start (Seconds (1.0));
  rcv.Stop (Seconds (10.0));

  UdpClientHelper snd1 (interfaces[0].GetAddress (1), 2333);
  snd1.SetAttribute ("MaxPackets", UintegerValue (1));

  UdpClientHelper snd2 (interfaces[1].GetAddress (1), 2333);
  snd2.SetAttribute ("MaxPackets", UintegerValue (1));

  ApplicationContainer snd;
  snd.Add(snd1.Install (nodes.Get (0)));
  snd.Add(snd2.Install (nodes.Get (1)));
  snd.Start (Seconds (1.0));
  snd.Stop (Seconds (10.0));

  Simulator::Run ();
  Symbolic Diff = d0 - d1;
  Diff.PrintRange("Diff");
  Simulator::Destroy ();
  s2e_kill_state(0,"Program Terminated");
  return 0;
}
