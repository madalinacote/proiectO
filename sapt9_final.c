//proiect -- document sapt 9  -- pipes
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
#include <sys/wait.h>

#define BUFF_SIZE 512
#define SIZE 512

void scrieDrepturiAcces(struct stat var, int fout)
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

void procesarePixeli(char* cale_completa, int fout)
{
    int pid = fork(); //creeaza proces pentru prelucrarea pixelilor

    if(pid < 0)
    {
        perror("eroare la fork-ul pentru procesarea pixelilor!\n");
        exit(-1);
    }

    __uint32_t latime_imagine;
    __uint32_t inaltime_imagine;

    if(pid == 0)
    {
        int imagine = open(cale_completa, O_RDWR);

        if(imagine == -1)
        {
            perror("eroare la deschiderea imaginii!\n");
            exit(-1);
        }

        if(read(imagine, &latime_imagine, sizeof(latime_imagine)) == -1)  // Citeste urmatorii 4 octeti
        {   
            perror("nu s-a putut efectua citirea latimii imaginii!\n");
            exit(6);
        }

        if(read(imagine, &inaltime_imagine, sizeof(inaltime_imagine)) == -1)  // Citeste urmatorii 4 octeti
        {   
            perror("nu s-a putut efectua citirea inaltimii imaginii!\n");
            exit(6);
        }

        int rezolutie = latime_imagine*inaltime_imagine;
        unsigned char P_culoare[3];
        unsigned char P_gri;

        for(int i = 0; i < rezolutie; i++)
        {
            if(read(imagine, P_culoare, sizeof(P_culoare)) == -1)
            {
                perror("eroare la citirea pixelilor!\n");
                exit(5);
            }

            P_gri = 0.299*P_culoare[0] + 0.587*P_culoare[1] + 0.114*P_culoare[2];  //proceseaza pixelii conform formulei

            lseek(imagine, -3, SEEK_CUR);  //se intoarce inapoi 3 octeti

            write(imagine, &P_gri, sizeof(P_gri));   //suprascrie fiecare pixel
            write(imagine, &P_gri, sizeof(P_gri));  //se face de 3 ori pt ca fiecare pixel are 3 componente de culoare (rgb)
            write(imagine, &P_gri, sizeof(P_gri));
        }

        close(imagine);
    }
    else
    {
        int status;
        waitpid(pid, &status, 0);
        printf("S-a incheiat procesul cu pid %d si status %d\n", pid, status); 
    }
}

void verificarePropozitii(char* continut, char caracter, int PF)
{
    int PF_script[2];
    if(pipe(PF_script) == -1)
    {
        perror("eroare la crearea pipe-ului pentru script!\n");
        exit(10);
    }

    int pid_script = fork();

    if(pid_script == -1)
    {
        perror("eroare la fork!\n");
        exit(10);
    }

    if(pid_script == 0)
    {
        
        dup2(PF_script[0], STDIN_FILENO);
        close(PF_script[0]);  //se inchide capatul de scriere al pipe-ului pentru script

        execlp("/home/mada/anul3/proiectSO/sapt9.sh", "sapt9.sh", NULL);

        close(PF_script[1]); // se inchide capatul de citire al pipe-ului pentru script

        perror("eroare la executarea scriptului!\n");
        exit(10);
    }
    else
    {
        close(PF_script[0]);
        write(PF_script[1], continut, strlen(continut));
        close(PF_script[1]);

        int status;
        waitpid(pid_script, &status, 0);

        if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
        {
            char bufferRezultat[BUFF_SIZE];
            int nrBytesCititi = read(PF_script[0], bufferRezultat, sizeof(bufferRezultat));

            if (nrBytesCititi == -1)
            {
                perror("eroare la citirea rezultatului scriptului!\n");
                exit(10);
            }

            write(PF, bufferRezultat, nrBytesCititi);
        }
        else
        {
            perror("eroare la executarea scriptului!\n");
            exit(11);
        }
    }
}

int main(int argc, char* argv[])
{
	int fin, fout, fout1;
    struct stat var;
    
    char buffer2[BUFF_SIZE];
    char buffer3[BUFF_SIZE];

    char eroare[50] = "Usage: ";  //se formeaza mesajul de eroare conform cerintei
	strcat(eroare, argv[0]);

    if(argc != 4)  //verifica numarul de argumente
    {
        perror("numar gresit de argumente!\n");
        exit(1);
    }

	strcat(eroare, " ");  //se continua mesajul de eroare conform cerintei
    strcat(eroare, argv[1]);

    if((stat(argv[1], &var)) < 0) //verifica daca fisierul exista
    {
        perror("fisierul nu a fost gasit!\n");
        exit(2);
    }

    if(!S_ISDIR(var.st_mode))
    {
        perror(eroare);
        exit(3);
    }

    DIR* dirI;
    DIR* dirO;
    struct dirent *dirInp;

    dirI = opendir(argv[1]);
    dirO = opendir(argv[2]);

    if(dirI == NULL)
    {
        perror("eroare la deschiderea directorului de intrare!\n");
        exit(4);
    }

    if(dirO == NULL)
    {
        perror("eroare la deschiderea directorului de iesire!\n");
        exit(4);
    }

    if((fout1 = open("statistica.txt", O_WRONLY | O_TRUNC | O_CREAT , S_IRUSR | S_IWUSR)) < 0)  //deschide fisierul de iesire
	{
		perror("nu s-a putut deschide fisierul de iesire!");
        exit(4);
	}

    int numar_linii_total = 0;

    while ((dirInp = readdir(dirI)) != NULL) 
    {
        char cale_completa[SIZE] = "";
        strcpy(cale_completa, argv[1]);
        strcat(cale_completa, "/");
        strcat(cale_completa, dirInp->d_name);

        fin = open(cale_completa, O_RDONLY);

        if (lstat(cale_completa, &var) < 0) 
        {
            perror("eroare la obtinerea informatiilor despre directorul de intrare!\n");
            exit(4);
        }
        
        int numar_linii = 0;
        char continut_citit[BUFF_SIZE];

        if (S_ISLNK(var.st_mode))
        {
            //legatura simbolica
            int pidLink = fork();
            if(pidLink == -1)
            {
                perror("eroare la fork!\n");                   
                exit(-1);
            }

            if(pidLink == 0)
            {
                char numeFisierIesire[SIZE];
                sprintf(numeFisierIesire, "%s%s_statistica.txt", argv[2], dirInp->d_name);
                if((fout = open(numeFisierIesire, O_WRONLY | O_TRUNC | O_CREAT , S_IRUSR | S_IWUSR)) < 0)  //deschide fisierul de iesire
	            {
		            perror("nu s-a putut deschide fisierul de iesire!\n");
                    exit(4);
                }

                struct stat target;
                if (lstat(cale_completa, &target) < 0)
                {
                    perror("eroare la accesarea informatiilor despre legatura simbolica!\n");
                    exit(4);
                }
            
                sprintf(buffer3, "numele legaturii simbolice: %s\n", dirInp->d_name);
                if (write(fout, buffer3, strlen(buffer3)) < 0)
                {
                    perror("nu s-a putut efectua scrierea numelui legaturii!\n");
                    exit(5);
                }

                sprintf(buffer3, "dimensiunea legaturii simbolice: %lu\n", var.st_size);
                if(write(fout, buffer3, strlen(buffer3)) < 0)
	            {
		            perror("nu s-a putut efectua scrierea dimensiunii legaturii simbolice!\n");
                    exit(5);
	            }

                sprintf(buffer3, "dimensiunea fisierului target: %ld\n", target.st_size);
                if(write(fout, buffer3, strlen(buffer3)) < 0)
	            {
		            perror("nu s-a putut efectua scrierea dimensiunii fisierului!\n");
                    exit(5);
	            }
            
                scrieDrepturiAcces(target, fout);
                numar_linii = numar_linii + 3;
            }
            else
            {
                int status;
                waitpid(pidLink, &status, 0);
                printf("S-a incheiat procesul cu pid %d si status %d\n", pidLink, status); 
            }
        }

        else
        {
            if(S_ISREG(var.st_mode))
            {
                //fisier .bmp
                if(strstr(cale_completa, ".bmp") != NULL)
                {
                    int pidBMP = fork();

                    if(pidBMP == -1)
                    {
                        perror("eroare la fork!\n");
                        exit(-1);
                    }

                    if(pidBMP == 0)
                    {
                        char numeFisierIesire[SIZE];
                        sprintf(numeFisierIesire, "%s%s_statistica.txt", argv[2], dirInp->d_name);
                        if((fout = open(numeFisierIesire, O_WRONLY | O_TRUNC | O_CREAT , S_IRUSR | S_IWUSR)) < 0)  //deschide fisierul de iesire
	                    {
		                    perror("nu s-a putut deschide fisierul de iesire!\n");
                            exit(4);
                        }
                        
                        sprintf(buffer3, "nume fisier: %s\n", dirInp->d_name);
	                    if(write(fout, buffer3, strlen(buffer3)) < 0)
	                    {
		                    perror("nu s-a putut efectua scrierea numelui fisierului .bmp!\n");
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
		                        perror("nu s-a putut efectua scrierea!\n");
                                exit(5);
	                        }
                        }
                        else
                        {
                            perror("nu s-a putut efectua citirea!\n");
                            exit(6);
                        }

                        lseek(fin, 12, SEEK_CUR);  // Sare la octetul 18 din header
                        __uint32_t latime, inaltime;
                        if(read(fin, &latime, sizeof(latime)) != -1)  // Citeste urmatorii 4 octeti
                        {
                            //extrage latimea imaginii din header, o converteste din char in unsigned si apoi o scrie in buffer
                            sprintf(buffer3, "latime: %u\n",  latime);
                            if(write(fout, buffer3, strlen(buffer3)) < 0)
                            {
                                perror("nu s-a putut efectua scrierea!\n");
                                exit(5);
                            }
                        }
                        else
                        {
                            perror("nu s-a putut efectua citirea!\n");
                            exit(6);
                        }
                    
                        if(read(fin, &inaltime, sizeof(inaltime)) != -1)  // citeste urmatorii 4 octeti
                        {
                            //extrage inaltimea imaginii din header, o converteste din char in unsigned si apoi o scrie in buffer
                            sprintf(buffer3, "inaltime: %u\n",  inaltime);
                            if(write(fout, buffer3, strlen(buffer3)) < 0)
	                        {
		                        perror("nu s-a putut efectua scrierea!\n");
                                exit(5);
	                        }
                        }
                        else
                        {
                            perror("nu s-a putut efectua citirea!\n");
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
		                        perror("nu s-a putut efectua scrierea!\n");
                                exit(5);
	                        }
                        }
                        else
                        {
                            perror("nu s-a putut efectua citirea!\n");
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
	                        perror("nu s-a putut efectua scrierea user id-ului fisierului .bmp!\n");
                            exit(5);
	                    }

	                    time_t ultima_modificare = var.st_mtime;   //ia din var data ultimei modificari

                        sprintf(buffer3, "data ultimei modificari: %s", ctime(&ultima_modificare));

                        if(write(fout, buffer3, strlen(buffer3)) < 0)
                        {
	                        perror("nu s-a putut efectua scrierea datei ultimei modificari a fisierului .bmp!\n");
                            exit(5);
	                    }

                        sprintf(buffer3, "contorul de legaturi: %ld\n", var.st_nlink); //ia din var contorul de legaturi

	                    if(write(fout, buffer3, strlen(buffer3)) < 0)
                        {
	                        perror("nu s-a putut efectua scrierea contorului de legaturi al fisierului .bmp!\n");
                            exit(5);
	                    }

                        scrieDrepturiAcces(var, fout);
                        if (strstr(cale_completa, ".bmp") != NULL) 
                        {
                            procesarePixeli(cale_completa, fout);
                        }
                        numar_linii = numar_linii + 6;
                    }
                    else
                    {
                        int status;
                        waitpid(pidBMP, &status, 0);
                        printf("S-a incheiat procesul cu pid %d si status %d\n", pidBMP, status); 
                    }
                }
                else
                {
                 // fisier obisnuit
                    int pidFile = fork();

                    if(pidFile == -1)
                    {
                        perror("eroare la fork!\n");
                        exit(-1);
                    }

                    if(pidFile == 0)
                    {
                        char numeFisierIesire[SIZE];
                        sprintf(numeFisierIesire, "%s%s_statistica.txt", argv[2], dirInp->d_name);
                        if((fout = open(numeFisierIesire, O_WRONLY | O_TRUNC | O_CREAT , S_IRUSR | S_IWUSR)) < 0)  //deschide fisierul de iesire
	                    {
		                    perror("nu s-a putut deschide fisierul de iesire!\n");
                            exit(4);
                        }
                            
                        sprintf(buffer3, "nume fisier: %s\n", dirInp->d_name);
	                    if(write(fout, buffer3, strlen(buffer3)) < 0)
	                    {
		                    perror("nu s-a putut efectua scrierea numelui fisierului!\n");
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
		                    perror("nu s-a putut efectua scrierea numelui fisierului!\n");
                            exit(5);
	                    }

                        sprintf(buffer3, "identificatorul utilizatorului: %u\n", var.st_uid);  //ia din var id-ul userului
                        if(write(fout, buffer3, strlen(buffer3)) < 0)
	                    {
	                        perror("nu s-a putut efectua scrierea user id-ului fisierului!\n");
                            exit(5);
	                    }

                        time_t ultima_modificare = var.st_mtime;   //ia din var data ultimei modificari

                        sprintf(buffer3, "data ultimei modificari: %s", ctime(&ultima_modificare));
                        if(write(fout, buffer3, strlen(buffer3)) < 0)
                        {
	                        perror("nu s-a putut efectua scrierea datei ultimei modificari a fisierului!\n");
                            exit(5);
	                    }

                        sprintf(buffer3, "contorul de legaturi: %ld\n", var.st_nlink); //ia din var contorul de legaturi
	                    if(write(fout, buffer3, strlen(buffer3)) < 0)
                        {
	                        perror("nu s-a putut efectua scrierea contorului de legaturi al fisierului!\n");
                            exit(5);
	                    }

                        scrieDrepturiAcces(var, fout); 
                        numar_linii = numar_linii + 5;
                        int PF[2];  //se creeaza un pipe de la parinte la fiu
                        int FF[2];  //se creeaza un pipe intre cei doi fii
                        if (pipe(PF) == -1)
                        {
                            perror("eroare la crearea pipe-ului de la parinte la fiu!\n");
                            exit(9);
                        }

                        if(pipe(FF) == -1)
                        {
                            perror("eroare la crearea pipe-ului de la primul la al doilea fiu!\n");
                            exit(9);
                        }

                        int verficare_propozitii = fork();  //se creeaza un nou proces pentru fisierele care nu sunt bmp

                        if (verficare_propozitii == -1)
                        {
                            perror("eroare la fork pentru generarea continutului");
                            exit(9);
                        }

                        if (verficare_propozitii == 0)
                        {
                            close(FF[0]); // inchide capatul de citire al pipe-ului dintre fii
                            close(PF[0]);

                            verificarePropozitii(continut_citit, *argv[3], FF[1]);

                            close(FF[1]);  //inchidem capatul de scriere al pipe-ului
                            close(PF[1]);
                            exit(11);
                        }
                        else
                        {
                            close(PF[1]);  //inchide capatul de scriere al pipe-ului
                            waitpid(verficare_propozitii, NULL, 0);
                            close(FF[1]);

                            char rezultat[BUFF_SIZE];
                            int bytesRezultat = read(FF[0], rezultat, sizeof(rezultat));

                            if(bytesRezultat == -1)
                            {
                                perror("eroare la rezultatul din pipe!\n");
                                exit(11);

                                close(FF[0]);  //inchide capatul de citire al pipe-ului
                                close(PF[0]);
                            }

                            int numar_prop = atoi(rezultat);
                            printf("au fost identificate in total %d propozitii corecte care contin caracterul %c\n", numar_prop, *argv[3]);        
                            numar_linii_total = numar_linii_total + numar_prop;
                        }
                    }
                    else
                    {
                        int status;
                        waitpid(pidFile, &status, 0);
                        printf("S-a incheiat procesul cu pid %d si status %d\n", pidFile, status); 
                    }
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
                        perror("eroare la deschiderea directorului!\n");
                        exit(4);
                    }

                    int pidDir = fork();

                    if(pidDir == -1)
                    {
                        perror("eroare la fork!\n");
                        exit(-1);
                    }

                    if(pidDir == 0)
                    {
                        char numeFisierIesire[SIZE];
                        sprintf(numeFisierIesire, "%s%s_statistica.txt", argv[2], dirInp->d_name);
                        if((fout = open(numeFisierIesire, O_WRONLY | O_TRUNC | O_CREAT , S_IRUSR | S_IWUSR)) < 0)  //deschide fisierul de iesire
	                    {
		                    perror("nu s-a putut deschide fisierul de iesire!\n");
                            exit(4);
                        }

                        sprintf(buffer3, "nume director: %s\n", dirInp->d_name);
                        if(write(fout, buffer3, strlen(buffer3)) < 0)
                        {
                            perror("nu s-a putut efectua scrierea numelui directorului!\n");
                            exit(5);
                        }

                        sprintf(buffer3, "identificatorul utilizatorului: %u\n", var.st_uid);
                        if(write(fout, buffer3, strlen(buffer3)) < 0)
                        {
                            perror("nu s-a putut efectua scrierea user id-ului pentru director!\n");
                            exit(5);
                        }

                        scrieDrepturiAcces(var, fout);
                        sprintf(buffer3, "\n");
                        numar_linii = numar_linii + 3;

                        if((closedir(intrare)) != 0)
                        {
                            perror("eroare la inchiderea directorului!\n");   
                            exit(9);
                        }
                        close(fout); 
                    }
                    else
                    {
                        int status;
                        waitpid(pidDir, &status, 0);
                        printf("S-a incheiat procesul cu pid %d si status %d\n", pidDir, status); 
                    }
                    
                }
                numar_linii_total = numar_linii_total + numar_linii;
            }
            dprintf(fout1, "numarul total de linii: %d\n", numar_linii_total);
        }
    }
    close(fout);
    close(fin);
    closedir(dirI);
    closedir(dirO);
    
	return 0;
}
