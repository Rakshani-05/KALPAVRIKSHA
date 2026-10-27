#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>
#include<errno.h>

const int STANDARD_PRICE=150;
const int PREMIUM_PRICE=250;
const int RECLINER_PRICE=400;
const float DISCOUNT_RATE=0.10F;

#define MIN_TICKETS 1
#define MAX_TICKETS 20
#define DISCOUNT_MIN_TICKETS 5
#define MIN_CATEGORY 1
#define MAX_CATEGORY 3

struct Bill{
    const char *name;
    const char *category;
    int tickets;
    float subtotal;
    float discount;
    float final_bill;
};

int read_line(char input[],int size){
    if(fgets(input,size,stdin)==NULL){
        return 0;
    }
    
    if(strchr(input,'\n')==NULL){
        int ch;
        while((ch=getchar())!='\n' && ch!=EOF){
        }
        
        return 0;
    }
    
    input[strcspn(input,"\n")]='\0';
    return 1;
}

void read_customer_name(char name[]){
    int valid;
    int i;
    do{
        printf("Enter customer name: ");
        valid=read_line(name,50);
        if(valid){
            if(name[0]=='\0'){
                valid=0;
            }
            for(i=0;name[i]!='\0';i++){
                if(isspace((unsigned char)name[i])){
                    valid=0;
                    break;
                }
            }
        }
        
        if(!valid){
            printf("Invalid name.Please enter a single name without spaces.\n");
        }
    } while(!valid);
}

int read_integer(const char *prompt,int min,int max){
    char input[100];
    char *end;
    long value;
    int valid;
    
    do{
        printf("%s",prompt);
        valid=read_line(input,sizeof(input));
        
        if(valid){
            char *start=input;
            
            while(isspace((unsigned char)*start)){
                start++;
            }
            
            errno=0;
            value=strtol(start,&end,10);
            
            while(isspace((unsigned char)*end)){
                end++;
            }
            
            if(start==end || *end!='\0' || errno==ERANGE || value<min || value>max){
                valid=0;
            }
        }
        
        if(!valid){
            printf("Invalid input.Enter a number between %d and %d.\n",min,max);
        }
    }while(!valid);
    
    return (int)value;
}

int get_ticket_price(int choice,const char **category){
    switch(choice){
        case 1:
            *category="Standard";
            return STANDARD_PRICE;
            
        case 2:
            *category="Premium";
            return PREMIUM_PRICE;
            
        case 3:
            *category="Recliner";
            return RECLINER_PRICE;
            
        default:
            *category="";
            return 0;
    }
}

float calculate_subtotal(int tickets,int price){
    return tickets*price;
}

float calculate_discount(float subtotal,int tickets){
    if(tickets>=DISCOUNT_MIN_TICKETS){
        return subtotal*DISCOUNT_RATE;
    }
    else{
        return 0.0f;
    }
}

void display_bill(const struct Bill *bill){
    printf("\nCustomer: %s\n",bill->name);
    printf("Ticket Categroy: %s\n",bill->category);
    printf("Tickets: %d\n",bill->tickets);
    printf("Subtotal: %.2f\n",bill->subtotal);
    printf("Discount: %.2f\n",bill->discount);
    printf("Final bill: %.2f",bill->final_bill);
}

int main(){
    char customer_name[50];
    const char *category;
    int tickets,choice,ticket_price;
    float subtotal,discount,final_bill;
    
    read_customer_name(customer_name);
    
    tickets=read_integer("Enter number of tickets (1-20): ",MIN_TICKETS,MAX_TICKETS);
    
    choice=read_integer("Enter ticket category (1-Standard, 2-Premium, 3-Recliner): ",MIN_CATEGORY,MAX_CATEGORY);
    
    ticket_price=get_ticket_price(choice,&category);
    
    subtotal=calculate_subtotal(tickets,ticket_price);
    discount=calculate_discount(subtotal,tickets);
    final_bill=subtotal-discount;

    struct Bill bill;
    bill.name=customer_name;
    bill.category=category;
    bill.tickets=tickets;
    bill.subtotal=subtotal;
    bill.discount=discount;
    bill.final_bill=final_bill;
    
    display_bill(&bill);
    
    return 0;
}
