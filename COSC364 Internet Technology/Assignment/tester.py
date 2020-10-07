import sys
import subprocess

SERVER ="python {server_py} router-{router_id}.conf; read -rsn 1 -p \"Press any key to continue...\";echo"

TMUX_SESSION = "tmux new-session -d -s rip-test -n controller"
TMUX_WINDOW = "tmux new-window -d -t rip-test -n {router_id} sh -c"
TMUX_ATTACH = "tmux attach -t rip-test:controller"
TMUX_STOP_ROUTER = "tmux send-key -t rip-test:{router_id} C-c"
TMUX_STOP_CONTROLLER = "tmux send-key -t rip-test:controller C-d"

PORT_DOWN = "iptables -A INPUT -p udp --source 127.0.0.1 --destination 127.0.0.1 --dport {} -j DROP"
PORT_UP   = "iptables -D INPUT -p udp --source 127.0.0.1 --destination 127.0.0.1 --dport {} -j DROP"

class Tester:
    def __init__(self, server_py, timer=None):
        self.server_py = server_py
        self.routers = set()
        self.links = {}
        self.next_port = 3000
        self.timer = timer

    def add_router(self, router_id):
        self.routers.add(router_id)

    def add_link(self, router_a, router_b, metric):
        if (router_a in self.routers and router_b in self.routers and router_a != router_b):
            # A listens on first port, B listens on second
            self.links[(router_a, router_b)] = (self.next_port, self.next_port+1, metric)
            self.next_port += 2

    def generate_configs(self):
        router_neighbours = {}
        for router_id in self.routers:
            router_neighbours[router_id] = []

        for (router_a, router_b), (port_a, port_b, metric) in self.links.items():
            router_neighbours[router_a].append((router_b, port_b, port_a, metric))
            router_neighbours[router_b].append((router_a, port_a, port_b, metric))

        for router_id in self.routers:
            with open("router-{}.conf".format(router_id), 'w') as f:
                f.write("router-id {}\n".format(router_id))
                if self.timer != None:
                    f.write("timer {}\n".format(self.timer))
                f.write("\n")

                f.write("# neighbour router-id output-port input-port metric\n")
                for neighbour in router_neighbours[router_id]:
                    f.write("neighbour ")
                    f.write(' '.join(str(x) for x in neighbour))
                    f.write("\n")

    def generate_iptables_scripts(self):
        for (router_a, router_b), (port_a, port_b, metric) in self.links.items():
            with open("link_{}-{}_up".format(router_a, router_b), 'w') as f:
                f.write("#/usr/bin/sh\n")
                f.write(PORT_UP.format(port_a) + "\n")
                f.write(PORT_UP.format(port_b) + "\n")

            with open("link_{}-{}_down".format(router_a, router_b), 'w') as f:
                f.write("#/usr/bin/sh\n")
                f.write(PORT_DOWN.format(port_a) + "\n")
                f.write(PORT_DOWN.format(port_b) + "\n")

    def start_router(self, router_id):
        command = TMUX_WINDOW.format(router_id=router_id).split();
        command.append(SERVER.format(server_py=self.server_py, router_id=router_id))
        print(command)
        subprocess.run(command)

    def stop_router(self, router_id):
        command = TMUX_STOP_ROUTER.format(router_id=router_id);
        subprocess.run(command.split())

    def start(self):
        subprocess.run(TMUX_SESSION.split())
        for router_id in self.routers:
            self.start_router(router_id)

        # We will block on this until we detach or exit.
        subprocess.run(TMUX_ATTACH.split())

        for router_id in self.routers:
            self.stop_router(router_id)

        subprocess.run(TMUX_STOP_CONTROLLER.split())

