#include<stdio.h>
struct student{
    int roll_no;
    char name[100];
    int m1,m2,m3;
};
int calculate_total(int m1,int m2,int m3){
    int total_marks=m1+m2+m3;
    return total_marks;
}
float calculate_average(int total){
    float avg=total/3.0;
    return avg;
}
char assign_grade(float avg){
    if(avg>=85)
    return 'A';
    else if(avg>=70)
    return 'B';
    else if(avg>=50)
    return 'C';
    else if(avg>=35)
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
int main(){
    int n;
    scanf("%d",&n);
    if(n<1 || n>100){
        printf("Enter a valid number between 1 to 100");
        return 0;
    }
    struct student students[100];
    int roll_no_students[100];
    int total_marks_students[100];
    float average_students[100];
    char grade_students[100];
    char *stars_students[100];
    int i;
    for(i=0;i<n;i++){
        scanf("%d",&students[i].roll_no);
        scanf(" %99[A-Za-z ]",students[i].name);
        scanf("%d %d %d",&students[i].m1,&students[i].m2,&students[i].m3);
        if(students[i].m1<0 || students[i].m1>100 || students[i].m2<0 || students[i].m2>100 || students[i].m3<0 || students[i].m3>100){
            printf("Marks should be within 0 to 100");
            return 0;
        }
        roll_no_students[i]=students[i].roll_no;
        total_marks_students[i]=calculate_total(students[i].m1,students[i].m2,students[i].m3);
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