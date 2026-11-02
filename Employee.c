#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <ctype.h>

#define NAME_SIZE 100
#define DESIGNATION_SIZE 100
#define INPUT_SIZE 256

typedef struct Employee Employee;
typedef struct ReportLink ReportLink;

struct ReportLink{
    Employee *employee;
    ReportLink *next;
};

struct Employee{
    int id;
    char name[NAME_SIZE];
    char designation[DESIGNATION_SIZE];

    Employee *manager;
    ReportLink *reports;

    Employee *next; 
};

static Employee *employees=NULL;

static int read_line(const char *prompt, char *buffer, size_t size)
{
    size_t length;
    int ch;

    printf("%s",prompt);

    if(fgets(buffer,(int)size,stdin)==NULL){
        return 0;
    }

    length=strlen(buffer);

    if(length>0 && buffer[length - 1]=='\n'){
        buffer[length-1]='\0';
    } 
    else {
        ch==getchar();

        if(ch!='\n' && ch!=EOF){
            while((ch=getchar())!='\n' && ch!=EOF){
            }

            printf("Input is too long. Please try again.\n");
            return -1;
        }
    }

    length=strlen(buffer);

    if(length>0 && buffer[length-1]=='\r'){
        buffer[length-1]='\0';
    }

    return 1;
}

static int read_integer(const char *prompt, int *value){
    char buffer[INPUT_SIZE];
    char *end;
    long number;
    int status;
    size_t i;

    while(1){
        status=read_line(prompt,buffer,sizeof(buffer));

        if(status==0){
            return 0;
        }

        if(status==-1){
            continue;
        }

        i=0;

        while(isspace((unsigned char)buffer[i])){
            i++;
        }

        if(buffer[i]=='\0'){
            printf("Input cannot be empty.\n");
            continue;
        }

        errno=0;
        number=strtol(buffer+i,&end,10);

        if(buffer+i==end || errno==ERANGE ||
            number<INT_MIN || number>INT_MAX){
            printf("Invalid integer. Please try again.\n");
            continue;
        }

        while(isspace((unsigned char)*end)){
            end++;
        }

        if(*end!='\0'){
            printf("Enter a valid integer without extra characters.\n");
            continue;
        }

        *value=(int)number;
        return 1;
    }
}

static int read_nonempty_text(const char *prompt,char *buffer,size_t size){
    int status;
    size_t i;

    while(1){
        status=read_line(prompt, buffer, size);

        if(status==0){
            return 0;
        }

        if(status==-1){
            continue;
        }

        i=0;

        while(isspace((unsigned char)buffer[i])){
            i++;
        }

        if(buffer[i]=='\0'){
            printf("This field cannot be empty.\n");
            continue;
        }

        if(i>0){
            memmove(buffer,buffer+i,strlen(buffer+i)+1);
        }

        i=strlen(buffer);

        while(i>0 && isspace((unsigned char)buffer[i-1])){
            buffer[--i]='\0';
        }

        return 1;
    }
}

static Employee *find_employee(int id){
    Employee *current=employees;

    while(current!=NULL){
        if(current->id==id){
            return current;
        }

        current=current->next;
    }

    return NULL;
}

static ReportLink *create_report_link(Employee *employee){
    ReportLink *link=malloc(sizeof(*link));

    if(link==NULL){
        return NULL;
    }

    link->employee=employee;
    link->next=NULL;

    return link;
}

static int add_report_link(Employee *manager,Employee *employee){
    ReportLink *link;
    ReportLink **current;

    link=create_report_link(employee);

    if(link==NULL){
        return 0;
    }

    current=&manager->reports;

    while(*current!=NULL){
        current=&(*current)->next;
    }

    *current=link;
    return 1;
}

static void remove_report_link(Employee *manager,Employee *employee){
    ReportLink **current;
    ReportLink *temporary;

    if(manager==NULL){
        return;
    }

    current=&manager->reports;

    while(*current!=NULL){
        if((*current)->employee==employee){
            temporary=*current;
            *current=temporary->next;
            free(temporary);
            return;
        }

        current=&(*current)->next;
    }
}

static void add_employee(void){
    Employee *employee;
    Employee *manager=NULL;
    int id;
    int manager_id;

    employee=malloc(sizeof(*employee));

    if(employee==NULL){
        printf("Error: Unable to allocate memory.\n");
        return;
    }

    employee->manager=NULL;
    employee->reports=NULL;
    employee->next=NULL;

    if(!read_integer("Enter Employee ID: ",&id)){
        free(employee);
        return;
    }

    if(id <= 0){
        printf("Employee ID must be positive.\n");
        free(employee);
        return;
    }

    if(find_employee(id) != NULL){
        printf("Error: Employee ID already exists.\n");
        free(employee);
        return;
    }

    if(!read_nonempty_text(
            "Enter Employee Name: ",
            employee->name,sizeof(employee->name))){
        free(employee);
        return;
    }

    if(!read_nonempty_text(
            "Enter Designation: ",
            employee->designation,sizeof(employee->designation))){
        free(employee);
        return;
    }

    printf("Enter 0 if this employee has no manager.\n");

    if(!read_integer("Enter Reporting Manager ID: ",&manager_id)){
        free(employee);
        return;
    }

    if(manager_id < 0){
        printf("Manager ID cannot be negative.\n");
        free(employee);
        return;
    }

    if(manager_id!=0){
        manager=find_employee(manager_id);

        if(manager==NULL){
            printf("Error: Reporting manager does not exist.\n");
            free(employee);
            return;
        }

        if(manager->id==id){
            printf("Error: An employee cannot manage themselves.\n");
            free(employee);
            return;
        }
    }

    employee->id=id;
    employee->manager=manager;

    if(manager!=NULL &&
        !add_report_link(manager, employee)){
        printf("Error: Unable to allocate reporting link.\n");
        free(employee);
        return;
    }

    employee->next=employees;
    employees=employee;

    printf("Employee added successfully.\n");
}

static void remove_employee(){
    int id;
    Employee *employee;
    Employee *manager;
    Employee **current;
    ReportLink *link;
    ReportLink *next_link;

    if(!read_integer("Enter Employee ID to remove: ", &id)){
        return;
    }

    employee=find_employee(id);

    if(employee==NULL){
        printf("Error: Employee not found.\n");
        return;
    }

    manager=employee->manager;

    remove_report_link(manager,employee);

    link=employee->reports;

    while(link!=NULL){
        next_link=link->next;

        link->employee->manager=manager;

        if(manager!=NULL){
            ReportLink **tail=&manager->reports;

            while(*tail!=NULL){
                tail=&(*tail)->next;
            }

            *tail=link;
            link->next=NULL;
        } 
        else{
            free(link);
        }

        link=next_link;
    }

    employee->reports=NULL;

    current=&employees;

    while(*current!=NULL){
        if(*current==employee){
            *current=employee->next;
            break;
        }

        current=&(*current)->next;
    }

    free(employee);

    printf("Employee removed successfully.\n");
}

static int count_under(Employee *employee){
    ReportLink *link;
    int total=0;

    if(employee==NULL){
        return 0;
    }

    link=employee->reports;

    while(link!=NULL){
        total+= 1+count_under(link->employee);
        link=link->next;
    }

    return total;
}

static void display_subtree(Employee *employee,int depth){
    ReportLink *link;
    int i;

    if(employee==NULL){
        return;
    }

    for(i=0;i<depth;i++){
        printf("    ");
    }

    printf("%d - %s - %s\n",
           employee->id,
           employee->name,
           employee->designation);

    link=employee->reports;

    while(link != NULL){
        display_subtree(link->employee,depth+1);
        link=link->next;
    }
}

static void display_employee_hierarchy(){
    int id;
    Employee *employee;
    int total;

    if(!read_integer("Enter Employee ID: ", &id)){
        return;
    }

    employee=find_employee(id);

    if(employee==NULL){
        printf("Error: Employee not found.\n");
        return;
    }

    printf("\n===== EMPLOYEE HIERARCHY =====\n");

    display_subtree(employee,0);

    total=count_under(employee);

    printf("Total Employees Under %d: %d\n",id,total);
}

static void display_complete_hierarchy(){
    Employee *current;
    int total=0;

    if(employees==NULL){
        printf("The organization has no employees.\n");
        return;
    }

    printf("\n===== ORGANIZATION HIERARCHY =====\n");

    current=employees;

    while(current!=NULL){
        if(current->manager==NULL){
            display_subtree(current,0);
            total+=1+count_under(current);
        }

        current=current->next;
    }

    printf("Total Employees: %d\n",total);
}

static void free_organization(){
    Employee *employee;
    Employee *next_employee;
    ReportLink *link;
    ReportLink *next_link;

    employee=employees;

    while(employee!=NULL){
        next_employee=employee->next;
        link=employee->reports;

        while(link!=NULL){
            next_link=link->next;
            free(link);
            link=next_link;
        }

        free(employee);
        employee=next_employee;
    }

    employees=NULL;
}

int main(){
    int choice;

    while(1){
        printf("\n===== EMPLOYEE HIERARCHY MANAGEMENT =====\n");
        printf("1. Add Employee\n");
        printf("2. Remove Employee\n");
        printf("3. Display Employee Hierarchy\n");
        printf("4. Display Complete Hierarchy\n");
        printf("5. Exit\n");

        if(!read_integer("Enter your choice: ",&choice)){
            printf("\nInput ended. Exiting program.\n");
            break;
        }

        switch(choice){
            case 1:
                add_employee();
                break;

            case 2:
                remove_employee();
                break;

            case 3:
                display_employee_hierarchy();
                break;

            case 4:
                display_complete_hierarchy();
                break;

            case 5:
                free_organization();
                printf("Program exited successfully.\n");
                return 0;

            default:
                printf("Invalid choice. Enter a number from 1 to 5.\n");
        }
    }

    free_organization();
    return 0;
}