#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>

void map_pool_updater (void *);
void mapper (void *);
void reducer (void *);
void word_count_writer(void *);

//buffer1: Mapper Pool, buffer2: Reducer Pool, buffer3: Summarizer Pool
char buffer1[10][1000], buffer2[10][1000],buffer3[10][1000];//Global buffers
sem_t m_empty,m_full,r_empty,r_full,w_empty,w_full;//Semaphores
pthread_mutex_t m_mutex = PTHREAD_MUTEX_INITIALIZER;//Mutexes
pthread_mutex_t r_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t w_mutex = PTHREAD_MUTEX_INITIALIZER;
int in=0, out=0, file_complete=0, writer_flag=0,rin=0, rout=0,win=0, wout=0, x, y;
pthread_t *aptr, *bptr;// Pointers to mapper and reducer thread arrays

struct node{
	char x[20];
	int count;
	struct node *next;
};

int main(int argc, char *argv[]){	
	
	char *file_name, file[50]="./";
 	int i,j; 	
 	
	file_name = (argv[1]);
	strcat(file, file_name);
	x = atoi(argv[2]);
 	y = atoi(argv[3]);
 	
 	pthread_t thread1,thread2[x],thread3[y],thread4;
 	
 	aptr = thread2;
	bptr = thread3;

	sem_init(&m_empty,0,1);
	sem_init(&m_full,0,0);
	sem_init(&r_empty,0,1);
	sem_init(&r_full,0,0);
	sem_init(&w_empty,0,1);
	sem_init(&w_full,0,0);
	
	pthread_create (&thread1, NULL, (void *)map_pool_updater, file);
	
	for(i=0; i<x; i++){
		pthread_create (&thread2[i], NULL, (void *)mapper, (void *)thread2[i]);
	}
	
	for(j=0; j<y; j++){
		pthread_create (&thread3[j], NULL, (void *)reducer, (void *)thread3[j]);
	}
	
	pthread_create (&thread4, NULL, (void *)word_count_writer, "");	

	pthread_join(thread1, NULL);	
	pthread_join(thread4, NULL);	

	sem_destroy(&m_empty);
	sem_destroy(&m_full);	
	sem_destroy(&r_empty);
	sem_destroy(&r_full);
	sem_destroy(&w_empty);
	sem_destroy(&w_full);

  	return 0;	
}

void map_pool_updater (void *file) {
	
	FILE *fp;    //object of file
	char str[1000], temp[1000], ch;
	int i;
	memset(temp, 0, sizeof(temp));
	fp = fopen(file,"r");

	if(fp == NULL){
		perror("File could not be opened\n");
      		exit(0);
  	}

	fscanf (fp, "%s", str);
	ch = *(str);
	strcpy(temp,str);

	while (fscanf (fp, "%s", str)!=EOF){				
		if (*(str)!=ch){
			sem_wait(&m_empty);
			pthread_mutex_lock(&m_mutex);						
			strcpy(buffer1[in],temp);
			in = (in+1)%10;
			pthread_mutex_unlock(&m_mutex);
			sem_post(&m_full);
			strcpy(temp,"");
			ch=*(str);			
		}
		else{
			strcat(temp," ");
		}
		strcat(temp,str);			
	}
	
	sem_wait(&m_empty);
	pthread_mutex_lock(&m_mutex);					
	strcpy(buffer1[in],temp);
	in = (in+1)%10;
	pthread_mutex_unlock(&m_mutex);
	sem_post(&m_full);
		
	sem_wait(&m_empty);
	pthread_mutex_lock(&m_mutex);						
	strcpy(buffer1[in],"End"); //Adding a word 'End' to signal the subsequent readers, the end of file, and thus exit.
	in = (in+1)%10;
	pthread_mutex_unlock(&m_mutex);
	sem_post(&m_full);
		
	fclose(fp);
	//printf("Updater Exiting\n");
	pthread_exit(0);
}

void mapper (void *ptr){
	char *tok, *temp;
	int flag=0,i;
	while(1){ 
		sem_wait(&m_full);
		pthread_mutex_lock(&m_mutex);
		temp=buffer1[out];
		
		//The word End signals the mapper that no more inputs are incoming and it should exit.	
		if(strcmp(temp,"End")==0){			
			flag=1;
		}
		
		out=(out+1)%10;
		pthread_mutex_unlock(&m_mutex);
		sem_post(&m_empty);
		
		sem_wait(&r_empty);
		pthread_mutex_lock(&r_mutex);	
		strcpy(buffer2[rin],"");
		tok = strtok (temp," \n");					
  		while (tok != NULL)
  		{			
			strcat(buffer2[rin],"(");
			strcat(buffer2[rin],tok);
			strcat(buffer2[rin],",1)");
			tok = strtok (NULL," \n");
  		}	
		rin=(rin+1)%10;
		pthread_mutex_unlock(&r_mutex);	
		sem_post(&r_full);		
		
		//If end was encountered the thread cancels similar threads and then cancels itself
		if(flag==1){
			int s;
			pthread_t self;
			self = pthread_self();
			for (i=0;i<x;i++){
				sleep(0.25);
				if(aptr[i]==self){
					//No Action
				}
				else {
					//printf("Killing Mapper thread: %lu\n",aptr[i]);
					pthread_cancel(aptr[i]);
				}
			}
			//printf("Killing Mapper thread: %lu\n",(unsigned long)self);
			s=pthread_cancel(self);			
		}
	}
	
	pthread_exit(0);
}

void reducer (void *ptr){
	//The reducer thread uses linked lists to store and count the tuples.
	
	struct node *head, *getnext, *dup_check;
	int flag=0,t=0,i=0,k;
	char first, line[100],str[10];
	char temp[1000], *tok;
	while(1){
		sem_wait(&r_full);
		pthread_mutex_lock(&r_mutex);
		strcpy(temp,buffer2[rout]);
		
		//The word End signals the mapper that no more inputs are incoming and it should exit.	
		if(strcmp(temp,"(End,1)")==0){			
			flag=1;
		}

		rout=(rout+1)%10;
	
		head = (struct node*)malloc(sizeof(struct node));	
		tok = strtok(temp,"(,1)");		
		if(tok!=NULL){
			head->next=NULL; 
			strcpy(head->x,tok);
			head->count++;
		}
		tok = strtok(NULL,"(,1)");
		while(tok!=NULL){
			flag=0;
			dup_check=head;
			while(dup_check != NULL){
    				if(strcmp(dup_check->x,tok)==0){
					dup_check->count++;
					flag=1;
				}
    				dup_check = dup_check->next;
			}
			if(flag==0){	       
				getnext=head;  		 
				while (getnext->next != NULL){ 		    		
        				getnext=getnext->next;
    				}
				getnext->next = (struct node*) malloc(sizeof(struct node));
    				getnext=getnext->next; 	   					
   				strcpy(getnext->x,tok);
				getnext->count++;
				getnext->next=NULL;	
			}
			tok = strtok(NULL,"(,1)");			
		}	
		
		pthread_mutex_unlock(&r_mutex);
		sem_post(&r_empty);
		
		sem_wait(&w_empty);
		pthread_mutex_lock(&w_mutex);	
		getnext=head;  
		strcpy(buffer3[win],"");		 
		while (getnext != NULL){ 
			strcat(buffer3[win], "(");
			strcat(buffer3[win], getnext->x);
			strcat(buffer3[win], ",");
			sprintf(str, "%d", getnext->count);
			strcat(buffer3[win], str);
			strcat(buffer3[win], ") ");	
        	getnext=getnext->next;
    	}

		win=(win+1)%10;
		strcpy(buffer3[win], "End");		
    	pthread_mutex_unlock(&w_mutex);	
		sem_post(&w_full);	
		
		//If end was encountered the thread cancels similar threads and then cancels itself
    	if(flag==1){	
			sleep(2);		
			int s;
			pthread_t self;
			self = pthread_self();			
			for (k=0;k<y;k++){
				if(bptr[k]==self){
					//No Action
				}
				else {								
					//printf("Killing reducer thread: %lu\n",bptr[k]);
					pthread_cancel(bptr[k]);
				}
			}
			//printf("Killing reducer thread: %lu\n",(unsigned long)self);
			s=pthread_cancel(self);			
		}
	}
	pthread_exit(0);	

}

void word_count_writer(void *ptr){
	FILE *file;
	char *tok;
	char temp[500];

	while(1){
		sem_wait(&w_full);
		pthread_mutex_lock(&w_mutex);
		file = fopen("./wordCount.txt","a+");	
		if(file == NULL){
			perror("File could not be created\n");
			exit(0);
		}
		strcpy(temp, buffer3[wout]);
		//If end was encountered the thread ends.
		if(strcmp(temp,"(End,1) ")==0){		
			sleep(2);
			break;
		}

		tok = strtok(temp," ");		
		while(tok!=NULL){
			fprintf(file,"%s\n", tok);
			tok = strtok(NULL," ");
		}	

		wout=(wout+1)%10;
		fclose(file);
		pthread_mutex_unlock(&w_mutex);
		sem_post(&w_empty);
	
	}
	//printf("Killing writer thread\n");
	pthread_exit(0);
}
		


















	

