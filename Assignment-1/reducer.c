#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct node{
	char x[20];
	int count;
	struct node *next;
};

int main(){
	struct node *head, *getnext, *dup_check, *array[26];
	int i,flag;
	for(i=0;i<26;i++) array[i]=NULL;
	char first, *line=NULL, *ch=NULL;	
    size_t read, len = 0;        
	read = getline(&line, &len, stdin);
	for(i=0;read!=-1;i++){		
		array[i] = malloc(sizeof(struct node));
		head=array[i];
    	head->next=0; 
		ch = strtok (line,",\n");
    	strcpy(head->x, ch+1);
		head->count++;
		//printf("%s has been added in the list\n",head->x);	
		first = *(ch+1);	
		while ((read = getline(&line,&len,stdin))!=-1&&*(line+1)==first){
			flag=0;
			ch=strtok(line,",\n"); 
			dup_check=head;
			while(dup_check != NULL){
    				if(strcmp(dup_check->x,ch+1)==0){
					dup_check->count++;
					flag=1;
					}
    				dup_check = dup_check->next;
			}
			if(flag==0){	       
				getnext=head;  		 
    				if (getnext!= 0){
    		    		while (getnext->next != 0){ 		    		
        	    		getnext=getnext->next;
    		    		}
    				}    		
    				getnext->next = malloc(sizeof(struct node));
    				getnext=getnext->next;
    				if(getnext==0){
        				printf("Out of memory");
        				return 0;
    				}  	  	
   				getnext->next=0;			
   				strcpy(getnext->x, ch+1);
				//printf("%s has been added in the list\n",getnext->x);	
			}	
		}
		getnext=head; 		
		while(getnext!=NULL){
    			printf("(%s,%d)\n", getnext->x,getnext->count);
    			getnext = getnext->next;
		}	
	}	
	return 0;
}
