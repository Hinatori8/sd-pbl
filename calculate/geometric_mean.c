#include<stdio.h>
#include<math.h>

double geometric_mean(double arr[], int size){
    double product = 1.0;
    for(int i=0;i<size;i++) {
        product *= 1+arr[i]/100.0;
    }
    return pow(product,1.0/size); 
}

int main(void) {
    double arr[5] = {21.8, 30.5, 53.6, 50.0, 12.9};
    double percent = 100*(geometric_mean(arr,5) - 1);
    printf("%lf",percent);//％
}