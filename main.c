#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <math.h>


#define SIZEOF_SMOBJ 300
#define SMOBJ_NAME  "/myMemoryObj"

double gregory(int n);
double nilakanta(int n);
long get_N_posix(int fila,int size);

int main(void) {
    char comando[50];
    sprintf(comando, "rm /dev/shm/%s", SMOBJ_NAME); //para eliminar la memoria posix en caso de existir

  
  system(comando);
  int n; // numero solicitado en la tarea
  printf("Inserte un valor para N: ");
  scanf("%d",&n);
   int fd;
   fd = shm_open (SMOBJ_NAME, O_CREAT | O_RDWR  , 00700); /* create s.m object*/
   if(fd == -1)
   {
       printf("Error file descriptor \n");
	   exit(1);
   }
   if(-1 == ftruncate(fd, SIZEOF_SMOBJ))
   {
       printf("Error shared memory cannot be resized \n");
	   exit(1);
   }
   
   close(fd);

    char bufn[5];

    sprintf(bufn, "%d\n", n);
    puts(bufn);
    //char buf[] = "Esto fue escrito por el hijo 111\n";
    char *ptr;
    
    fd = shm_open (SMOBJ_NAME,  O_RDWR  , 00200); /* open s.m object*/
    if(fd == -1)
     {
         printf("Error file descriptor %s\n", strerror(errno));
  	   exit(1);
     }
     
     ptr = mmap(NULL, sizeof(bufn), PROT_WRITE, MAP_SHARED, fd, 0);
     if(ptr == MAP_FAILED)
     {
        printf("Map failed in write process: %s\n", strerror(errno));
        exit(1);
     }
     
     memcpy(ptr,bufn, sizeof(bufn));
     close(fd);

  // Creates two children
  pid_t pid1, pid2;
  pid1 = fork();
  pid2 = fork(); 

  //*************Aqui va el primer hijo************
  if (pid1 == 0) {
    double pi=gregory(get_N_posix(0,4));
    
    char buf[10];

    sprintf(buf, "%lf\n", pi);
    puts(buf);
    
    int fd;
    //char buf[] = "Esto fue escrito por el hijo 111\n";
    char *ptr;
    
    fd = shm_open (SMOBJ_NAME,  O_RDWR  , 00200); /* open s.m object*/
    if(fd == -1)
     {
         printf("Error file descriptor %s\n", strerror(errno));
  	   exit(1);
     }
     
     ptr = mmap(NULL, sizeof(buf), PROT_WRITE, MAP_SHARED, fd, 0);
     if(ptr == MAP_FAILED)
     {
        printf("Map failed in write process: %s\n", strerror(errno));
        exit(1);
     }
     
     memcpy(ptr+sizeof(buf),buf, sizeof(buf));
     close(fd);

    
  }


  //*************Aqui va el segundo hijo************
  else if (pid2 == 0) {
    double pi=nilakanta(get_N_posix(0,4));
    
    char buf[10];

    sprintf(buf, "%lf\n", pi);
    puts(buf);
    
    int fd;
    char *ptr;
    
    fd = shm_open (SMOBJ_NAME,  O_RDWR  , 00200); /* open s.m object*/
    if(fd == -1)
     {
         printf("Error file descriptor %s\n", strerror(errno));
  	   exit(1);
     }
     
     ptr = mmap(NULL, sizeof(buf), PROT_WRITE, MAP_SHARED, fd, 0);
     if(ptr == MAP_FAILED)
     {
        printf("Map failed in write process: %s\n", strerror(errno));
        exit(1);
     }
     
     memcpy(ptr+2*sizeof(buf),buf, sizeof(buf));
     close(fd);
  
    
  }
    
  else {
    /* Aca va el proceso padre */
    sleep(1);
    printf("Terminando en 3\n");
    sleep(1);
    printf("Terminando en 2\n");
    sleep(1);
    printf("Terminando en 1\n");

    


    printf("Proceso padre terminado\n");


  }
}

//*************Funciones auxiliares*****************


double gregory(int n) {
  double pi;
  int i = 0;
  for (i = 0; i <= n; ++i) {
    pi = pi + 4 * (pow((-1), i) / (2 * i + 1));
  }
  return pi;
}
double nilakanta(int n) {
  double pi = 3;
  int i = 0;
  for (i = 0; i <= n; ++i) {
    pi = pi + 4 * ((pow((-1), i)) / ((pow(2 * i + 3, 3) - (2 * i + 3))));
  }
  return pi;
}

long get_N_posix(int fila,int size)	//Esta funcion es para desplegar en pantalla los valores almacenados en la posix
		{
		int fd;
		char *ptr1;
		//char *ptr2;
		//char bufaa[size];
		//char bufbb[size];
		struct stat shmobj_st;	
		fd = shm_open(SMOBJ_NAME, O_RDWR,0);

		
		if(fd == -1){
			printf("Error fd %s\n", strerror(errno));
			exit(1);
			}
			
		if(fstat(fd, &shmobj_st)==1)
			{
			printf("error fstat \n");
			exit(1);
			}
		
		ptr1=mmap(NULL,shmobj_st.st_size, PROT_READ, MAP_SHARED,fd,0)+fila*size;
		
		if(ptr1==MAP_FAILED)
		{
			printf("Fallo el proceso de mapeo leyendo el proceso %s\n",strerror(errno));
			exit(1);
		}
		
		char *a=ptr1;
		int aa =  atoi(a);

		//printf("%d",aa);
		close(fd);
		
    return aa;
}
