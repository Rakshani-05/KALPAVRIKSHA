#include<stdio.h>
#define MAX_STUDENTS 100
#define MAX_SIZE 100

struct student{
    int roll_no;
    char name[MAX_SIZE];
    int mark1,mark2,mark3;
};

int calculate_total(int mark1,int mark2,int mark3){
    int total_marks=mark1+mark2+mark3;
    return total_marks;
}

float calculate_average(int total){
    float average_score=total/3.0;
    return average_score;
}

char assign_grade(float average_score){
    if(average_score>=85)
        return 'A';
    else if(average_score>=70)
        return 'B';
    else if(average_score>=50)
        return 'C';
    else if(average_score>=35)
        return 'D';
    else
        return 'F';
}

char *stars(char grade){
    if(grade=='A'){
        return "*****";
    }
    else if(grade=='B'){
        return "****";
    }
    else if(grade=='C'){
        return "***";
    }
    else if(grade=='D'){
        return "**";
    }
    else
        return "";
        
}

void print_roll_no(int roll_no_students[],int index,int n){
    if(index==n){
        return;
    }
    
    printf("%d ",roll_no_students[index]);
    print_roll_no(roll_no_students,index+1,n);
    
}

int invalid_roll_no(int roll_no_students[],int roll_no,int count){
    if(roll_no<=0){
        printf("Roll no must be positive,");
        return 1;
    }
    
    for(int i=0;i<count;i++){
        
        if(roll_no==roll_no_students[i]){
            printf("Roll number already exists,");
            return 1;
        }
    }   
    return 0;
}
int read_student_count(void){
    int n;
    int result;
    int ch;
    
    printf("Enter the number of students (1-100): ");
    
    while(1){
        result=scanf("%d",&n);
        
        if(result==EOF){
            return 0;
        }
        
        if(result!=1){
            printf("Invalid input.Enter a number between 1 and 100: ");
            
            while((ch=getchar())!='\n' && ch!=EOF){
                
            }
            continue;
        }
        
        while((ch=getchar())!='\n' && ch!=EOF){
            if(ch!=' ' && ch!='\t' && ch!='\r'){
                result=0;
            }
        }
        
        if(result==0){
            printf("Invalid input.Enter a single integer.\n");
            continue;
        }
        
        if(n<1 || n>MAX_STUDENTS){
            printf("Number of students must be 1 and 100.\n");
            continue;
        }
        
        return n;
    }
}

void read_student_details(struct student students[],int roll_no_students[],int n){
    int result;
    int ch;
    for(int i=0;i<n;i++){
        
        while(1){
            printf("Enter the roll number for the student: ");
            result=scanf("%d",&students[i].roll_no);
            
            if(result!=1){
                printf("Invalid input. Enter a valid roll number: ");
                
                while((ch=getchar())!='\n' && ch!=EOF){
                    
                }
                continue;
            }
            
            while((ch=getchar())!='\n' && ch!=EOF){
                if(ch!=' ' && ch!='\t' && ch!='\r'){
                    result=0;
                }
            }
            
            if(result==0){
                printf("Enter only the roll number.\n");
                continue;
            }
            
            if(invalid_roll_no(roll_no_students,students[i].roll_no,i)){
                printf("Enter a valid, unique roll number: ");
                continue;
            }
            break;
        }
        
        printf("Enter the name of the student: ");
        if(scanf(" %99[^\n]",students[i].name)!=1){
            printf("Invalid name input.\n");
            students[i].name[0]='\0';
        }
        
        while((ch=getchar())!='\n' && ch!=EOF){
        }
        
        while(1) {
            printf("Enter three subject marks (0-100): ");
        
            result=scanf("%d %d %d",
                        &students[i].mark1,
                        &students[i].mark2,
                        &students[i].mark3);
        
            if(result != 3){
                printf("Invalid input. Enter three integer marks.\n");
        
                while((ch = getchar()) != '\n' && ch != EOF) {
        
                }   
                continue;
            }
            
            while((ch=getchar())!='\n' && ch!=EOF){
                if(ch!=' ' && ch!='\t' && ch!='\r'){
                    result=0;
                }
            }
            
            if(result==0){
                printf("Enter exactly three integer marks.\n");
                continue;
            }
        
            if(students[i].mark1<0 || students[i].mark1>100 ||
                students[i].mark2<0 || students[i].mark2>100 ||
                students[i].mark3<0 || students[i].mark3>100){
        
                printf("Marks must be between 0 and 100.\n");
                continue;
            }
            
            break;
        
        }

        roll_no_students[i]=students[i].roll_no;
    
    }
}

int main(){
    int n;
    
    struct student students[MAX_STUDENTS];
    int roll_no_students[MAX_STUDENTS];
    int total_marks_students[MAX_STUDENTS];
    float average_students[MAX_STUDENTS];
    char grade_students[MAX_STUDENTS];
    char *stars_students[MAX_STUDENTS];
    
    n=read_student_count();
    
    read_student_details(students,roll_no_students,n);
    
    for(int i=0;i<n;i++){
        total_marks_students[i]=calculate_total(students[i].mark1,students[i].mark2,students[i].mark3);
        average_students[i]=calculate_average(total_marks_students[i]);
        grade_students[i]=assign_grade(average_students[i]);
        stars_students[i]=stars(grade_students[i]);
    }
    
    for(int i=0;i<n;i++){
        printf("Roll: %d\n",students[i].roll_no);
        printf("Name: %s\n",students[i].name);
        printf("Total: %d\n",total_marks_students[i]);
        printf("Average: %.2f\n",average_students[i]);
        printf("Grade: %c\n",grade_students[i]);
        
        if(average_students[i]<35){
            continue;
        }
        
        printf("Performance: %s\n",stars_students[i]);
    }
    
    printf("\n");
    printf("List of Roll Numbers (via recursion): ");
    print_roll_no(roll_no_students,0,n);
    return 0;
    
}
