#include<stdio.h>
#include<math.h>
#define items 5

int main(void){
    //ここでは百分率で格納
    double per[items] = {21.8, 30.5, 53.6, 50.0, 12.9};
    double product = 1.0;
    
    for(int i=0;i<items;i++) product *= 1+per[i]/100;

    double ans = (pow(product,1/items) - 1)/100;
    printf("%.3lf",product);

    return 0;
}