#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <wiringPi.h>
#include <softPwm.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "MRPII.h"
typedef unsigned char uint8_t;
#define _BV(bit) (1 << (bit))

#define TRIG 22
#define ECHO 21
#define MOTORLATCH  0
#define MOTORCLK    1
#define MOTORENABLE 2
#define MOTORDATA   3
#define MOTOR_1_PWM 4
#define MOTOR_2_PWM 5
#define MOTOR_3_PWM 6
#define MOTOR_4_PWM 7

#define MOTOR1_A 2
#define MOTOR1_B 3
#define MOTOR2_A 1
#define MOTOR2_B 4
#define MOTOR4_A 0
#define MOTOR4_B 6
#define MOTOR3_A 5
#define MOTOR3_B 7

#define FORWARD  1
#define BACKWARD 2
#define BRAKE    3
#define RELEASE  4

static unsigned char latch_state;

void latch_tx(void)
{
   unsigned char i;
   
      digitalWrite(MOTORLATCH, LOW);

   digitalWrite(MOTORDATA, LOW);

   for (i=0; i<8; i++)
   {
      digitalWrite(MOTORCLK, LOW);

      if (latch_state & _BV(7-i))
      {
         digitalWrite(MOTORDATA, HIGH);
      }
      else
      {
         digitalWrite(MOTORDATA, LOW);
      }
      digitalWrite(MOTORCLK, HIGH);
   }
   digitalWrite(MOTORLATCH, HIGH);
}

void enable(void)
{
   wiringPiSetup();
   pinMode(TRIG, OUTPUT);
   pinMode(ECHO, INPUT);
   pinMode(MOTORLATCH,  OUTPUT);
   pinMode(MOTORENABLE, OUTPUT);
   pinMode(MOTORDATA,   OUTPUT);
   pinMode(MOTORCLK,    OUTPUT);

   //TRIG pin must start LOW
   digitalWrite(TRIG, LOW);
   delay(20);
   latch_state = 0;

   latch_tx();

   digitalWrite(MOTORENABLE, LOW);
}

void DCMotorInit(uint8_t num)
{
   switch (num)
   {
      case 1:
         latch_state &= ~_BV(MOTOR1_A) & ~_BV(MOTOR1_B);
         latch_tx();
         break;
      case 2:
         latch_state &= ~_BV(MOTOR2_A) & ~_BV(MOTOR2_B);
         latch_tx();
         break;
      case 3:
         latch_state &= ~_BV(MOTOR3_A) & ~_BV(MOTOR3_B);
         latch_tx();
         break;
      case 4:
         latch_state &= ~_BV(MOTOR4_A) & ~_BV(MOTOR4_B);
         latch_tx();
         break;
   }
}

void DCMotorRun(uint8_t motornum, uint8_t cmd)
{
   uint8_t a, b;

   switch (motornum)
   {
      case 1:
         a = MOTOR1_A; b = MOTOR1_B;
         break;
      case 2:
         a = MOTOR2_A; b = MOTOR2_B;
         break;
      case 3:
         a = MOTOR3_A; b = MOTOR3_B;
         break;
      case 4:
         a = MOTOR4_A; b = MOTOR4_B;
         break;
	  default:
         return;
   }

   switch (cmd)
   {
      case FORWARD:
         latch_state |= _BV(a);
         latch_state &= ~_BV(b);
         latch_tx();
         break;
      case BACKWARD:
         latch_state &= ~_BV(a);
         latch_state |= _BV(b);
         latch_tx();
         break;
      case RELEASE:
         latch_state &= ~_BV(a);
         latch_state &= ~_BV(b);
         latch_tx();
       break;
   }
}




int getCM() {
       //delay(200);
	 //Send trig pulse
        digitalWrite(TRIG, HIGH);
        delayMicroseconds(60);
        digitalWrite(TRIG, LOW);

        //Wait for echo start
        while(digitalRead(ECHO) == LOW);

        //Wait for echo end
        long startTime = micros();
        while(digitalRead(ECHO) == HIGH);
        long travelTime = micros() - startTime;

        //Get distance in cm
        int distance = travelTime / 58;
       // printf("Distance: %dcm\n", distance);
        return distance;
}

void left(unsigned int t)
{
   DCMotorRun(2, BACKWARD);
   digitalWrite(MOTOR_2_PWM, 1);
   DCMotorRun(3, FORWARD);
   digitalWrite(MOTOR_3_PWM, 1);
   DCMotorRun(1, BACKWARD);
   digitalWrite(MOTOR_1_PWM, 1);
   DCMotorRun(4, FORWARD);
   digitalWrite(MOTOR_4_PWM, 1);
 delay(t);
   DCMotorRun(1, RELEASE);
   DCMotorRun(2, RELEASE);
   DCMotorRun(3, RELEASE);
   DCMotorRun(4, RELEASE);
}

void lleft()
{
   DCMotorRun(2, BACKWARD);
   digitalWrite(MOTOR_2_PWM, 1);
   DCMotorRun(3, FORWARD);
   digitalWrite(MOTOR_3_PWM, 1);
   DCMotorRun(1, BACKWARD);
   digitalWrite(MOTOR_1_PWM, 1);
   DCMotorRun(4, FORWARD);
   digitalWrite(MOTOR_4_PWM, 1);
}

void right(unsigned int t)
{
   DCMotorRun(1, FORWARD);
   digitalWrite(MOTOR_1_PWM, 1);
   DCMotorRun(2, FORWARD);
   digitalWrite(MOTOR_2_PWM, 1);
   DCMotorRun(3, BACKWARD);
   digitalWrite(MOTOR_3_PWM, 1);
   DCMotorRun(4, BACKWARD);
   digitalWrite(MOTOR_4_PWM, 1);
 delay(t);
   DCMotorRun(1, RELEASE);
   DCMotorRun(2, RELEASE);
   DCMotorRun(3, RELEASE);
   DCMotorRun(4, RELEASE);
}

void rright()
{
   DCMotorRun(1, FORWARD);
   digitalWrite(MOTOR_1_PWM, 1);
   DCMotorRun(2, FORWARD);
   digitalWrite(MOTOR_2_PWM, 1);
   DCMotorRun(3, BACKWARD);
   digitalWrite(MOTOR_3_PWM, 1);
   DCMotorRun(4, BACKWARD);
   digitalWrite(MOTOR_4_PWM, 1);
}

void backwd(unsigned int t)
{
   DCMotorRun(1, FORWARD);
 digitalWrite(MOTOR_1_PWM, 1);
   DCMotorRun(2, BACKWARD);
   digitalWrite(MOTOR_2_PWM, 1);

   DCMotorRun(3, FORWARD);
   digitalWrite(MOTOR_3_PWM, 1);
   DCMotorRun(4, BACKWARD);
   digitalWrite(MOTOR_4_PWM, 1);
   delay(t);

   DCMotorRun(1, RELEASE);
   DCMotorRun(2, RELEASE);
   DCMotorRun(3, RELEASE);
   DCMotorRun(4, RELEASE);
 

}

void bbackwd()
{
   DCMotorRun(1, FORWARD);
 digitalWrite(MOTOR_1_PWM, 1);
   DCMotorRun(2, BACKWARD);
   digitalWrite(MOTOR_2_PWM, 1);

   DCMotorRun(3, FORWARD);
   digitalWrite(MOTOR_3_PWM, 1);
   DCMotorRun(4, BACKWARD);
   digitalWrite(MOTOR_4_PWM, 1);
} 


void forwd(unsigned int t)
{
   DCMotorRun(1, BACKWARD);
   digitalWrite(MOTOR_1_PWM, 1);
   DCMotorRun(2, FORWARD);
   digitalWrite(MOTOR_2_PWM, 1);

   DCMotorRun(3, BACKWARD);
   digitalWrite(MOTOR_3_PWM, 1);
   DCMotorRun(4, FORWARD);
   digitalWrite(MOTOR_4_PWM, 1);
   delay(t);

   DCMotorRun(1, RELEASE);
   DCMotorRun(2, RELEASE);
   DCMotorRun(3, RELEASE);
   DCMotorRun(4, RELEASE);
}

void fforwd()
{
 DCMotorRun(1, BACKWARD);
   digitalWrite(MOTOR_1_PWM, 1);
   DCMotorRun(2, FORWARD);
 digitalWrite(MOTOR_2_PWM, 1);

   DCMotorRun(3, BACKWARD);
   digitalWrite(MOTOR_3_PWM, 1);
   DCMotorRun(4, FORWARD);
   digitalWrite(MOTOR_4_PWM, 1);
}

void stop()
{
  DCMotorRun(1, RELEASE);
   DCMotorRun(2, RELEASE);
   DCMotorRun(3, RELEASE);
   DCMotorRun(4, RELEASE);
}

int main(int argc, char *argv[])
{	
    if (wiringPiSetup () == -1)
   {
      fprintf (stdout, "oops: %s\n", strerror (errno)) ;
      return 1 ;
   }
   pinMode(MOTOR_1_PWM, OUTPUT);
   pinMode(MOTOR_2_PWM, OUTPUT);
   pinMode(MOTOR_3_PWM, OUTPUT);
   pinMode(MOTOR_4_PWM, OUTPUT);

   digitalWrite(MOTOR_1_PWM, 0);
   digitalWrite(MOTOR_2_PWM, 0);
   digitalWrite(MOTOR_3_PWM, 0);
   digitalWrite(MOTOR_4_PWM, 0);


   enable();

   DCMotorInit(1);
   DCMotorInit(2);
   DCMotorInit(3);
   DCMotorInit(4);


    int socket_desc , client_sock , c , read_size;
    struct sockaddr_in server , client;
    char client_message[2000];
     
    //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");
  int reuse = 1;
    if (setsockopt(socket_desc, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse, sizeof(reuse)) < 0)
        perror("setsockopt(SO_REUSEADDR) failed");

#ifdef SO_REUSEPORT
    if (setsockopt(socket_desc, SOL_SOCKET, SO_REUSEPORT, (const char*)&reuse, sizeof(reuse)) < 0) 
        perror("setsockopt(SO_REUSEPORT) failed");
#endif
     
    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr("192.168.1.105");
    server.sin_port = htons( 8888 );
     
    //Bind
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        //print the error message
        perror("bind failed. Error");
        return 1;
    }
    puts("bind done");
     
    //Listen
    listen(socket_desc , 3);
     
    //Accept and incoming connection
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);
     
    //accept connection from an incoming client
    client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
    if (client_sock < 0)
    {
        perror("accept failed");
        return 1;
    }
    puts("Connection accepted");
     
    //Receive a message from client
while( (read_size = recv(client_sock , client_message , 2000 , 0)) > 0 )
    {
        
//Send the message back to client_mess
	puts(client_message);
	if(strcmp( client_message, "1") == 0)
		{
			puts("odin");
			forwd(1000);
		}

		else if(strcmp( client_message,"2") == 0)
		{
			puts("dva");
			backwd(1000);
		}
		else if(strcmp(client_message,"3") ==0)
		{
			puts("tri");
			left(1000);
		}
		else if(strcmp(client_message,"4") ==0)
		{
			puts("chetire");
			right(1000);
		}
		else if(strcmp(client_message,"5") ==0)
		{
			puts("break");
			break;
		}
	memset(client_message,0,sizeof(client_message));
        
	}
     
    if(read_size == 0)
    {
        puts("Client disconnected");
        fflush(stdout);
    }
    else if(read_size == -1)
    {
        perror("recv failed");
    }
     
 
return 0;
}
