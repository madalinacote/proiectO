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
	
	char buffer[BUFF_SIZE];
	char buffer2[BUFF_SIZE];
	char buffer3[BUFF_SIZE1];

	if(argc != 2)  //daca avem prea multe argumente
	{
		perror("prea multe argumente");
		exit(1);
	}

	char eroare[25] = "Usage: ";  //se formeaza mesajul de eroare conform cerintei
	strcat(eroare, argv[0]);
	strcat(eroare, " ");
	strcat(eroare, argv[1]);

    char *nume_fisier = "";
    int contor = 0;
    strcpy(nume_fisier, argv[1]);
    char *ext;
    ext = strtok(nume_fisier, ".");  //se separa extensia fisierului la .

	

    while(ext != NULL)
    {
        if(contor == 0)
        {
            contor++;
            ext = strtok(NULL, ".");
            continue;  //sare peste primul rand ca sa ajunga direct la extensia separata
        }
        if(!S_ISREG(var.st_mode) || (strcmp(ext, "bmp")) != 0)  //verifica daca este fisier si daca continutul lui ext corespunde cu extensia potrivita
	    {
            perror(eroare);
            exit(2);
	    }
    }

	if((fin = open(argv[1], O_RDONLY)) < 0)   
	{
		perror("nu s-a putut deschide fisierul de intrare");
        exit(3);
	}
    
    if((fout = open("statistica.txt", O_WRONLY | O_TRUNC | O_CREAT , S_IRWXU)) < 0)
	{
		perror("nu s-a putut deschide fisierul de iesire");
        exit(4);
	}

    sprintf(buffer3, "nume fisier: %s\n", argv[1]);  //scrie numele fisierului
	if(write(fout, buffer3, strlen(buffer3)) < 0)
	{
		perror("nu s-a putut efectua scrierea");
        exit(5);
	}

    lseek(fin, 2, SEEK_SET);  //sare peste primii 2 biti (irelevanti) din headerul fisierului
    if(read(fin, buffer2, BUFF_SIZE) != -1)  //citeste urmatorii BUFF_SIZE biti 
    {
        sprintf(buffer3, "dimensiunea fisierului: %lu\n", strtoul(buffer3, NULL, 2));  //extrage dimensiunea fisierului si o scrie in buffer3
        if(write(fout, buffer3, strlen(buffer3)) < 0)
	    {
		    perror("nu s-a putut efectua scrierea");
            exit(6);
	    }
    }

    lseek(fin, 12, SEEK_CUR);  //sare peste urmatorii 12 biti (irelevanti) din header
    if(read(fin, buffer2, BUFF_SIZE) != -1)  //citeste urmatorii BUFF_SIZE  biti
    {
        sprintf(buffer3, "latime: %lu\n", strtoul(buffer3, NULL, 2));  // scrie width ul imaginii in buffer3
        if(write(fout, buffer3, strlen(buffer3)) < 0)
	    {
		    perror("nu s-a putut efectua scrierea");
            exit(7);
	    }
    }

    if(read(fin, buffer2, BUFF_SIZE) != -1)
    {
        sprintf(buffer3, "inaltime: %lu\n", strtoul(buffer3, NULL, 2));   //scrie height ul imaginii in buffer3
        if(write(fout, buffer3, strlen(buffer3)) < 0)
	    {
		    perror("nu s-a putut efectua scrierea");
            exit(8);
	    }
    }

    lseek(fin, 8, SEEK_CUR);  //sare peste urmatorii 8 biti (irelevanti) din header
    if(read(fin, buffer2, BUFF_SIZE) != -1)  //citeste urmatorii BUFF_SIZE biti
    {
        sprintf(buffer3, "dimensiunea imaginii: %lu\n", strtoul(buffer3, NULL, 2));
        if(write(fout, buffer3, strlen(buffer3)) < 0)
	    {
		    perror("nu s-a putut efectua scrierea");
            exit(9);
	    }
    }

	if(fstat(fin, &var)) // fin este fisierul din care vrem sa aflam info
	{
		perror("eroare");
        exit(10);
	}
	else
	{
        sprintf(buffer3, "identificatorul utilizatorului: %d\n", var.st_uid);  //ia din var user id ul
	}
	time_t ultima_modificare = var.st_mtime;

    if(fstat(fin, &var)) // fin este fisierul din care vrem sa aflam info
	{
		perror("eroare");
        exit(10);
	}
	else
	{
        sprintf(buffer3, "data ultimei modificari: %s\n", ctime(&ultima_modificare));  //ia din var data ultimei modificari
	}

	if(!S_ISLNK(var.st_mode))  // verifica daca este legatura
	{
		perror("nu exista legaturi");
        exit(11);
	}
	else
	{
		if(stat(argv[1], &var))
			sprintf(buffer3, "contorul de legaturi: %ld\n", var.st_nlink); //se numara legaturile si se afiseaza
	}
	
	if(fstat(fin, &var))
	{
		perror("nproprietarul nu are drept de acces");
		exit(12);
	}
	else
	{
		sprintf(buffer3, "drepturi de acces user: %o\n", var.st_mode & (S_IRUSR | S_IWUSR | S_IXUSR));
	}
	
	if(fstat(fin, &var))
	{
		perror("grupul nu are drept de acces");
		exit(13);
	}
	else
	{
		sprintf(buffer3, "drepturi de acces grup: %o\n", var.st_mode & (S_IRGRP | S_IWGRP | S_IXGRP));
	}

	if(fstat(fin, &var))
	{
		perror("ceilalti utilizatori nu au drept de acces");
		exit(14);
	}
	else
	{
		sprintf(buffer3, "drepturi de acces altii: %o\n", var.st_mode & (S_IROTH | S_IWOTH | S_IXOTH));
	}


	close(fin);
	close(fout);

	return 0;
}
