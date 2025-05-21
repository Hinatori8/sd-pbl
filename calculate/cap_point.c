#include<stdio.h>
#include<math.h>

int Lorentz_function(int class ,int members) {
    double numerator = 19.0;
    double denominator = 1.0 + 0.01*pow((class - members),2);
    if(class < members) return 0;
    return round(1+numerator/denominator);
} 
int main(void) {
    int class; // 教室のキャパ（引数）
    int members = 12; // ユーザーの利用者数 ←ｘ座標がこのとき、最大値となるように平行移動させる
    printf("最大評価を行う要求人数は%dです.\n",members);
    scanf("%d",&class);
    int ans = Lorentz_function(class,members);
    printf("評価点は%d点です.\n",ans);
}