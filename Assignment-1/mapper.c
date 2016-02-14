#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(){
	FILE *fp;    //object of file
	char str[1000], *ch;	
	char file_name[] = "/home/rishabh/ASP1/input.txt";
	fp = fopen(file_name,"r");
	if(fp == NULL){
		perror("File could not be opened\n");
      	exit(0);
  	 }

	if(fgets (str,1000,fp)==NULL){
		perror("Error in reading the file\n");
		exit(0);
	}      		 
       	//puts(str);
	ch = strtok (str," \n");
  	while (ch != NULL)
  	{
		printf ("(%s,1)\n",ch);		
		ch = strtok (NULL," \n");
  	}
	fclose(fp);
  	return 0;	
}
