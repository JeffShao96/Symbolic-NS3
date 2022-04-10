#include <iostream>
#include <fstream>
#include <string>
#include <cassert>

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"

//Network Topology
//
// snda -------rcv------- sndb
//    point-to-point
//

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("CurrentNS3ScriptExample");

int
main (int argc, char *argv[])
{
  Time::SetResolution (Time::MS);

  uint32_t d0 = 0;
  uint32_t d1 = 0;
  CommandLine cmd (__FILE__);
  cmd.AddValue ("d0", "The delay for link between snd1 and rcv.", d0);
  cmd.AddValue ("d1", "The delay for link between snd2 and rcv.", d1);
  cmd.Parse (argc, argv);


  std::vector<PointToPointHelper> p2p (2);
  p2p[0].SetChannelAttribute ("Delay", TimeValue (Time (d0)));
  p2p[1].SetChannelAttribute ("Delay", TimeValue (Time (d1)));

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
      address.SetBase (subset.str ().c_str (), "255.255.255.0");
      interfaces[i] = address.Assign (devices[i]);
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
  Time Diff = Time(d0)-Time(d1);
  std::cout<<"Diff is "<<Diff<<std::endl;
  Simulator::Destroy ();
  return 0;
}
