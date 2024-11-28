# Battleships
This is a semestral project for my university Computer architecture course and I worked on it in pair with [lmnek](https://github.com/lmnek). 
The project is written in the **C** language and uses the [MZ_APO](https://github.com/lmnek/battleships/tree/main/dokumentace) microcontroller. 
The topic is the game Battleships for two players, each playing on their microcontroller and communicating using the **TCP** protocol. 
The peripherals used are an LCD, rotary knobs, RGB LEDs, and a row of LEDs. 
The [documentation folder](https://github.com/lmnek/battleships/tree/main/dokumentace) includes a player's manual and state diagrams (in Czech).

## Configuration
- In the Makefile, change the IP address to the IP of the boards being used
- In the tcp.c file, change the SERVER_IP constant to the IP address of the other board (the peer)
- Run `make` and `make run` commands

## Manual
I have also created this manual as part of the assignement.

https://github.com/Rionit/battleships/blob/main/battleships_manual.png

## Video
Here is a showcase how it worked on two MZ_APO boards.

https://github.com/user-attachments/assets/5a7e73c3-579f-4dab-a390-e15e67024708

