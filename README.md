**OSPF router simulator** written in C, featuring SPF routing, multi area support, dynamic topology changes, and a CLI.  
This is built to behave like a real routing engine while staying small, fast and easy to mess around with.

### Core Routing Engine
- Full **Longest Prefix Match (LPM)** lookup logic  
- Static and dynamically-learned OSPF routes  
- IP prefix model with loopbacks and LAN prefixes  
- Router-style CLI commands (`add route`, `delete route`, `lookup`, `show route`, etc.)

### OSPF Implementation
- Dijkstra’s **Shortest Path First (SPF)** algorithm  
- Link State Database (LSDB) with automatic recomputation  
- Fast convergence after topology changes (link up/down)  
- Multi area support with **Area 0 backbone**  
- ABR behavior and summary route generation  
- Dynamic topology manipulation (add/remove links)

### Topology Generator
- Random OSPF network generator  
- Reproducible seeds  
- Automatic allocation of loopbacks and LAN networks 
