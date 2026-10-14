#include<stdio.h>
#include<ctype.h>
#define MAX_SIZE 100
int operand_stack[MAX_SIZE];
int top_operand=-1;
char operator_stack[MAX_SIZE];
int top_operator=-1;

int precedence_operator(char operator){
    if(operator=='+' || operator=='-'){
        return 1;
    }
    else if(operator=='*' || operator=='/'){
        return 2;
    }
    return 0;
}

void push_operand(int operand){
    if(top_operand<MAX_SIZE-1){
    operand_stack[++top_operand]=operand;
    }
}

int pop_operand(int *error){
    if(top_operand<0){
        *error=1;
        return 0;
    }
    int popped_value=operand_stack[top_operand--];
    return popped_value;
}

void push_operator(char operator){
    if(top_operator<MAX_SIZE-1){
        operator_stack[++top_operator]=operator;
    }
}

char pop_operator(int *error){
    if(top_operator<0){
        *error=1;
        return '\0';
    }
    char pop_value=operator_stack[top_operator--];
    return pop_value;
}

int calculate_operation(int left_operand,int right_operand,char operator,int *error){
    if(operator=='+'){
        return left_operand+right_operand;
    }
    else if(operator=='-'){
        return left_operand-right_operand;
    }
    else if(operator=='*'){
        return left_operand*right_operand;
    }
    else if(operator=='/'){
        if(right_operand==0){
            *error=2;
            return 0;
        }
        return left_operand/right_operand;
    }
    else{
        *error=1;
        return 0;
    }
}

int evaluate_expression(char expression[],int *error){
    int i=0;
    int expect_num=1;
    top_operand=-1;
    top_operator=-1;
    *error=0;
    while(expression[i]!='\0'){
        if(isspace((unsigned char)expression[i])){
            i++;
            continue;
        }
        if(isdigit((unsigned char)expression[i])){
            int curr_num=0;
            while(1){
                if(isdigit((unsigned char)expression[i])){
                    curr_num=curr_num*10+(expression[i]-'0');
                    i++;
                }
                else if(isspace((unsigned char)expression[i])){
                    int j=i;
                    while(isspace((unsigned char)expression[j])){
                        j++;
                    }
                    if(isdigit((unsigned char)expression[j])){
                        i=j;
                    }
                    else{
                        break;
                    }
                }
                else{
                    break;
                }
            }
            if(!expect_num){
                *error=1;
                return 0;
            }
            push_operand(curr_num);
            expect_num=0;
        }
        else if(expression[i]=='+' || expression[i]=='-' || expression[i]=='*' || expression[i]=='/'){
            if(expect_num){
                *error=1;
                return 0;
            }
            while(top_operator!=-1 && precedence_operator(operator_stack[top_operator])>=precedence_operator(expression[i])){
                int right_operand=pop_operand(error);
                int left_operand=pop_operand(error);
                char operator=pop_operator(error);

                if(*error){
                    return 0;
                }

                int result=calculate_operation(left_operand,right_operand,operator,error);

                if(*error){
                    return 0;
                }

                push_operand(result);
            }

            push_operator(expression[i]);
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

    while(top_operator!=-1){
        int right_operand=pop_operand(error);
        int left_operand=pop_operand(error);
        char operator=pop_operator(error);

        if(*error){
            return 0;
        }

        int result=calculate_operation(left_operand,right_operand,operator,error);

        if(*error){
            return 0;
        }

        push_operand(result);
    }

    if(top_operand!=0){
        *error=1;
        return 0;
    }

    return operand_stack[top_operand];
}

int main(){
    char expression[MAX_SIZE];
    printf("Enter an expression: ");
    fflush(stdout);
    if(fgets(expression,sizeof(expression),stdin)==NULL){
        printf("Error: Invalid expression.\n");
        return 1;
    }
    int error=0;
    int result=evaluate_expression(expression,&error);
    if(error==1){
        printf("Error: Invalid expression.");
    }
    else if(error==2){
        printf("Error: Division by zero.");
    }
    else{
        printf("%d ",result);
    }
    return 0;
}