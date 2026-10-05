#include<stdio.h>
#include<ctype.h>
#define stack_size 100
int stack[100];
int ind=-1;
char op_stack[100];
int op_ind=-1;
int res=-1;
int precedence(char ch){
    if(ch=='+' || ch=='-'){
        return 1;
    }
    else if(ch=='*' || ch=='/'){
        return 2;
    }
    return 0;
}
void push(int n){
    if(ind<stack_size-1){
    stack[++ind]=n;
    }
}
int pop(int *error){
    if(ind<0){
        *error=1;
        return 0;
    }
    int popped_value=stack[ind--];
    return popped_value;
}
void push_op(char s){
    if(op_ind<stack_size-1){
        op_stack[++op_ind]=s;
    }
}
char pop_op(int *error){
    if(op_ind<0){
        *error=1;
        return '\0';
    }
    char pop_value=op_stack[op_ind--];
    return pop_value;
}
int calculate(int a,int b,char ch,int *error){
    if(ch=='+'){
        return a+b;
    }
    else if(ch=='-')
    return a-b;
    else if(ch=='*')
    return a*b;
    else if(ch=='/'){
        if(b==0){
            *error=2;
            return 0;
        }
        else{
            return a/b;
        }
    }
    else{
        *error=1;
        return 0;
    }
}
int evaluate(char str[],int *error){
    int i=0;
    int expect_num=1;
    while(str[i]!='\0'){
    if(isspace(str[i])){
        i++;
        continue;
    }
    else if(isdigit(str[i])){
        int curr_num=0;
        while(isdigit(str[i])){
           curr_num=curr_num*10+(str[i]-'0');
           i++;
        }
        push(curr_num);
        expect_num=0;
    }
    else if(str[i]=='+' || str[i]=='-' || str[i]=='*' || str[i]=='/'){
        if(expect_num){
            *error=1;
            return 0;
        }
        while(op_ind!=-1 && precedence(op_stack[op_ind])>=precedence(str[i])){
        int b=pop(error);
        int a=pop(error);
        char ch=pop_op(error);
        if(*error){
            return 0;
        }
        int res=calculate(a,b,ch,error);
        if(*error){
            return 0;
        }
        push(res);
        }
        push_op(str[i]);
        i++;
        expect_num=1;
    }
    else{
     *error=1;
     return 0;
    }
}
if(expect_num){
    *error=1;
    return 0;
}
while(op_ind!=-1){
    int b=pop(error);
    int a=pop(error);
    char ch=pop_op(error);
    if(*error)
    return 0;
    int res=calculate(a,b,ch,error);
    if(*error)
    return 0;
    push(res);
}
return stack[ind];
}
int main(){
    char str[100];
    fgets(str,sizeof(str),stdin);
    int error=0;
    int res=evaluate(str,&error);
    if(error==1){
        printf("Error: Invalid expression.");
    }
    else if(error==2){
        printf("Error: Division by zero.");
    }
    else{
    printf("%d ",res);
    }
}