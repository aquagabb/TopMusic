#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sqlite3.h>

/*portul folosit */
#define PORT 2021

struct sockaddr_in server;	// structura folosita de server
struct sockaddr_in from;
char msg[100], comanda[100];	//mesajul primit de la client 
char msgrasp[100] = " ", msgrasp1[100] = " ", msgrasp2[100] = " ", msgrasp3[100] = " ", msgrasp4[100] = " ", msgrasp5[100];	//mesaj de raspuns pentru client
int sd;	//descriptorul de socket 
int client, ok = 1, nradmin, numara, nrowner;
int length = sizeof(from);
char nume[20], parola[20], check[100], checkadmin[20];
int nrmelodii = 0, nrcomm = 0, nrgenuri = 0;
char linie[100];
char idcom[100];
sqlite3 * db;
char *zErrMsg = 0;
int rc;
char *sql;
char sqlOut[300];
const char *data = "Callback function called";

static int callback(void *NotUsed, int argc, char **argv, char **azColName)
{      /* Functia de callback standard */
	int i;
	for (i = 0; i < argc; i++)
	{
		printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
	}
	printf("\n");
	return 0;
}

static int numaraMelodii(void *NotUsed, int argc, char **argv, char **azColName)
{       /* Functia de callback care numara melodiile din baza de date,ulterior pentru a putea fi trimise catre client */

	nrmelodii++;
	return 0;
}

static int numaraGENURI(void *NotUsed, int argc, char **argv, char **azColName)
{      /* Functia de callback care numara genurile muzicale,o melodie poate avea mai multe genuri */

	nrgenuri++;
	return 0;
}
static int callback5(void *NotUsed, int argc, char **argv, char **azColName)
{
	/* Functia de callback care trimite catre client linie cu linie toate genurile unei melodii */
	int i;
	for (i = 0; i < argc; i++)
	{
		strcat(linie, argv[i]);
		strcat(linie, "  /  ");
	}

	if (write(client, linie, 100) <= 0)
		perror("[server]Eroare la write() catre client.\n");

	bzero(linie, 100);
	printf("\n");

	return 0;
}
static int trimiteID(void *NotUsed, int argc, char **argv, char **azColName)
{
        /* Functia de callback folosita pentru a lua ID-ul melodiei sterse */
	strcat(idcom,argv[0]);

	return 0;
}
static int callback6(void *NotUsed, int argc, char **argv, char **azColName)
{
        /* Functia de callback care trimite link-ul si descrierea */
	int i;
	for (i = 0; i < argc - 1; i++)
	{
		strcat(linie, argv[i]);
		strcat(linie, "  \n");
		strcat(linie, "Description:");
		strcat(linie, argv[i + 1]);
	}

	if (write(client, linie, 100) <= 0)
		perror("[server]Eroare la write() catre client.\n");
	else
	{
		printf("\e[38;5;214m[server]Descrierea si link-ul au fost trimise cu succes !\n");
		printf("\033[0m");
	}
	bzero(linie, 100);
	printf("\n");

	return 0;
}
static int callback2(void *NotUsed, int argc, char **argv, char **azColName)
{      /* Functia de callback care trimite fiecare melodie in parte  */
	int i;

	for (i = 0; i < argc - 1; i++)
	{
		strcat(linie, argv[i]);
		strcat(linie, "                 ");
	}

	if (write(client, linie, 100) <= 0)
		perror("[server]Eroare la write() catre client.\n");

       /* Aici numar cate genuri are o melodie*/
	bzero(linie, 100);
	nrgenuri = 0;
	sprintf(sqlOut, "select *from genuri where idcomentariu='%s';", argv[3]);
	rc = sqlite3_exec(db, sqlOut, numaraGENURI, 0, &zErrMsg);
	char trimit[100];
	sprintf(trimit, "%d", nrgenuri);

	/*Trimit numarul de genuri ale melodiei respective */
	if (write(client, trimit, 100) <= 0)
		perror("[server]Eroare la write() catre client.\n");

        /* Folosesc callback 5 pt a trimite toate genurile melodiei din top dupa gen */
	sprintf(sqlOut, "select gen from genuri where idcomentariu='%s';", argv[3]);
	rc = sqlite3_exec(db, sqlOut, callback5, 0, &zErrMsg);
	nrgenuri = 0;

	printf("\n");

	return 0;
}

static int callback4(void *NotUsed, int argc, char **argv, char **azColName)
{      /* Functia de callback pentru a afisa melodiile standard din top */
	int i;

	for (i = 0; i < argc; i++)
	{
		strcat(linie, argv[i]);
		strcat(linie, "                 ");
	}
	if (write(client, linie, 100) <= 0)
	{
		perror("[server]Eroare la write() catre client.\n");
		/*continuam sa ascultam */
	}
	else
		printf("[server]Mesajul a fost trasmis cu succes.\n");

	bzero(linie, 100);
	printf("\n");

	return 0;
}

static int callback3(void *NotUsed, int argc, char **argv, char **azColName)
{       /* Functia de callback pentru a trimite comentariile unei melodii */
	int i;

	for (i = 0; i < argc - 1; i++)
	{
		strcat(linie, "[");
		strcat(linie, argv[i]);
		strcat(linie, "]");
		strcat(linie, "   :    ");
		strcat(linie, argv[i + 1]);
	}
	if (write(client, linie, 100) <= 0)
	{
		perror("[server]Eroare la write() catre client.\n");
	}

	printf("\e[38;5;214mS-a trimis un comentariu => %s ! \n ", linie);
	printf("\033[0m");
	bzero(linie, 100);

	return 0;
}

static int numaraComentarii(void *NotUsed, int argc, char **argv, char **azColName)
{      /* Functia de callback care numara cate comentarii are o melodie respectiva */

	nrcomm++;
	return 0;
}

int inserare_BD(char numele[20], char passwd[20])
{
       /* Functia de inserare pentru un user */
	rc = sqlite3_open("base.db", &db);

	if (rc)
	{
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
	}
	else

	{
		fprintf(stderr, "Opened database successfully\n");
	}

	sprintf(sqlOut, "INSERT INTO users(nume,parola,connected,rang,drepturi) values('%s','%s','Offline','User','Da');", numele, passwd);

	rc = sqlite3_exec(db, sqlOut, callback, 0, &zErrMsg);
	int numara = sqlite3_changes(db);
	if (numara == 1)
		return 1;
	else
		return 0;

	sqlite3_close(db);

}

int inserare_muzica(char melodia[20], char artist[20], char descriere[20], char genul[20], char link[100])
{
       /* Functia de inserare melodie  */
	rc = sqlite3_open("base.db", &db);

	if (rc)
	{
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
		//return(0);
	}
	else

	{
		fprintf(stderr, "Opened database successfully\n");
	}

	sql = "SELECT *from music7;";
	rc = sqlite3_exec(db, sql, numaraMelodii, 0, &zErrMsg);

	sprintf(sqlOut, "INSERT INTO music7(nume,artist,voturi,descriere,link,idcomentariu,schimba) values('%s','%s',0,'%s','%s',%d,'Da');", melodia, artist, descriere, link, nrmelodii);
	rc = sqlite3_exec(db, sqlOut, callback, 0, &zErrMsg);
	int numara = sqlite3_changes(db);

	if (numara == 1)
	{
		sprintf(sqlOut, "INSERT INTO genuri(gen,idcomentariu) values ('%s',%d)", genul, nrmelodii);
		rc = sqlite3_exec(db, sqlOut, callback, 0, &zErrMsg);
	}

	/* Se insereaza mai multe genuri */

	int gen = 1;
	while (gen < 3)
	{
		gen++;
		/* Se primeste Yes or No*/
		bzero(msgrasp5, 100);
		if (read(client, msgrasp5, 100) <= 0)
		{
			perror("[client]Eroare la read() spre server.\n");
			close(client);
		}
		printf("[server]Trimitem genul  inapoi...-%s-\n", msgrasp5);

		if (write(client, msgrasp5, 100) <= 0)
		{
			perror("[server]Eroare la write() catre client.\n");
		
		}
		else
			printf("[server]Mesajul a fost trasmis cu succes.\n");
               /* In cazul in care este no,va iesi din while,altfel va insera un gen introdus de client */
		if (strncmp(msgrasp5, "no", 2) == 0)
			gen = 3;
		else
		if (strncmp(msgrasp5, "yes", 2) == 0)
		{
			bzero(msgrasp5, 100);
			if (read(client, msgrasp5, 100) <= 0)
			{
				perror("[client]Eroare la read() spre server.\n");
				close(client);
			}


			sprintf(sqlOut, "INSERT INTO genuri(gen,idcomentariu) values ('%s',%d)", msgrasp5, nrmelodii);
			rc = sqlite3_exec(db, sqlOut, callback, 0, &zErrMsg);

			if (write(client, msgrasp5, 100) <= 0)
				perror("[server]Eroare la write() catre client.\n");

		}
	}
	nrmelodii = 0;

	if (numara == 1)
		return 1;
	else
		return 0;

	sqlite3_close(db);

}

/*codul de eroare returnat de anumite apeluri */
extern int errno;

void handler()
{
	wait(NULL);

}
int main()
{

	if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror("[server]Eroare la socket().\n");
		return errno;
	}

	/*pregatirea structurilor de date */
	bzero(&server, sizeof(server));
	bzero(&from, sizeof(from));

	/*umplem structura folosita de server */
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_port = htons(PORT);

	if (bind(sd, (struct sockaddr *) &server, sizeof(struct sockaddr)) == -1)
	{
		perror("[server]Eroare la bind().\n");
		return errno;
	}

	if (listen(sd, 5) == -1)
	{
		perror("[server]Eroare la listen().\n");
		return errno;
	}
	while (1)
	{

		pid_t copil;

		printf("[server]Asteptam la portul %d...\n", PORT);
		fflush(stdout);
		client = accept(sd, (struct sockaddr *) &from, &length);

		signal(SIGCHLD, handler);

		copil = fork();
		if (copil == 0)
		{

			/*eroare la acceptarea conexiunii de la un client */
			if (client < 0)
			{
				perror("[server]Eroare la accept().\n");
				continue;
			}

			while (ok == 1)
			{	

			}	

			/*am terminat cu acest client, inchidem conexiunea */

			close(client);
			exit(1);
		}
	}
	/*while */
} /*main */
