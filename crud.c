#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>

#define FILE_NAME "user.txt"
#define TEMP_FILE "temp.txt"
#define MAX_SIZE 100

struct user {
    int id;
    char name[MAX_SIZE];
    int age;
};

int read_line(const char *prompt,char *buffer,int size) {
    printf("%s",prompt);

    if(fgets(buffer,size,stdin)==NULL){
        return 0;
    }

    size_t length=strlen(buffer);

    if(length>0 && buffer[length-1]=='\n'){
        buffer[length-1]='\0';
    } 
    else if(!feof(stdin)){
        int ch;
        while((ch=getchar())!='\n' && ch!=EOF){
            
        }
    }

    return 1;
}

int read_integer(const char *prompt, int *value){
    char input[MAX_SIZE];
    char *end;
    long number;

    while(1){
        if(!read_line(prompt,input,sizeof(input))){
            return 0;
        }

        number=strtol(input,&end,10);

        while(isspace((unsigned char)*end)){
            end++;
        }

        if(input==end || *end!='\0' ||
            number<-2147483647L-1 ||
            number>2147483647L){
            printf("Invalid input. Enter an integer.\n");
            continue;
        }

        *value=(int)number;
        return 1;
    }
}

int ensure_data_file_exists(){
    FILE *file=fopen(FILE_NAME,"a");

    if(file==NULL){
        printf("Unable to create or open the data file.\n");
        return 0;
    }

    fclose(file);
    return 1;
}

int user_id_exists(int search_id){
    struct user current_user;
    FILE *file=fopen(FILE_NAME,"r");

    if(file==NULL){
        return 0;
    }

    while(fscanf(file, "%d|%99[^|]|%d",
                  &current_user.id,
                  current_user.name,
                  &current_user.age)==3){
        if(current_user.id==search_id){
            fclose(file);
            return 1;
        }
    }

    fclose(file);
    return 0;
}

int read_user_name(char *name, int size){
    while(1){
        if(!read_line("Enter name: ",name,size)){
            return 0;
        }

        if(name[0]=='\0'){
            printf("Name cannot be empty.\n");
            continue;
        }

        if(strchr(name,'|')!=NULL){
            printf("Name cannot contain the '|' character.\n");
            continue;
        }

        return 1;
    }
}

int read_user_age(int *age){
    while(1){
        if(!read_integer("Enter age: ",age)){
            return 0;
        }

        if(*age<0){
            printf("Age cannot be negative.\n");
            continue;
        }

        return 1;
    }
}

void create_user(){
    struct user new_user;
    FILE *file;

    if(!ensure_data_file_exists()){
        return;
    }

    if(!read_integer("Enter id: ", &new_user.id)){
        return;
    }

    if(new_user.id<=0){
        printf("ID must be a positive integer.\n");
        return;
    }

    if(user_id_exists(new_user.id)){
        printf("ID already exists.\n");
        return;
    }

    if(!read_user_name(new_user.name,MAX_SIZE)){
        return;
    }

    if(!read_user_age(&new_user.age)){
        return;
    }

    file=fopen(FILE_NAME, "a");

    if(file==NULL){
        printf("Unable to open the data file.\n");
        return;
    }

    fprintf(file, "%d|%s|%d\n",
            new_user.id, new_user.name,new_user.age);

    fclose(file);
    printf("User created successfully.\n");
}

void read_users(){
    struct user current_user;
    FILE *file=fopen(FILE_NAME,"r");
    int count=0;

    if(file==NULL){
        printf("No users found.\n");
        return;
    }

    printf("\n%-10s %-30s %-10s\n",
           "User ID","User Name","User Age");

    while(fscanf(file, "%d|%99[^|]|%d",
                  &current_user.id,
                  current_user.name,
                  &current_user.age)==3){
        printf("%-10d %-30s %-10d\n",
               current_user.id,
               current_user.name,
               current_user.age);
        count++;
    }

    if(count==0){
        printf("No users found.\n");
    }

    fclose(file);
}

void update_user(){
    struct user current_user;
    FILE *file;
    FILE *temp;
    int search_id;
    int found = 0;

    if(!read_integer("Enter ID to update: ",&search_id)){
        return;
    }

    file=fopen(FILE_NAME,"r");

    if(file==NULL){
        printf("No users found.\n");
        return;
    }

    temp=fopen(TEMP_FILE,"w");

    if (temp==NULL){
        printf("Unable to create temporary file.\n");
        fclose(file);
        return;
    }

    while(fscanf(file,"%d|%99[^|]|%d",
                  &current_user.id,
                  current_user.name,
                  &current_user.age)==3){

        if(current_user.id==search_id){
            found=1;

            if(!read_user_name(current_user.name,MAX_SIZE) ||
                !read_user_age(&current_user.age)) {
                fclose(file);
                fclose(temp);
                remove(TEMP_FILE);
                return;
            }
        }

        fprintf(temp,"%d|%s|%d\n",
                current_user.id,
                current_user.name,
                current_user.age);
    }

    fclose(file);
    fclose(temp);

    if(!found){
        remove(TEMP_FILE);
        printf("User ID not found.\n");
        return;
    }

    if(remove(FILE_NAME)!= 0||
        rename(TEMP_FILE,FILE_NAME)!=0){
        printf("Unable to replace the data file.\n");
        return;
    }

    printf("User updated successfully.\n");
}

void delete_user(){
    struct user current_user;
    FILE *file;
    FILE *temp;
    int search_id;
    int found = 0;

    if(!read_integer("Enter ID to delete: ",&search_id)){
        return;
    }

    file=fopen(FILE_NAME,"r");

    if(file==NULL){
        printf("No users found.\n");
        return;
    }

    temp=fopen(TEMP_FILE,"w");

    if(temp==NULL){
        printf("Unable to create temporary file.\n");
        fclose(file);
        return;
    }

    while(fscanf(file,"%d|%99[^|]|%d",
                  &current_user.id,
                  current_user.name,
                  &current_user.age)==3){

        if(current_user.id==search_id){
            found=1;
            continue;
        }

        fprintf(temp,"%d|%s|%d\n",
                current_user.id,
                current_user.name,
                current_user.age);
    }

    fclose(file);
    fclose(temp);

    if(!found){
        remove(TEMP_FILE);
        printf("User ID not found.\n");
        return;
    }

    if(remove(FILE_NAME)!= 0 ||
        rename(TEMP_FILE, FILE_NAME)!=0) {
        printf("Unable to replace the data file.\n");
        return;
    }

    printf("User deleted successfully.\n");
}

int main(){
    int choice;

    while(1){
        printf("\n--- User CRUD ---\n");
        printf("1. Create user\n");
        printf("2. Read users\n");
        printf("3. Update user\n");
        printf("4. Delete user\n");
        printf("5. Exit\n");

        if(!read_integer("Enter choice: ",&choice)){
            printf("\nProgram exited.\n");
            break;
        }

        switch(choice){
            case 1:
                create_user();
                break;

            case 2:
                read_users();
                break;

            case 3:
                update_user();
                break;

            case 4:
                delete_user();
                break;

            case 5:
                printf("Program exited.\n");
                return 0;

            default:
                printf("Invalid choice. Enter a number from 1 to 5.\n");
        }
    }

    return 0;
}

