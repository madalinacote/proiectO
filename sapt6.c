//proiect -- document sapt 6


#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <time.h>

#define BUFF_SIZE 4
#define BUFF_SIZE1 512

int main(int argc, char* argv[])
{
	int fin, fout, rd;
    struct stat var;
	
	char buffer2[BUFF_SIZE];
	char buffer3[BUFF_SIZE1];

    char eroare[50] = "Usage: ";  //se formeaza mesajul de eroare conform cerintei
	strcat(eroare, argv[0]);

	if(argc != 2)  //verifica numarul de argumente
	{
		perror("numar gresit de argumente");
		exit(1);
	}

	strcat(eroare, " ");  //se continua mesajul de eroare conform cerintei
	strcat(eroare, argv[1]);

    if((stat(argv[1], &var)) < 0) //verifica daca fisierul exista
    {
        perror("fisierul nu a fost gasit!");
        exit(0);
    }

    if(!S_ISREG(var.st_mode) || !(strstr(argv[1], ".bmp")) != 0)  //verifica daca este fisier si daca este de tipul bmp
	{
        perror(eroare);  //daca nu, se afiseaza mesajul de eroare
        exit(2);
	}

	if((fin = open(argv[1], O_RDONLY)) < 0)  //deschide fisierul de intrare
	{
		perror("nu s-a putut deschide fisierul de intrare");
        exit(3);
	}
    
    if((fout = open("statistica.txt", O_WRONLY | O_TRUNC | O_CREAT , S_IRUSR | S_IWUSR)) < 0)  //deschide fisierul de iesire
	{
		perror("nu s-a putut deschide fisierul de iesire");
        exit(4);
	}

    sprintf(buffer3, "nume fisier: %s\n", argv[1]);  //scrie numele fisierului in fisierul de iesire
	if(write(fout, buffer3, strlen(buffer3)) < 0)
	{
		perror("nu s-a putut efectua scrierea");
        exit(5);
	}

    lseek(fin, 2, SEEK_SET);  //sare peste primii 2 biti (irelevanti) din headerul fisierului
    if(read(fin, buffer2, 4) != -1)  //citeste urmatorii 4 biti 
    {
        //extrage dimensiunea fisierului din header, o converteste din char in unsigned si apoi o scrie in buffer
        sprintf(buffer3, "dimensiunea fisierului: %u octeti\n", (buffer2[0] | (buffer2[1] << 8) | (buffer2[2] << 16) | (buffer2[3] << 24)));  
        if(write(fout, buffer3, strlen(buffer3)) < 0)
	    {
		    perror("nu s-a putut efectua scrierea");
            exit(5);
	    }
    }
    else
    {
        perror("nu s-a putut efectua citirea!");
        exit(6);
    }

    lseek(fin, 12, SEEK_CUR);  //sare peste urmatorii 12 octeti (irelevanti) din header
    if(read(fin, buffer2, 4) != -1)  //citeste urmatorii 4  octeti
    {
        //extrage latimea imaginii din header, o converteste din char in unsigned si apoi o scrie in buffer
        sprintf(buffer3, "latime: %u\n",  (buffer2[0] | (buffer2[1] << 8) | (buffer2[2] << 16) | (buffer2[3] << 24)));
        if(write(fout, buffer3, strlen(buffer3)) < 0)
	    {
		    perror("nu s-a putut efectua scrierea");
            exit(5);
	    }
    }
    else
    {
        perror("nu s-a putut efectua citirea!");
        exit(6);
    }

    if(read(fin, buffer2, 4) != -1)  // citeste urmatorii 4 octeti
    {
        //extrage inaltimea imaginii din header, o converteste din char in unsigned si apoi o scrie in buffer
        sprintf(buffer3, "inaltime: %u\n",  (buffer2[0] | (buffer2[1] << 8) | (buffer2[2] << 16) | (buffer2[3] << 24)));
        if(write(fout, buffer3, strlen(buffer3)) < 0)
	    {
		    perror("nu s-a putut efectua scrierea");
            exit(5);
	    }
    }
    else
    {
        perror("nu s-a putut efectua citirea!");
        exit(6);
    }

    lseek(fin, 8, SEEK_CUR);  //sare peste urmatorii 8 octeti (irelevanti) din header
    if(read(fin, buffer2, 4) != -1)  //citeste urmatorii 4 octeti
    {
        //extrage dimensiunea imaginii din header, o converteste din char in unsigned si apoi o scrie in buffer
        sprintf(buffer3, "dimensiunea imaginii: %u octeti\n", (buffer2[0] | (buffer2[1] << 8) | (buffer2[2] << 16) | (buffer2[3] << 24)));
        if(write(fout, buffer3, strlen(buffer3)) < 0)
	    {
		    perror("nu s-a putut efectua scrierea");
            exit(5);
	    }
    }
    else
    {
        perror("nu s-a putut efectua citirea!");
        exit(6);
    }

	if((stat(argv[1], &var)) < 0)
	{
		perror("eroare");
        exit(7);
	}

    sprintf(buffer3, "identificatorul utilizatorului: %d\n", var.st_uid);  //ia din var id-ul userului
    if(write(fout, buffer3, strlen(buffer3)) < 0)
	{
	    perror("nu s-a putut efectua scrierea");
        exit(5);
	}
	time_t ultima_modificare = var.st_mtime;   //ia din var data ultimei modificari

    sprintf(buffer3, "data ultimei modificari: %s", ctime(&ultima_modificare));

    if(write(fout, buffer3, strlen(buffer3)) < 0)
    {
	    perror("nu s-a putut efectua scrierea");
        exit(5);
	}

    sprintf(buffer3, "contorul de legaturi: %ld\n", var.st_nlink); //ia din var contorul de legaturi

	if(write(fout, buffer3, strlen(buffer3)) < 0)
    {
	    perror("nu s-a putut efectua scrierea");
        exit(5);
	}

    char buffer[3] = "";
	
	if(var.st_mode & S_IRUSR)  //se verifica dreptul de citire al userului
		strcat(buffer, "R");
    else
		strcat(buffer, "-");
    if(var.st_mode & S_IWUSR)  //se verifica dreptul de scriere al userului
		strcat(buffer, "W");
    else
		strcat(buffer, "-");
    if(var.st_mode & S_IXUSR)  //se verifica dreptul de citire al userului
		strcat(buffer, "X");
    else
		strcat(buffer, "-");

    sprintf(buffer3, "drepturi de acces user: %s\n", buffer);
    if(write(fout, buffer3, strlen(buffer3)) < 0)
	{
	    perror("nu s-a putut efectua scrierea");
        exit(5);
	}

    strcpy(buffer, "");  //se goleste buffer-ul

    if(var.st_mode & S_IRGRP)  //se verifica dreptul de citire al userului
		strcat(buffer, "R");
    else
		strcat(buffer, "-");
    if(var.st_mode & S_IWGRP)  //se verifica dreptul de scriere al userului
		strcat(buffer, "W");
    else
		strcat(buffer, "-");
    if(var.st_mode & S_IXGRP)  //se verifica dreptul de executie al userului
		strcat(buffer, "X");
    else
		strcat(buffer, "-");
    
    sprintf(buffer3, "drepturi de acces grup: %s\n", buffer);
    if(write(fout, buffer3, strlen(buffer3)) < 0)
	{
	    perror("nu s-a putut efectua scrierea");
        exit(5);
	}

    strcpy(buffer, "");   //se goleste buffer-ul

    if(var.st_mode & S_IROTH)  //se verifica dreptul de citire al userului
		strcat(buffer, "R");
    else
		strcat(buffer, "-");
    if(var.st_mode & S_IWOTH)  //se verifica dreptul de scriere al userului
		strcat(buffer, "W");
    else
		strcat(buffer, "-");
    if(var.st_mode & S_IXOTH)  //se verifica dreptul de executie al userului
		strcat(buffer, "X");
    else
		strcat(buffer, "-");

    sprintf(buffer3, "drepturi de acces altii: %s\n", buffer);
    if(write(fout, buffer3, strlen(buffer3)) < 0)
	{
	    perror("nu s-a putut efectua scrierea");
        exit(5);
	}

    strcpy(buffer, "");  //se goleste buffer-ul

	close(fin);  //se inchide fisierul de intrare
	close(fout);  //se inchide fisierul de iesire

	return 0;
}