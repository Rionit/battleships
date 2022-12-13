/*******************************************************************
  Battleships - semestral project on MZ_APO board.

  tcp.h      - header file for implementing TCP protocol

  Kryštof Gärtner, Filip Doležal

 *******************************************************************/

#ifndef TCP_H
#define TCP_H

void setup_connection(bool starts);
void connect_from();
void connect_to();

void send_response(int message);
void receive_coords(short *shotx, short *shoty);
int send_coord(int curx, int cury);
void send_ready();

void close_socket();

#endif
