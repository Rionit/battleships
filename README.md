# Battleships
This is the semestral project for my university Computer architecture course. The project is written in the **C** language and uses the [MZ_APO](https://github.com/lmnek/battleships/tree/main/dokumentace) microcontroller. 
The topic is the game Battleships for two players, each playing on their microcontroller and communicating using the **TCP** protocol. 
The peripherals used are an LCD, rotary knobs, RGB LEDs, and a row of LEDs. 
The [documentation folder](https://github.com/lmnek/battleships/tree/main/dokumentace) includes a player's manual and state diagrams (in Czech).

## Configuration
- In the Makefile, change the IP address to the IP of the boards being used
- In the tcp.c file, change the SERVER_IP constant to the IP address of the other board (the peer)
- Run `make` and `make run` commands
