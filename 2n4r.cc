#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/point-to-point-module.h"
#include "ns3/netanim-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("P2PRoutingSimulation");

int main() {
	LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);
	LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO);

	NodeContainer nodes;
	nodes.Create(6);

	PointToPointHelper pointToPoint;
	pointToPoint.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
	pointToPoint.SetChannelAttribute("Delay", StringValue("2ms"));

	NetDeviceContainer devices01;
	devices01 = pointToPoint.Install(nodes.Get(0), nodes.Get(1));
	
	NetDeviceContainer devices12;
	devices12 = pointToPoint.Install(nodes.Get(1), nodes.Get(2));

	NetDeviceContainer devices23;
        devices23 = pointToPoint.Install(nodes.Get(2), nodes.Get(3));

	NetDeviceContainer devices34;
        devices34 = pointToPoint.Install(nodes.Get(3), nodes.Get(4));

	NetDeviceContainer devices45;
        devices45 = pointToPoint.Install(nodes.Get(4), nodes.Get(5));

	InternetStackHelper stack;
	stack.Install(nodes);

	Ipv4AddressHelper address;

	address.SetBase("10.1.1.0", "255.255.255.0");
	Ipv4InterfaceContainer interfaces01 = address.Assign(devices01);

	address.SetBase("10.1.2.0", "255.255.255.0");
        Ipv4InterfaceContainer interfaces12 = address.Assign(devices12);

	address.SetBase("10.1.3.0", "255.255.255.0");
        Ipv4InterfaceContainer interfaces23 = address.Assign(devices23);

	address.SetBase("10.1.4.0", "255.255.255.0");
        Ipv4InterfaceContainer interfaces34 = address.Assign(devices34);

	address.SetBase("10.1.5.0", "255.255.255.0");
        Ipv4InterfaceContainer interfaces45 = address.Assign(devices45);

	UdpEchoServerHelper echoServer(9);
	ApplicationContainer serverApps = echoServer.Install(nodes.Get(5));
	serverApps.Start(Seconds(1.0));
	serverApps.Stop(Seconds(10.0));

	UdpEchoClientHelper echoClient(interfaces45.GetAddress(1), 9);
	echoClient.SetAttribute("MaxPackets", UintegerValue(1));
	echoClient.SetAttribute("PacketSize", UintegerValue(1024));
	echoClient.SetAttribute("Interval", TimeValue(Seconds(1.0)));

	ApplicationContainer clientApps = echoClient.Install(nodes.Get(0));
	clientApps.Start(Seconds(2.0));
	clientApps.Stop(Seconds(10.0));

	Ipv4GlobalRoutingHelper::PopulateRoutingTables();

	AsciiTraceHelper ascii;
	pointToPoint.EnableAsciiAll(ascii.CreateFileStream("p2p-routing-trace.tr"));

	AnimationInterface anim("p2p-routing-simulation.xml");
	anim.SetConstantPosition(nodes.Get(0), 0, 50);
	anim.SetConstantPosition(nodes.Get(1), 20, 50);
	anim.SetConstantPosition(nodes.Get(2), 40, 50);
	anim.SetConstantPosition(nodes.Get(3), 60, 50);
	anim.SetConstantPosition(nodes.Get(4), 80, 50);
	anim.SetConstantPosition(nodes.Get(5), 100, 50);

	anim.UpdateNodeDescription(nodes.Get(0), "Client");
	anim.UpdateNodeDescription(nodes.Get(5), "Server");

	anim.UpdateNodeColor(nodes.Get(0), 0, 255, 0);
	anim.UpdateNodeColor(nodes.Get(5), 0, 0, 255);

	Simulator::Stop(Seconds(10.0));
	Simulator::Run();
	Simulator::Destroy();

	return 0;
}
