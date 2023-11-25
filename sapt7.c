//proiect -- document sapt 7 -- directoare

#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <time.h>
#include <dirent.h>
#include <sys/types.h>

#define BUFF_SIZE 512
#define SIZE 300

void writeAccessRights(struct stat var, char buffer[BUFF_SIZE], int *fout)
{
    char bufferDrepturi[3] = "";
	
	if(var.st_mode & S_IRUSR)  //se verifica dreptul de citire al userului
		strcat(bufferDrepturi, "R");
    else
		strcat(bufferDrepturi, "-");
    if(var.st_mode & S_IWUSR)  //se verifica dreptul de scriere al userului
	    strcat(bufferDrepturi, "W");
    else
        strcat(bufferDrepturi, "-");
    if(var.st_mode & S_IXUSR)  //se verifica dreptul de citire al userului
	    strcat(bufferDrepturi, "X");
    else
		strcat(bufferDrepturi, "-");

    sprintf(buffer, "drepturi de acces user: %s\n", bufferDrepturi);
    if(write(*fout, buffer, strlen(buffer)) < 0)
	{
	    perror("nu s-a putut efectua scrierea drepturilor userului!");
        exit(5);
	}
	
	if(var.st_mode & S_IRGRP)  //se verifica dreptul de citire al grupului
		strcat(bufferDrepturi, "R");
    else
		strcat(bufferDrepturi, "-");
    if(var.st_mode & S_IWGRP)  //se verifica dreptul de scriere al grupului
	    strcat(bufferDrepturi, "W");
    else
        strcat(bufferDrepturi, "-");
    if(var.st_mode & S_IXGRP)  //se verifica dreptul de citire al grupului
	    strcat(bufferDrepturi, "X");
    else
		strcat(bufferDrepturi, "-");

    sprintf(buffer, "drepturi de acces grup: %s\n", bufferDrepturi);
    if(write(*fout, buffer, strlen(buffer)) < 0)
	{
	    perror("nu s-a putut efectua scrierea drepturilor grupului!");
        exit(5);
	}

    if(var.st_mode & S_IROTH)  //se verifica dreptul de citire al altora
		strcat(bufferDrepturi, "R");
    else
		strcat(bufferDrepturi, "-");
    if(var.st_mode & S_IWOTH)  //se verifica dreptul de scriere al altora
	    strcat(bufferDrepturi, "W");
    else
        strcat(bufferDrepturi, "-");
    if(var.st_mode & S_IXOTH)  //se verifica dreptul de citire al altora
	    strcat(bufferDrepturi, "X");
    else
		strcat(bufferDrepturi, "-");

    sprintf(buffer, "drepturi de acces altii: %s\n", bufferDrepturi);
    if(write(*fout, buffer, strlen(buffer)) < 0)
	{
	    perror("nu s-a putut efectua scrierea drepturilor altora!");
        exit(5);
	}

}

int main(int argc, char* argv[])
{
	int fin, finBMP, finFile, finLink, fout;
    struct stat var;
    
    char buffer2[BUFF_SIZE];
    char buffer3[BUFF_SIZE];

    char eroare[50] = "Usage: ";  //se formeaza mesajul de eroare conform cerintei
	strcat(eroare, argv[0]);

    if(argc != 2)  //verifica numarul de argumente
    {
        perror("numar gresit de argumente!");
	    exit(1);
	}

	strcat(eroare, " ");  //se continua mesajul de eroare conform cerintei
	strcat(eroare, argv[1]);

    if((stat(argv[1], &var)) < 0) //verifica daca fisierul exista
    {
        perror("fisierul nu a fost gasit!");
        exit(2);
    }

    if(!S_ISDIR(var.st_mode))
    {
        perror(eroare);
        exit(3);
    }

    DIR **dir;
    DIR *dirp;
    struct dirent *dirInp;
    struct stat target;

    char *cale;

    *dir = opendir(cale);

    if(*dir == NULL)
    {
        perror("eroare la deschiderea directorului!");
        exit(4);
    }

    if((fout = open("statistica.txt", O_WRONLY | O_TRUNC | O_CREAT , S_IRUSR | S_IWUSR)) < 0)  //deschide fisierul de iesire
	{
		perror("nu s-a putut deschide fisierul de iesire!");
        exit(4);
	}

    while((dirInp = readdir(dirp)) != NULL)  //se parcurge fiecare intrare
    {
        char cale[SIZE] = "";  //construim calea pentru fiecare intrare
        strcpy(cale, argv[1]);
        char numeFisier[SIZE] = "";
        strcpy(numeFisier, dirInp->d_name);  //scoatem numele din structura directorului
        strcpy(cale, "/");
        strcat(cale, numeFisier);

        if((lstat(cale, &var)) < 0)
        {
            perror("intrarea nu exista!");
            exit(5);
        }

        if(S_ISLNK(var.st_mode))
        {
            if((finLink = open(cale, O_RDONLY)) < 0)  //deschide fisierul de intrare
	        {
		        perror("nu s-a putut deschide fisierul de intrare!");
                exit(3);
	        }

            if((stat(cale, &target)) < 0)
            {
                perror("eroare la accesarea informatiilor despre legatura simbolica!");
                exit(4);
            }
            sprintf(buffer3, "numele legaturii simbolice: %s\n", dirInp->d_name);
            if(write(fout, buffer3, strlen(buffer3)) < 0)
	        {
		        perror("nu s-a putut efectua scrierea numelui legaturii!");
                exit(5);
	        }

            sprintf(buffer3, "dimensiunea legaturii simbolice: %lu\n", var.st_size);
            if(write(fout, buffer3, strlen(buffer3)) < 0)
	        {
		        perror("nu s-a putut efectua scrierea dimensiunii legaturii simbolice!");
                exit(5);
	        }

            sprintf(buffer3, "dimensiunea fisierului target: %ld\n", target.st_size);
            if(write(fout, buffer3, strlen(buffer3)) < 0)
	        {
		        perror("nu s-a putut efectua scrierea dimensiunii fisierului");
                exit(5);
	        }
            
            writeAccessRights(var, buffer3, &fout);
            close(finLink);
        }

        else
        {
            if(S_ISREG(var.st_mode))
            {
                if(strstr(cale, ".bmp") == 0)
                {
                    if((finBMP = open(cale, O_RDONLY)) < 0)  //deschide fisierul de intrare daca e .bmp
	                {
		                perror("nu s-a putut deschide fisierul de intrare");
                        exit(3);
	                }

                    sprintf(buffer3, "nume fisier: %s\n", dirInp->d_name);
	                if(write(fout, buffer3, strlen(buffer3)) < 0)
	                {
		                perror("nu s-a putut efectua scrierea numelui fisierului .bmp!");
                        exit(5);
	                }

                    lseek(finBMP, 2, SEEK_SET);
                    if(read(finBMP, buffer2, 4) != -1)
                    {
                        sprintf(buffer3, "dimensiunea fisierului: %lu octeti\n", var.st_size);  
                        if(write(fout, buffer3, strlen(buffer3)) < 0)
	                    {
		                    perror("nu s-a putut efectua scrierea dimensiunii fisierului .bmp!");
                            exit(5);
	                    }
                    }
                    else
                    {
                        perror("nu s-a putut efectua citirea fisierului .bmp!");
                        exit(6);
                    }

                    lseek(finBMP, 12, SEEK_CUR);
                    if(read(finBMP, buffer2, 4) != -1)
                    {
                        sprintf(buffer3, "latime: %u\n",  (buffer2[0] | (buffer2[1] << 8) | (buffer2[2] << 16) | (buffer2[3] << 24)));
                        if(write(fout, buffer3, strlen(buffer3)) < 0)
	                    {
		                    perror("nu s-a putut efectua scrierea latimii fisierului .bmp!");
                            exit(5);
	                    }
                    }
                    else
                    {
                        perror("nu s-a putut efectua citirea fisierului .bmp!");
                        exit(6);
                    }
                    
                    if(read(finBMP, buffer2, 4) != -1)
                    {
                        sprintf(buffer3, "inaltime: %u\n",  (buffer2[0] | (buffer2[1] << 8) | (buffer2[2] << 16) | (buffer2[3] << 24)));
                        if(write(fout, buffer3, strlen(buffer3)) < 0)
	                    {
		                    perror("nu s-a putut efectua scrierea  inaltimii fisierului .bmp!");
                            exit(5);
	                    }
                    }
                    else
                    {
                        perror("nu s-a putut efectua citirea fisierului .bmp!");
                        exit(6);
                    }

                    lseek(finBMP, 8, SEEK_CUR);
                    if(read(finBMP, buffer2, 4) != -1)
                    {
                        sprintf(buffer3, "dimensiunea imaginii: %u octeti\n", (buffer2[0] | (buffer2[1] << 8) | (buffer2[2] << 16) | (buffer2[3] << 24)));
                        if(write(fout, buffer3, strlen(buffer3)) < 0)
	                    {
		                    perror("nu s-a putut efectua scrierea dimensiunii imaginii .bmp!");
                            exit(5);
	                    }
                    }
                    else
                    {
                        perror("nu s-a putut efectua citirea fisierului .bmp!");
                        exit(6);
                    }

                    if((stat(cale, &var)) < 0)
	                {
		                perror("eroare");
                        exit(7);
	                }

                    sprintf(buffer3, "identificatorul utilizatorului: %d\n", var.st_uid);  //ia din var id-ul userului
                    if(write(fout, buffer3, strlen(buffer3)) < 0)
	                {
	                    perror("nu s-a putut efectua scrierea user id-ului fisierului .bmp!");
                        exit(5);
	                }

	                time_t ultima_modificare = var.st_mtime;   //ia din var data ultimei modificari

                    sprintf(buffer3, "data ultimei modificari: %s", ctime(&ultima_modificare));

                    if(write(fout, buffer3, strlen(buffer3)) < 0)
                    {
	                    perror("nu s-a putut efectua scrierea datei ultimei modificari a fisierului .bmp!");
                        exit(5);
	                }

                    sprintf(buffer3, "contorul de legaturi: %ld\n", var.st_nlink); //ia din var contorul de legaturi

	                if(write(fout, buffer3, strlen(buffer3)) < 0)
                    {
	                    perror("nu s-a putut efectua scrierea contorului de legaturi al fisierului .bmp!");
                        exit(5);
	                }

                    writeAccessRights(var, buffer3, &fout);

                    close(finBMP);
                }
                else
                {
                    if((finFile = open(cale, O_RDONLY)) < 0)  //deschide fisierul de intrare daca e .bmp
	                {
		                perror("nu s-a putut deschide fisierul de intrare");
                        exit(3);
	                }

                    sprintf(buffer3, "nume fisier: %s\n", dirInp->d_name);
	                if(write(fout, buffer3, strlen(buffer3)) < 0)
	                {
		                perror("nu s-a putut efectua scrierea numelui fisierului!");
                        exit(5);
	                }

                    lseek(fin, 2, SEEK_SET);
                    if(read(fin, buffer2, 4) != -1)
                    {
                        sprintf(buffer3, "dimensiunea fisierului: %lu octeti\n", var.st_size);  
                        if(write(fout, buffer3, strlen(buffer3)) < 0)
	                    {
		                    perror("nu s-a putut efectua scrierea dimensiunii fisierului!");
                            exit(5);
	                    }
                    }
                    else
                    {
                        perror("nu s-a putut efectua citirea fisierului!");
                        exit(6);
                    }

                    if((stat(cale, &var)) < 0)
	                {
		                perror("eroare");
                        exit(7);
	                }

                    sprintf(buffer3, "identificatorul utilizatorului: %d\n", var.st_uid);  //ia din var id-ul userului
                    if(write(fout, buffer3, strlen(buffer3)) < 0)
	                {
	                    perror("nu s-a putut efectua scrierea user id-ului fisierului!");
                        exit(5);
	                }

                    time_t ultima_modificare = var.st_mtime;   //ia din var data ultimei modificari

                    sprintf(buffer3, "data ultimei modificari: %s", ctime(&ultima_modificare));

                    if(write(fout, buffer3, strlen(buffer3)) < 0)
                    {
	                    perror("nu s-a putut efectua scrierea datei ultimei modificari a fisierului!");
                        exit(5);
	                }

                    sprintf(buffer3, "contorul de legaturi: %ld\n", var.st_nlink); //ia din var contorul de legaturi

	                if(write(fout, buffer3, strlen(buffer3)) < 0)
                    {
	                    perror("nu s-a putut efectua scrierea contorului de legaturi al fisierului!");
                        exit(5);
	                }

                    writeAccessRights(var, buffer3, &fout);
                    close(finFile);
                }
            }
            else
            {
                if(S_ISDIR(var.st_mode))
                {
                    DIR *intrare;

                    intrare = opendir(cale);

                    if(intrare == NULL)
                    {
                        perror("eroare la deschiderea directorului!");
                        exit(4);
                    }

                    sprintf(buffer3, "nume director: %s/n", dirInp->d_name);
                    if(write(fout, buffer3, strlen(buffer3)) < 0)
                    {
                        perror("nu s-a putut efectua scrierea numelui directorului!");
                        exit(5);
                    }

                    sprintf(buffer3, "identificatorul utilizatorului: %u\n", var.st_uid);
                    if(write(fout, buffer3, strlen(buffer3)) < 0)
                    {
                        perror("nu s-a putut efectua scrierea user id-ului pentru director!");
                        exit(5);
                    }

                    writeAccessRights(var, buffer3, &fout);

                    if((closedir(*dir)) != 0)
                    {
                        perror("eroare la inchiderea directorului");   
                        exit(9);
                    }                   
                }
            }
        }
    }

    close(fout);
    if((closedir(dirp)) != 0)
    {
        perror("eroare la inchiderea directorului");   
        exit(9);
    }

	return 0;
}