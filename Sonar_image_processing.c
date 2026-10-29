#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX 10
#define MIN 2
#define RANGE

void generateMatrix(int matrix[MAX][MAX],int n){
    int i, j;

    for(i=0; i<n; i++){
        for(j = 0; j<n; j++){
            *(*(matrix +i)+j)=rand()%RANGE;
        }
    }
}

void displayMatrix(int matrix[MAX][MAX],int n){
    int i,j;

    for(i=0;i<n;i++){
        for(j=0;j<n;j++){
            printf("%4d",*(*(matrix+i)+j));
        }
        printf("\n");
    }
}

void rotateMatrix(int matrix[MAX][MAX],int n){
    int i,j,temp;

    for(i=0;i<n;i++){
        for(j=i+1;j<n;j++){
            int *p=*(matrix+i)+j;
            int *q=*(matrix+j)+i;

            temp=*p;
            *p=*q;
            *q=temp;
        }
    }

    for(i =0;i<n;i++){
        int *left=*(matrix+i);
        int *right=*(matrix+i)+n-1;

        while(left<right){
            temp=*left;
            *left=*right;
            *right=temp;

            left++;
            right--;
        }
    }
}

void smoothMatrix(int matrix[MAX][MAX],int n){
    int buffer[3][MAX];
    int *prev=*(buffer+0);
    int *curr =*(buffer+1);
    int *next =*(buffer+2);

    int i,j,r,c;
    int sum,count,rowIndex;

    for(j=0;j<n;j++){
        *(curr+j)=*(*(matrix+0)+j);
        *(next+j)=*(*(matrix+1)+j);
    }

    for(i=0;i<n;i++){
        for(j=0;j<n;j++){
            sum=0;
            count=0;

            for(r=-1;r<=1;r++){
                rowIndex=i+r;

                if(rowIndex<0 || rowIndex>=n)
                    continue;

                for(c=-1;c<=1;c++){
                    int colIndex=j+c;
                    int *sourceRow;

                    if(colIndex<0 || colIndex>=n)
                        continue;

                    if(r==-1)
                        sourceRow=prev;
                    else if(r==0)
                        sourceRow=curr;
                    else
                        sourceRow=next;

                    sum+=*(sourceRow+colIndex);
                    count++;
                }
            }

            *(*(matrix+i)+j)=sum/count;
        }

        {
            int *tempRow=prev;
            prev=curr;
            curr=next;
            next=tempRow;
        }

        if(i+2<n){
            for(j=0;j<n;j++){
                *(next+j)=*(*(matrix+i+2)+j);
            }
        }
    }
}

int main(void){
    int matrix[MAX][MAX];
    int n;

    srand((unsigned int)time(NULL));

    printf("Enter matrix size (2 to 10): ");

    if(scanf("%d",&n)!=1 || n<MIN || n>MAX){
        printf("Invalid matrix size.\n");
        return 1;
    }

    generateMatrix(matrix,n);

    printf("\nOriginal Randomly Generated Matrix:\n");
    displayMatrix(matrix,n);

    rotateMatrix(matrix,n);

    printf("\nMatrix after 90 degree Rotation:\n");
    displayMatrix(matrix,n);

    smoothMatrix(matrix,n);

    printf("\nMatrix after Applying 3*3 Smoothing Filter:\n");
    displayMatrix(matrix,n);

    return 0;
}