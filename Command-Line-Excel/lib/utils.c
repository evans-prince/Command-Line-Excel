#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Function to convert column index to name
char *col_index_to_name(int col){

    // Allocate memory for the name
    char *name=(char *)malloc(4*sizeof(char));

    // Convert the column index to name
    int i=0;
    while(col>=0){
        name[i++]='A'+((col)%26);
        col=(col/26)-1;
    }
    name[i]='\0';

    // Reverse the name
    int j=0;
    i--;
    while(j<i){
        char temp=name[i];
        name[i]=name[j];
        name[j]=temp;
        j++;i--;
    }
    
    return name;
}

// int main(){
//     int col=18277;
//     char *name=col_index_to_name(col);
//     printf("%s\n",name);
// }