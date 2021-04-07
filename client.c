#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>
/* codul de eroare returnat de anumite apeluri */
extern int errno;

/* portul de conectare la server*/
int port;

int main (int argc, char *argv[])
{
  int sd,ok=1;		
  struct sockaddr_in server;	// structura folosita pentru conectare 
  char msg[100],msg1[100],top[100];		// mesajul trimis
  int nrr=0;
  /* exista toate argumentele in linia de comanda? */
  if (argc != 3)
    {
      printf ("[client] Sintaxa: %s <adresa_server> <port>\n", argv[0]);
      return -1;
    }

  port = atoi (argv[2]);

  if ((sd = socket (AF_INET, SOCK_STREAM, 0)) == -1)
    {
      perror ("[client] Eroare la socket().\n");
      return errno;
    }
  

  server.sin_family = AF_INET;
  server.sin_addr.s_addr = inet_addr(argv[1]);
  server.sin_port = htons (port);
  

  if (connect (sd, (struct sockaddr *) &server,sizeof (struct sockaddr)) == -1)
    {
      perror ("[client]Eroare la connect().\n");
      return errno;
    }

 /* Mesajul de inceput,dupa ce conexiunea s-a stabilit */
 printf("\n");
 printf("\033[0;93m");
 printf("-------->>>>>>>>>Connection has been estabilished<<<<<<<<-------\n");
 printf("\033[0m");
 printf("\n");
 printf("\033[1;34m");
 printf("List of Commands:_____________  \n");
 printf("                 [#1] register      \n");
 printf("                 [#2] login       \n");
 printf("                 [#3] exit       \n");
 printf("\033[0m");

        printf("\033[0m");
 printf("\n");
    /* Bucla infinita in care se fac cererile client-server */
     while(ok==1)
     {
       bzero (msg, 100);

       printf ("\e[38;5;143m[client] \e[38:5:245mType a command: ");
       printf("\033[0m");
       /* Folosit pentru a fi sigur de comanda primita din consola */
       fflush (stdout);
       read (0, msg, 100); 
 
       /* Se trimite comanda catre servere si se primeste raspuns */

       if (write (sd, msg, 100) <= 0)
       {
       perror ("[client]Eroare la write() spre server.\n");
       return errno;
       }
 
      if (read (sd, msg, 100) < 0)
     {
      perror ("[client]Eroare la read() de la server.\n");
      return errno;
     } 

      if(strncmp(msg,"9",1)==0)
     { /* Comanda exit */
       printf("\n");
       printf("\033[0;31m");
       printf("----------->>>>>>>>>The Connection Was Ended<<<<<<<<-----------\n");
       printf("\033[0m");
       printf("\n");
       /* Daca primeste cifra 9,atunci bucla infinita se va opri prin ok=0 */
       ok=0; 
     }
     else   
      if(strncmp(msg,"1",1)==0)
      { /* Comanda register */
        bzero(msg,100);
        printf("\n");
        printf("\033[0;33m");    
        printf("_______________Create an account__________\n");
        printf("\033[0m");
        printf("\n");
        /* Se introduce Username */
        printf("\e[38;5;101mUsername: " );
        printf("\033[0m");
        fgets(msg,sizeof(msg),stdin);
        msg[strlen(msg)-1]=0;

          if(write(sd,msg,100)<=0)
         {
           perror ("[client]Eroare la write() spre server.\n");
           return errno;
         }
        /* Folosit pentru a goli mesajul */
        bzero(msg,100);
         /* Se introduce parola */
        printf("\e[38;5;101mPassword: " );
        printf("\033[0m");
        fgets(msg,sizeof(msg),stdin);
        msg[strlen(msg)-1]=0;
 
         if(write(sd,msg,100)<=0)
         {
           perror ("[client]Eroare la write() spre server.\n");
           return errno;
         }

         bzero(msg,100);
         /* Raspunsul primit de server */
         if (read (sd, msg, 100) < 0)
         {
          perror ("[client]Eroare la read() de la server.\n");
          return errno;
         }

         if(strcmp(msg,"1")==0)
         {
          printf("\n");
         printf("\033[0;32m");
         printf("[#info]___Account created successfully____\n");
          printf("\033[0m");
         printf("\n");
         }
         else
         {
         printf("\n");
         printf("\e[38;5;196m[#info]___This username already exists____\n");
         printf("\n");
 
         }
      }
  else
      if(strncmp(msg,"2",1)==0)
     { /* Comanda login */
       printf("\n");
          printf("\033[0;33m");
       printf("__________________Login___________________\n");
         printf("\033[0m");
        printf("\n"); 
       printf("\e[");
       printf("\e[38;5;101mUsername: " );
        printf("\033[0m");
       bzero(msg,100);
       fgets(msg,sizeof(msg),stdin);
       msg[strlen(msg)-1]=0;
       char utilizator[30];
       strcpy(utilizator,msg);
       /* Se trimite Username */
       if(write(sd,msg,100)<=0)
       {
           perror ("[client]Eroare la write() spre server.\n");
           return errno;

       }
       bzero(msg,100);
       printf("\e[38;5;101mPassword: " );
        printf("\033[0m");
       bzero(msg,100);
       fgets(msg,sizeof(msg),stdin);
       msg[strlen(msg)-1]=0;
       /* Se trimite Password */
       if(write(sd,msg,100)<=0)
       {
           perror ("[client]Eroare la write() spre server.\n");
           return errno;

       }
       /* Se primeste raspuns user=1, admin=2 , owner=3  */
       bzero(msg,100);
       if (read (sd, msg, 100) < 0)
       {
         perror ("[client]Eroare la read() de la server.\n");
         return errno;
       }
       char NameUSER[30];
       strcpy(NameUSER,msg);
       if(strcmp(msg,"2")==0)
       {/* Daca este admin */
         bzero(msg,100);
         printf("\e[38;5;101mAdminCheckCode: " );
          printf("\033[0m");
         bzero(msg,100);
         fgets(msg,sizeof(msg),stdin);
         msg[strlen(msg)-1]=0;
         /* Se trimite CheckCode si se verifica */
         if(write(sd,msg,100)<=0)
         {
           perror ("[client]Eroare la write() spre server.\n");
           return errno;
         }
         bzero(msg,100);
         if (read (sd, msg, 100) < 0)
         {
           perror ("[client]Eroare la read() de la server.\n");
           return errno;
         }
         if(strcmp(msg,"1")==0)
         {/* Inseamna ca CheckCode este bun si este logat ca admin */
          printf("\n"); 
          printf("\033[0;32m");
          printf("[#info]___>>>>___Logged as admin___<<<<___\n");
          printf("\033[0m");
          printf("\033[1;34m");
          printf("\n");
           printf("List of Commands:_____________  \n");
      printf("                 [#1] delete user        \n");
      printf("                 [#2] delete song       \n");
      printf("                 [#3] restrict user       \n");
      printf("                 [#4] allow user       \n");
      printf("                 [#5] add song      \n");
      printf("                 [#6] vote song      \n");
      printf("                 [#7] top       \n");
      printf("                 [#8] tgenre       \n");
      printf("                 [#9] add comment      \n");
      printf("                 [#10] info song\n");
      printf("                 [#11] logout     \n");
          ok=2;
          printf("\033[0m");
          printf("\n"); 
          
         }
         else
         {
          	printf("\n"); 
      	 	printf("\033[0;31m");
      		printf("[#info]___>>>>____Login failed____<<<<____\n");
            printf("\033[0m");
      printf("\n"); 
         }
      }
   else
      if(strcmp(msg,"3")==0)
      {/* Daca este OWNER */
      printf("\n"); 
      printf("\033[0;32m");    
      printf("[#info]___>>>>___Logged as OWNER___<<<<___\n");
      printf("\033[0m");
      printf("\n");
      printf("\033[1;34m");
      printf("List of Commands:__________________ \n");
      printf("                 [#1] make admin      \n");
      printf("                 [#2] delete user        \n");
      printf("                 [#3] delete song       \n");
      printf("                 [#4] restrict user       \n");
      printf("                 [#5] allow user       \n");
      printf("                 [#6] add song      \n");
      printf("                 [#7] vote song      \n");
      printf("                 [#8] top       \n");
      printf("                 [#9] tgenre       \n");
      printf("                 [#10] add comment      \n");
      printf("                 [#11] info song\n");
      printf("                 [#12] logout     \n");
      printf("\n"); 
      printf("\033[0m");
       
      ok=2;
      }
      else
      if(strcmp(msg,"0")==0)
      {/* Daca logarea a esuat */
      printf("\n"); 
       printf("\033[0;31m");
      printf("[#info]___>>>>____Login failed____<<<<____\n");
            printf("\033[0m");
      printf("\n"); 
      }
      else
      if(strcmp(msg,"1")==0)
      {/* Daca este user */
        printf("\n");
        printf("\033[0;32m");
        printf("[#info]-->>>>-Connected succesfully-<<<<--\n");
        printf("\033[0m");
        printf("\n"); 
  
           printf("\033[1;34m");
          printf("List of Commands:____________  \n");
      printf("                 [#1] add song      \n");
      printf("                 [#2] vote song      \n");
      printf("                 [#3] top       \n");
      printf("                 [#4] tgenre       \n");
      printf("                 [#5] add comment      \n");
      printf("                 [#6] info song\n");
      printf("                 [#7] logout     \n");
        printf("\n");
            printf("\033[0m");        
        ok=2; 
      }
      

    /* Dupa ce s-a stabilit tipul contului si este logat va putea folosi urmatoarele comenzi */

     while(ok==2)
   {
     if(strcmp(NameUSER,"1")==0)
       {   
           printf ("\e[38;5;143m[User]\e[38;5;166m[%s]\e[38:5:245mType a command: ",utilizator);
       printf("\033[0m");
         
    }
     else
     if(strcmp(NameUSER,"2")==0)
 { printf ("\e[38;5;227m[*Admin]\e[38;5;143m[%s]\e[38:5:245mType a command: ",utilizator);
       printf("\033[0m");
}
     else
     {  printf ("\e[38;5;210m[#OWNER]\e[38;5;143m[%s]\e[38:5:245mType a command: ",utilizator);
       printf("\033[0m");
}

       bzero (msg, 100);
       fflush (stdout);
       fgets(msg,sizeof(msg),stdin);
       msg[strlen(msg)-1]=0; 

       /* Se trimite Comanda */
       if(write(sd,msg,100)<=0)
       {
          perror ("[client]Eroare la write() spre server.\n");
          return errno;

       }
       /* Se primeste o cifra in functie de comanda scrisa */
       bzero(msg,100); 
       if (read (sd, msg, 100) < 0)
       {
        perror ("[client]Eroare la read() de la server.\n");
        return errno;
       }
       fflush (stdout);

      if(strncmp(msg,"4",1)==0)
   { 	/* Se adauga o melodie */
         printf("\033[0;94m");
        printf ("_______________Add a song_________________ \n");
        printf("\033[0m");
        printf("\n");  
        printf("\e[38;5;101mName: " );
        printf("\033[0m");
        fgets(msg,sizeof(msg),stdin);
        msg[strlen(msg)-1]=0;
        if(write(sd,msg,100)<=0)
        {
           perror ("[client]Eroare la write() spre server.\n");
           return errno;
        }
        bzero(msg,100);  
        if (read (sd, msg, 100) < 0)
        {
          perror ("[client]Eroare la read() de la server.\n");
          return errno;
        }

        /* Artist */
        printf("\e[38;5;101mSinger:" );
        printf("\033[0m");
        fgets(msg,sizeof(msg),stdin);
        msg[strlen(msg)-1]=0;
     
        if(write(sd,msg,100)<=0)
        {
           perror ("[client]Eroare la write() spre server.\n");
           return errno;
        }
        bzero(msg,100);
      
        if (read (sd, msg, 100) < 0)
        {
          perror ("[client]Eroare la read() de la server.\n");
          return errno;
        }

        /* Descriere */
        printf("\e[38;5;101mDescription:" );
        printf("\033[0m");
        fgets(msg,sizeof(msg),stdin);
        msg[strlen(msg)-1]=0; 
        if(write(sd,msg,100)<=0)
        {
           perror ("[client]Eroare la write() spre server.\n");
           return errno;
        }
        bzero(msg,100); 
        if (read (sd, msg, 100) < 0)
        {
          perror ("[client]Eroare la read() de la server.\n");
          return errno;
        }
 
        /* Genul */
        printf("\e[38;5;101mType: " );
        printf("\033[0m");
        fgets(msg,sizeof(msg),stdin);
        msg[strlen(msg)-1]=0;
        if(write(sd,msg,100)<=0)
        {
           perror ("[client]Eroare la write() spre server.\n");
           return errno;

        }
        bzero(msg,100);     
        if (read (sd, msg, 100) < 0)
        {
          perror ("[client]Eroare la read() de la server.\n");
          return errno;
        }
        /* Link */
         printf("\e[38;5;101mLink " );
       printf("\033[0m");
        fgets(msg,sizeof(msg),stdin);
        msg[strlen(msg)-1]=0;
        if(write(sd,msg,100)<=0)
        {
           perror ("[client]Eroare la write() spre server.\n");
           return errno;

        }
        bzero(msg,100);     
        if (read (sd, msg, 100) < 0)
        {
          perror ("[client]Eroare la read() de la server.\n");
          return errno;
        }

       /* pentru mai multe genuri */
       int gen=1;
       while(gen<3)
{
        gen++;

         printf("\e[38;5;214mWant to add more genres? " );
        printf("\033[0m");
        fgets(msg,sizeof(msg),stdin);
        msg[strlen(msg)-1]=0;
        if(write(sd,msg,100)<=0)
        {
           perror ("[client]Eroare la write() spre server.\n");
           return errno;

        }
        bzero(msg,100);     
        if (read (sd, msg, 100) < 0)
        {
          perror ("[client]Eroare la read() de la server.\n");
          return errno;
        }

        if(strcmp(msg,"no")==0)
            gen=3;
        else
        if(strcmp(msg,"yes")==0)
       {
         printf("\e[38;5;208mEnter the Genre: " );
        printf("\033[0m");
        fgets(msg,sizeof(msg),stdin);
        msg[strlen(msg)-1]=0;
        if(write(sd,msg,100)<=0)
        {
           perror ("[client]Eroare la write() spre server.\n");
           return errno;

        }
        bzero(msg,100);     
        if (read (sd, msg, 100) < 0)
        {
          perror ("[client]Eroare la read() de la server.\n");
          return errno;
        }

        }

}
        /* Raspunsul primit de la server daca melodia a fost adaugata in baza de date */
        bzero(msg,100);
        if (read (sd, msg, 100) < 0)
        {
          perror ("[client]Eroare la read() de la server.\n");
          return errno;
        }
        
        if(strcmp(msg,"1")==0)
        {
          printf("\n");      
          printf("\e[38;5;106m[#info]  The Song was succesfully added  ! \n");    
          printf("\n");
        }
        else
        {
          printf("\n");
          printf("\e[38;5;160m[#info]  This song already exists ! \n");
          printf("\n");
        } 
   }
    else
      if(strncmp(msg,"5",1)==0)
      {
           /* Log out */
           bzero(msg,100);
           if (read (sd, msg, 100) < 0)
           {
              perror ("[client]Eroare la read() de la server.\n");
              return errno;
           }
           if(strcmp(msg,"1")==0)
           { 
              ok=1;
              printf("\n");
                   printf("\033[0;91m");
              printf ("__________________Logged OUT_______________\n");
             printf("\033[0m");
              printf("\n"); 
        
           }
   
       }
    else
      if(strncmp(msg,"6",1)==0)
      { /* Make admin */
        printf("\n"); 
        printf("\033[0;33m");
        printf ("________________Owner Acces________________\n");
        printf("\033[0m");  
        printf("\n"); 
    

        bzero(msg,100);
        printf("\e[38;5;113mWhat user do you want to make it admin?\n" );
        printf("\e[38;5;208mEnter the name: ");
         printf("\033[0m");
        fgets(msg,sizeof(msg),stdin);
        msg[strlen(msg)-1]=0;
        if(write(sd,msg,100)<=0)
        {
           perror ("[client]Eroare la write() spre server.\n");
           return errno;
        }

        /* Mesaj de verificare */
        bzero(msg,100);
        if (read (sd, msg, 100) < 0)
        {
           perror ("[client]Eroare la read() de la server.\n");
           return errno;
        }
        printf("\n");  
        if(strcmp(msg,"1")==0)
         {   printf("\033[0;32m");    
             printf ("[#info]->-The user has been made an ADMIN-<-\n");
             printf("\n");
             printf("\033[0m");
         }
        else
         {
             printf ("\e[38;5;160m[#info]-->>-The user is already an ADMIN-<<-\n");
             printf("\n");
             printf("\033[0m");
         }
          
         
   
      }
    else
      if(strncmp(msg,"8",1)==0)
      { /* Vote song */
        printf("\n"); 
        printf("\033[0;94m");
        printf("_______________Vote a song________________\n"); 
        printf("\033[0m");
        printf("\n"); 
 
        bzero(msg,100); 
        if (read (sd, msg, 100) < 0)
        {
          perror ("[client]Eroare la read() de la server.\n");
          return errno;
        } 
        if(strcmp(msg,"1")==0)
        {
          bzero(msg,100);
          printf("\e[38;5;113mWhat song do you want to vote for?\n" );
          printf("\e[38;5;208mEnter the name: ");
          printf("\033[0m");
          fgets(msg,sizeof(msg),stdin);
          msg[strlen(msg)-1]=0; 
          if(write(sd,msg,100)<=0)
          {
             perror ("[client]Eroare la write() spre server.\n");
             return errno;
          }
          bzero(msg,100);
          
             if (read (sd, msg, 100) < 0)
             {
             perror ("[client]Eroare la read() de la server.\n");
             return errno;
             }
        
            bzero(msg,100);
            /* Verificare daca s-a votat */
            if (read (sd, msg, 100) < 0)
            {
            perror ("[client]Eroare la read() de la server.\n");
            return errno;
            }
 
            if(strcmp(msg,"1")==0)
            { 
            printf("\n");  
         
            printf("\e[38;5;106m[#info]--->>>>>-Voted Succesfully-<<<<<---\n");
            printf("\033[0m");

            printf("\n"); 
              
            }
            else
            { 
            printf("\n");  

     
            printf("\e[38;5;196m[#info]-->>>-This song does not exist-<<<-\n");
            printf("\033[0m");
  
            printf("\n");  
            }

     }
     else
          {  printf("\n"); 
            printf("\e[38;5;93mThe admin blocked this command for you :( \n");
            printf("\n");  
            printf("\e[38;5;196m[#info]--->>>>-Not allowed to vote-<<<<---\n");
             printf("\033[0m");
            printf("\n"); 
 
            }
   
      }
     else
        if(strncmp(msg,"30",2)==0)
     {
         /* Comanda pentru a oferi drept de vot */
         printf("\n");  
          printf ("\e[38;5;184m_________________Admin acces______________\n");
         printf("\n"); 
    
        printf("\e[38;5;113mWho do you want to change permission to?\n" );
        printf("\e[38;5;208mEnter the name: ");
         printf("\033[0m");
         bzero(msg,100);
         fgets(msg,sizeof(msg),stdin);
         msg[strlen(msg)-1]=0;
         char nume1[100];
         strcpy(nume1,msg);
         /* Nume user */
         if(write(sd,msg,100)<=0)
         {
           perror ("[client]Eroare la write() spre server.\n");
           return errno;
         }
         /* Primite raspuns si verificare */
         if (read (sd, msg, 100) < 0)
         {
           perror ("[client]Eroare la read() de la server.\n");
           return errno;
         }      
         if(strcmp(msg,"1")==0) 
           {
             printf("\n"); 
            printf("\e[38;5;106m[#info]-->>-[\e[38;5;105m%s\e[38;5;106m] can vote -<<<--\n",nume1);
             printf("\n"); 
            }
         else
            {
            printf("\n"); 
            printf("\e[38;5;106m[#info]-->>-[\e[38;5;105m%s\e[38;5;106m] does not exist-<<<--\n",nume1);
            printf("\n"); 
            }

     }
    else
      if(strncmp(msg,"7",1)==0)
    {  /* Comanda restrict */
         printf("\n");  
         printf ("\e[38;5;184m_________________Admin acces______________\n");
         printf("\n"); 
    
        printf("\e[38;5;113mWho do you want to change permission to?\n" );
        printf("\e[38;5;208mEnter the name: ");
         printf("\033[0m");
         bzero(msg,100);
         fgets(msg,sizeof(msg),stdin);
         msg[strlen(msg)-1]=0;
         char nume1[100];
         strcpy(nume1,msg);

         /* Trimit numele userului */
         if(write(sd,msg,100)<=0)
         {
           perror ("[client]Eroare la write() spre server.\n");
           return errno;
         }
         /* Raspund de la server daca s-a modificat dreptul */
         if (read (sd, msg, 100) < 0)
         {
           perror ("[client]Eroare la read() de la server.\n");
           return errno;
         }      

         if(strcmp(msg,"1")==0) 
           {
             printf("\n"); 
            printf("\e[38;5;106m[#info]-->>-[\e[38;5;105m%s\e[38;5;106m] cannot vote anymore-<<<--\n",nume1);
             printf("\n"); 
            }
         else
            {
            printf("\n"); 
            printf("\e[38;5;106m[#info]-->>-[\e[38;5;105m%s\e[38;5;106m] does not exist-<<<--\n",nume1);
            printf("\n"); 
            }
            
            
    }
     else
       if(strcmp(msg,"21")==0)
      { /* Comanda Info song */
        bzero(msg,100);



          printf("\e[38;5;113mWhat is The Name Of The Song ? : \n" );
         printf("\e[38;5;208mEnter the name: ");
           printf("\033[0m");
          fgets(msg,sizeof(msg),stdin);
          msg[strlen(msg)-1]=0; 
          if(write(sd,msg,100)<=0)
          {
             perror ("[client]Eroare la write() spre server.\n");
             return errno;
          }

         /* Link-ul si descrierea */
          bzero(msg1,100);
        if (read (sd, msg1, 100) < 0)
        {
          perror ("[client]Eroare la read() de la server.\n");
          return errno;
        }

        printf("\e[38;5;225mLink: %s  \n",msg1);

        /* Numarul de comentarii */
           bzero(msg1,100);
        if (read (sd, msg1, 100) < 0)
        {
          perror ("[client]Eroare la read() de la server.\n");
          return errno;
        }
        printf("\e[38;5;102mThe Number of comments : \e[38;5;75m%s \n",msg1);
        
        nrr=atoi(msg1);
        /* Cat timp exista comentarii,clientul va primi de la server si le va afisa */
        int numar=0;
        while( numar <nrr)
        { 
          numar=numar+1;
          bzero(msg1,100);
          if (read (sd, msg1, 100) < 0)
          {
            perror ("[client]Eroare la read() de la server.\n");
            return errno;
          }
          printf("\e[38;5;137m%s \n",msg1);
          printf("\033[0m");
        }  
       printf("\n"); 
      }
    else
      if(strcmp(msg,"19")==0)
      { /* Comanda Tgenre */
        bzero(msg,100);


          printf("\e[38;5;208mEnter the Genre: ");
           printf("\033[0m"); 
          fgets(msg,sizeof(msg),stdin);
          msg[strlen(msg)-1]=0; 
          if(write(sd,msg,100)<=0)
          {
             perror ("[client]Eroare la write() spre server.\n");
             return errno;
          }

           bzero(msg1,100);
        if (read (sd, msg1, 100) < 0)
        {
          perror ("[client]Eroare la read() de la server.\n");
          return errno;
        }

        printf("\n");
        printf("\e[38;5;107m Top      <->      Name      <->        Singer         <->     Votes  <->      GENRE \n");
        printf("_________________________________________________________________________________________________\n");
        nrr=atoi(msg1);
       
        int numar=0;
        while( numar <nrr)
        { 
          numar=numar+1;
          bzero(msg1,100);
          if (read (sd, msg1, 100) < 0)
          {
            perror ("[client]Eroare la read() de la server.\n");
            return errno;
          }

          
          if(numar==1)
          printf("\e[38;5;203m%d                %s \n",numar,msg1);
          else
          if(numar==2)
          printf("\e[38;5;227m%d                %s \n",numar,msg1);
          else
          if(numar==3)
          printf("\e[38;5;99m%d                %s \n",numar,msg1);
          else
          printf("\e[38;5;253m%d                %s \n",numar,msg1);
        } 
 	printf("\n");  
      }
       else
           if(strcmp(msg,"10")==0)
           { /* Comanda add comment */

              bzero(msg,100);

             printf("\n");  
             printf ("\e[38;5;184m________________Add a Comment_____________\n");
             printf("\n"); 
   

          printf("\e[38;5;113mWhat is the name of song?\n" );
        printf("\033[0m");
          fgets(msg,sizeof(msg),stdin);
          msg[strlen(msg)-1]=0; 
          /* Trimit numele melodiei */
          if(write(sd,msg,100)<=0)
          {
             perror ("[client]Eroare la write() spre server.\n");
             return errno;
          }
            /* Verific daca exista melodia */
             bzero(msg,100);

            if (read (sd, msg, 100) < 0)
            {
            perror ("[client]Eroare la read() de la server.\n");
            return errno;
            }
            /* Daca melodia exista,ii adaug comentariu */
            if(strcmp(msg,"1")==0)
            { 
                printf("\e[38;5;208mEnter the comment: ");
                printf("\033[0m");      
                fgets(msg,sizeof(msg),stdin);
                msg[strlen(msg)-1]=0; 
                if(write(sd,msg,100)<=0)
               {
                perror ("[client]Eroare la write() spre server.\n");
                return errno;
               }
            printf("\n"); 
            printf("\e[38;5;112m[#info]---->>>>-Successfully added-<<<<---\n");
            printf("\n"); 
            }
            else
            { 
            printf("\n");  
            printf("\e[38;5;196m[#info]-->>>-This song does not exist-<<<--\n");
            printf("\n"); 

            }


       

            
          }
      else
       if(strcmp(msg,"12")==0)
      {/* Comanda Top  */
        bzero(msg,100);
        if (read (sd, msg1, 100) < 0)
        {
          perror ("[client]Eroare la read() de la server.\n");
          return errno;
        }
        printf("\n");
        printf("\e[38;5;107m Top      <->      Name      <->        Singer         <->     Votes  <->    GENRE \n");
        printf("______________________________________________________________________________________________\n");
        nrr=atoi(msg1);
        int numar=0;
        while( numar <nrr)
        { 
          numar=numar+1;
          bzero(msg1,100);
          if (read (sd, msg1, 100) < 0)
          {
            perror ("[client]Eroare la read() de la server.\n");
            return errno;
          }
          
          if(numar==1)
          printf("\e[38;5;203m%d                %s ",numar,msg1);
          else
          if(numar==2)
          printf("\e[38;5;227m%d                %s ",numar,msg1);
          else
          if(numar==3)
          printf("\e[38;5;99m%d                %s ",numar,msg1);
          else
          printf("\e[38;5;253m%d                %s ",numar,msg1);

         bzero(msg1,100);
          if (read (sd, msg1, 100) < 0)
          {
            perror ("[client]Eroare la read() de la server.\n");
            return errno;
          }

          int dd=atoi(msg1);

   
          
          int gen=0;
          while( gen <dd)
        { 
         gen=gen+1;
          bzero(msg1,100);
          if (read (sd, msg1, 100) < 0)
          {
            perror ("[client]Eroare la read() de la server.\n");
            return errno;
          }
          printf("%s ",msg1);
}
	printf("\n");
           
        }  
          printf("\n");
      }
     else
       if(strcmp(msg,"17")==0)
    {   /* Comanda Delete user */
        printf("\n");  
        printf("\n");  
        printf ("\e[38;5;184m_________________Admin acces______________\n");
        printf("\n"); 
    
        printf("\e[38;5;113mWhat user do you want to delete?\n" );
        printf("\e[38;5;208mEnter the name: ");
        printf("\033[0m");
        bzero(msg,100);
        fgets(msg,sizeof(msg),stdin);
        msg[strlen(msg)-1]=0;
        char nume1[100];
        strcpy(nume1,msg);
        /* Numele user-ului de sters */
        if(write(sd,msg,100)<=0)
        {
          perror ("[client]Eroare la write() spre server.\n");
          return errno;
        } 
        if (read (sd, msg, 100) < 0)
        {
          perror ("[client]Eroare la read() de la server.\n");
          return errno;
        }
        /* Verificare */
        if(strcmp(msg,"1")==0)
       {
          printf("\n"); 
          printf("\e[38;5;106m[#info]-->>-The user [\e[38;5;105m%s\e[38;5;106m] was deleted-<<<--\n",nume1);
          printf("\n"); 
        }
        else
{

        printf("\n"); 
        printf("\e[38;5;196m[#info]--=->>>-This user does not exist-<<<---\n");
        printf("\n"); 
  
}
         
     }

     else
       if(strcmp(msg,"15")==0)
       {/* Comanda Delete song */
         printf("\n");  
         printf ("\e[38;5;184m_________________Admin acces______________\n");
         printf("\n"); 

         printf("\e[38;5;113mWhat Song Do You Want To Delete?\n" );
         printf("\e[38;5;208mEnter the name: ");
         printf("\033[0m");
         bzero(msg,100);
         fgets(msg,sizeof(msg),stdin);
         msg[strlen(msg)-1]=0;
         char nume1[100];
         strcpy(nume1,msg);
         /* Numele melodiei */
         if(write(sd,msg,100)<=0)
         {
           perror ("[client]Eroare la write() spre server.\n");
           return errno;
         }  
         if (read (sd, msg, 100) < 0)
         {
           perror ("[client]Eroare la read() de la server.\n");
           return errno;
         }

         if(strcmp(msg,"1")==0)    
          {   
          printf("\n"); 
          printf("\e[38;5;106m[#info]-->>-The song [\e[38;5;105m%s\e[38;5;106m] was deleted-<<<--\n",nume1);
          printf("\n"); 
          }
         else
         {
             printf("\n"); 
             printf("\e[38;5;196m[#info]-->>-This song doesn't exist-<<<---\n");
             printf("\n"); 
          }

         
      }
     else
       printf("\e[38;5;58m[#info]This is not a valid command ! \n");
  }
  
}


   else
      printf("\e[38;5;58m[#info]This is not a valid command ! \n");

 


 
}
 close(sd);
 exit(1); /* inchidem conexiunea, am terminat */

}
