#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h> 		// descripteurs de fichiers ( open()...)
#include <termios.h>	// 
#include <errno.h>
#include <iostream>

/* baudrate settings are defined in <asm/termbits.h>, which is included by <termios.h> */
#define BAUDRATE B115200            
/* change this definition for the correct port */
#define MODEMDEVICE "/dev/ttyS0"
#define _POSIX_SOURCE 1 /* POSIX compliant source */

#define FALSE 0
#define TRUE 1

volatile int STOP=FALSE; 
int main()
{
  	int sfd, c, res;
    struct termios newtio;
    char buf[255];
    ssize_t size;
	
	sfd = open(MODEMDEVICE, O_RDONLY | O_NOCTTY ); 
	if (sfd == -1)
	{
	  printf ("Error  no is : %d\n", errno);
	  printf("Error description is : %s\n",strerror(errno));
	  return(-1);
	}
    bzero(&newtio, sizeof(newtio)); /* clear struct for new port settings */
 /* 
          BAUDRATE: Set bps rate. You could also use cfsetispeed and cfsetospeed.
          CRTSCTS : output hardware flow control (only used if the cable has
                    all necessary lines. See sect. 7 of Serial-HOWTO)
          CS8     : 8n1 (8bit,no parity,1 stopbit)
          CLOCAL  : local connection, no modem contol
          CREAD   : enable receiving characters
        */
    newtio.c_cflag = BAUDRATE | CRTSCTS | CS8 | CLOCAL | CREAD;
         
        /*
          IGNPAR  : ignore bytes with parity errors
          ICRNL   : map CR to NL (otherwise a CR input on the other computer
                    will not terminate input)
          otherwise make device raw (no other input processing)
        */
    newtio.c_iflag = IGNPAR | ICRNL;
         
       /*
          ICANON  : enable canonical input
          disable all echo functionality, and don't send signals to calling program
        */
    newtio.c_lflag = ICANON;
         

	/* 
	  now clean the modem line and activate the settings for the port
	*/
	 tcflush(sfd, TCIFLUSH);
	 tcsetattr(sfd,TCSANOW,&newtio);
	
	for (int i =0 ; i<50 ; i++)
	{
		
	/*
	  ** On lit :
	  ** on passe a read :
	  ** - le fd,
	  ** - le buffer
	  ** - la taille du buffer
	  ** Attention si tu passe une taille de buffer plus grande que la taille de ton buffer,
	  ** ton programme deviens sensible aux Buffer Overflow
	  */
	  size = read (sfd, buf, 127);
   
	  /*
	  ** On raoute un '\0' à la fin de la chaine lut, pour être sur d'avoir une chaine de caractères valide.
	  ** size correspondant a l'index du dernier caractere du buffer + 1.
	  ** Ceci est utile si tu veux utiliser ta chaine dans une fonction comme strcmp() ou printf()
	  */
 
	  buf[size] = 0;
   
	  /*
	  ** On affiche ce que l'on viens de lire dans la console :
	  ** NOTE :
	  ** il existe des FD speciaux :
	  ** Le fd 1 est la sortie standart ( console )
	  */
 
	  //write (1, buf, size);
	  std::cout << i << " " << buf << "\n";
	  }
 
  /* Ne pas oublier de libérer ton file descriptor */
  close(sfd);
 
  return 0;
}