#include<stdio.h>
#define file_name "user.txt"
struct user{
    int id;
    char name[100];
    int age;
};
int validid(int id){
    struct user u;
    FILE *file;
    file=fopen(file_name,"r");
    if(file==NULL)
    return 0;
    while(fscanf(file,"%d|%99[^|]|%d\n",&u.id,u.name,&u.age)==3){
        if(u.id==id){
            fclose(file);
            return 1;
        }
    }
    fclose(file);
    return 0;
}
void createuser(){
    FILE *file;
    struct user u;
    file=fopen(file_name,"a");
    if(file==NULL){
        printf("Cannot open file\n");
        return;
    }
    printf("Enter id: ");
    scanf("%d",&u.id);
    if(validid(u.id)){
        printf("Id already exists\n");
        fclose(file);
        return;
    }
    printf("Enter name: ");
    scanf("%99s",u.name);
    printf("Enter age: ");
    scanf("%d",&u.age);
    fprintf(file,"%d|%s|%d\n",u.id,u.name,u.age);
    fclose(file);
    printf("User created Successfully\n");
    return;
}
void readuser(){
    FILE *file;
    file=fopen(file_name,"r");
    struct user u;
    if(file==NULL){
        printf("No users found\n");
        return;
    }
    printf("user_id     user_name       user_age\n");
    int count=0;
    while(fscanf(file,"%d|%99[^|]|%d\n",&u.id,u.name,&u.age)==3){
        printf("%-10d %-30s %-10d\n",u.id,u.name,u.age);
        count++;
    }
    if(count==0)
    printf("No users found\n");
    fclose(file);
    return;
    }
void updateuser(){
    struct user u;
    FILE *file;
    FILE *temp;
    int searchid;
    int found=0;
    printf("Enter id to update:\n");
    scanf("%d",&searchid);
    file=fopen(file_name,"r");
    if(file==NULL){
        printf("No users found\n");
        return;
    }
    temp=fopen("temp.txt","w");
    if(temp==NULL){
        printf("Could not create temporary file");
        fclose(file);
        return;
    }
    while(fscanf(file,"%d|%99[^|]|%d\n",&u.id,u.name,&u.age)==3){
        if(searchid==u.id){
            found=1;
            printf("Enter new name:\n");
            scanf("%99s",u.name);
            printf("Enter new age:\n");
            scanf("%d",&u.age);
        }
        fprintf(temp,"%d|%s|%d\n",u.id,u.name,u.age);
    }
    fclose(file);
    fclose(temp);
    remove(file_name);
    rename("temp.txt",file_name);
    if(found){
        printf("User updated successfully\n");
    }
    else{
        printf("Cannot find user with this id\n");
    }
}
void deleteuser(){
    struct user u;
    FILE *file;
    FILE *temp;
    int searchid;
    int found=0;
    printf("Enter id to delete: ");
    scanf("%d",&searchid);
    file=fopen(file_name,"r");
    if(file==NULL){
        printf("Unable to open this file.\n");
        return;
    }
    temp=fopen("temp.txt","w");
    if(temp==NULL){
        printf("Unable to craete temporary file");
        fclose(file);
        return;
    }
    while(fscanf(file,"%d|%99[^|]|%d\n",&u.id,u.name,&u.age)==3){
        if(searchid==u.id){
            found=1;
            continue;
        }
            fprintf(temp,"%d|%s|%d\n",u.id,u.name,u.age);
    }
    fclose(file);
    fclose(temp);
    remove(file_name);
    rename("temp.txt",file_name);
    if(found){
        printf("User deleted successfully\n");
    }
    else{
        printf("User id not found\n");
    }
}
int main(){
    int choice;
    while(1){
    printf("--- user CRUD ---\n");
    printf("1.create user\n");
    printf("2.Read user\n");
    printf("3.Update user\n");
    printf("4.delete user\n");
    printf("5.exit\n");
    printf("Enter choice:\n");
    scanf("%d",&choice);
    if(choice==1)
    createuser();
    else if(choice==2)
    readuser();
    else if(choice==3)
    updateuser();
    else if(choice==4)
    deleteuser();
    else if(choice==5){
        printf("Program exited");
        break;
    }
    else
    printf("Invalid choice.Enter the correct choice");
    }
}
