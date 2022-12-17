//
// Created by winnie泰 on 2022/12/17.
//
using namespace std;

#include "iostream"
#include "Matrix.h"

int main() {
    mat<int> intMat1(4, 4, 1);
    int *data1 = (int *) malloc(4 * 4 * sizeof(int));
    for (int i = 0; i < 16; ++i) {
        *data1 = i;
    }
    intMat1.setData(data1);
    mat<int> intMat2(4, 4, 1);
    int *data2 = (int *) malloc(4 * 4 * sizeof(int));
    for (int i = 0; i < 16; ++i) {
        *data2 = 2*i;
    }
    intMat2.setData(data2);
    mat<int>ans1(4,4,1);
    ans1=intMat1+intMat2;
    cout<<ans1<<endl;
    ans1=((intMat1*intMat2));
    cout<<ans1<<endl;

    mat<int>ans2(4,4,1);
    ans2=intMat1+intMat2;
    cout<<ans2<<endl;

    mat<int>ans3(2,2,2);
    mat<int>ans4(2,2,2);
    int pInt3[]={3,2,3,9,4,2,3,0};
    int pInt4[]={2,0,1,1,2,2,3,0};
    ans3.setData(pInt3);
    ans4.setData(pInt4);
    cout<<ans3<<endl;
    cout<<ans4<<endl;
    cout<<(ans3+ans4)<<endl;
    cout<<2+ans3<<endl;

   mat<float>mat2(3,3,1);
   mat<float>mat3(3,3,1);
   float pFloat2[]={3.0f,2,3,9.3f,4.1f,2,3.2f,0,8.1f};
   float pFloat3[]={1.0f,1,2,2.3f,4.3f,2,3.3f,0.2f,2.1f};
   mat2.setData(pFloat2);
   mat3.setData(pFloat3);
   cout<<mat2*mat3<<endl;

   mat<float>mat5(mat2,1,1,2,2);
   cout<<mat5<<endl;
}