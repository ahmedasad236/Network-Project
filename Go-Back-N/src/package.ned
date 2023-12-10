package go_back_n;

@license(LGPL);
//
// TODO documentation
//
network Network
{
    submodules:
        coordinator: Coordinator {
            @display("p=487,394");
        }
        node0: Node {
            @display("p=827,181");
        }
        node1: Node {
            @display("p=72,181");
        }
    connections:
        node0.in_ports[0] <-- coordinator.out_ports[0];
        node1.in_ports[0] <-- coordinator.out_ports[1];

        node0.in_ports[1] <-- node1.out_port;
        node1.in_ports[1] <-- node0.out_port;
}

