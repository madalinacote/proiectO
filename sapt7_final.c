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

void writeAccessRights(struct stat var, int fout)
{
    char buffer[BUFF_SIZE];

    char bufferDrepturi[4] = "";

    if (var.st_mode & S_IRUSR) 
        strcat(bufferDrepturi, "R");
    else
        strcat(bufferDrepturi, "-");
    if (var.st_mode & S_IWUSR)
        strcat(bufferDrepturi, "W");
    else
        strcat(bufferDrepturi, "-");
    if (var.st_mode & S_IXUSR)
        strcat(bufferDrepturi, "X");
    else
        strcat(bufferDrepturi, "-");

    sprintf(buffer, "drepturi de acces user: %s\n", bufferDrepturi);
    if (write(fout, buffer, strlen(buffer)) < 0)
    {
        perror("nu s-a putut efectua scrierea drepturilor userului!");
        exit(5);
    }

    strcpy(bufferDrepturi, "");

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
    if(write(fout, buffer, strlen(buffer)) < 0)
	{
	    perror("nu s-a putut efectua scrierea drepturilor grupului!");
        exit(5);
	}
    strcpy(bufferDrepturi, "");

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

    strcat(buffer, "\n");
    if(write(fout, buffer, strlen(buffer)) < 0)
	{
	    perror("nu s-a putut efectua scrierea drepturilor altora!");
        exit(5);
	}
    strcpy(bufferDrepturi, "");

}

int main(int argc, char* argv[])
{
	int fin, fout;
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

    DIR* dir;
    struct dirent *dirInp;

    dir = opendir(argv[1]);

    if(dir == NULL)
    {
        perror("eroare la deschiderea directorului!");
        exit(4);
    }

    if((fout = open("statistica.txt", O_WRONLY | O_TRUNC | O_CREAT , S_IRUSR | S_IWUSR)) < 0)  //deschide fisierul de iesire
	{
		perror("nu s-a putut deschide fisierul de iesire!");
        exit(4);
	}

    while ((dirInp = readdir(dir)) != NULL) 
    {
        char cale_completa[SIZE] = "";
        strcpy(cale_completa, argv[1]);
        strcat(cale_completa, "/");
        strcat(cale_completa, dirInp->d_name);

        fin = open(cale_completa, O_RDONLY);

        if (lstat(cale_completa, &var) < 0) 
        {
            perror("eroare la obtinerea informatiilor despre directorul de intrare!");
            exit(4);
        }

        //printf("Cale completa: %s\n", cale_completa);
    
        if (S_ISLNK(var.st_mode))
        {
            //legatura simbolica
            struct stat target;
            if (lstat(cale_completa, &target) < 0)
            {
                perror("eroare la accesarea informatiilor despre legatura simbolica!");
                exit(4);
            }
            
            sprintf(buffer3, "numele legaturii simbolice: %s\n", dirInp->d_name);
            if (write(fout, buffer3, strlen(buffer3)) < 0)
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
            
            writeAccessRights(target, fout);
        }

        else
        {
            if(S_ISREG(var.st_mode))
            {
                //fisier .bmp
                if(strstr(cale_completa, ".bmp") != NULL)
                {
                    sprintf(buffer3, "nume fisier: %s\n", dirInp->d_name);
	                if(write(fout, buffer3, strlen(buffer3)) < 0)
	                {
		                perror("nu s-a putut efectua scrierea numelui fisierului .bmp!");
                        exit(5);
	                }

                    lseek(fin, 2, SEEK_SET);  //sare peste primii 2 biti (irelevanti) din headerul fisierului
                    __uint32_t dim_fisier;
                    if(read(fin, &dim_fisier, sizeof(dim_fisier)) != -1)  //citeste urmatorii 4 biti 
                    {
                        //extrage dimensiunea fisierului din header, o converteste din char in unsigned si apoi o scrie in buffer
                        sprintf(buffer3, "dimensiunea fisierului: %u octeti\n", dim_fisier);  
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

                    lseek(fin, 12, SEEK_CUR);  // Sare la octetul 18 din header
                    __uint32_t latime, inaltime;
                    if (read(fin, &latime, sizeof(latime)) != -1)  // Citeste urmatorii 4 octeti
                    {
                        // Extrage latimea imaginii din header, o converteste din char in unsigned si apoi o scrie in buffer
                        sprintf(buffer3, "latime: %u\n",  latime);
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
                    
                    if(read(fin, &inaltime, sizeof(inaltime)) != -1)  // citeste urmatorii 4 octeti
                    {
                        //extrage inaltimea imaginii din header, o converteste din char in unsigned si apoi o scrie in buffer
                        sprintf(buffer3, "inaltime: %u\n",  inaltime);
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
                    __uint32_t dim_img;
                    if(read(fin, &dim_img, sizeof(dim_img)) != -1)  //citeste urmatorii 4 octeti
                    {
                        //extrage dimensiunea imaginii din header, o converteste din char in unsigned si apoi o scrie in buffer
                        sprintf(buffer3, "dimensiunea imaginii: %u octeti\n", dim_img);
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

                    if((stat(cale_completa, &var)) < 0)
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

                    writeAccessRights(var, fout);
                }

                else
                {
                    // fisier obisnuit
                    sprintf(buffer3, "nume fisier: %s\n", dirInp->d_name);
	                if(write(fout, buffer3, strlen(buffer3)) < 0)
	                {
		                perror("nu s-a putut efectua scrierea numelui fisierului!");
                        exit(5);
	                }

                    if((stat(cale_completa, &var)) < 0)
	                {
		                perror("eroare");
                        exit(7);
	                }

                    sprintf(buffer3, "dimensiunea fisierului: %lu octeti\n", var.st_size);
                    if(write(fout, buffer3, strlen(buffer3)) < 0)
	                {
		                perror("nu s-a putut efectua scrierea numelui fisierului!");
                        exit(5);
	                }

                    sprintf(buffer3, "identificatorul utilizatorului: %u\n", var.st_uid);  //ia din var id-ul userului
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

                    writeAccessRights(var, fout);
                    
                }
            }
            else
            {
                if(S_ISDIR(var.st_mode))
                {
                    DIR *intrare;

                    intrare = opendir(cale_completa);

                    if(intrare == NULL)
                    {
                        perror("eroare la deschiderea directorului!");
                        exit(4);
                    }

                    sprintf(buffer3, "nume director: %s\n", dirInp->d_name);
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

                    writeAccessRights(var, fout);
                    sprintf(buffer3, "\n");

                    if((closedir(intrare)) != 0)
                    {
                        perror("eroare la inchiderea directorului");   
                        exit(9);
                    }             
                }
            }
        }
    }
	
    close(fout);
    close(fin);
    
	return 0;
}
