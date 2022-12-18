

# CS205 C/ C++ Programming - Project 5 Report

**Name**: 周三泰 
**SID**: 12112008

## Part 1 - Analysis and practical improvement

本次proj的内容是以C++语言，实现一个矩阵类。

因为需要支持不同的数据类型，包括unsigned char, short, int, float, double, etc，故有两种选择，一是以union来存储数据，让各种数据类型的指针共享一块内存，二是建立一个模版类。经考量，使用union极其繁复，代码重复度高，效率低下，遂选择建立一个模版类。

模版类的成员变量包含：
``` cpp
		size_t row;
    size_t col;
    int *ref;
    size_t roi_s;
    size_t roi_p;
    size_t channel;
    T_ *data;
```
其中roi_s是代表每遍历完一行数据以后，指针需要跳跃的距离。roi_p代表指针指向的地址+该值等于Region Of Interest数据的首地址。

通过重载运算符，实现矩阵支持用+,-,*,<<,+=,-=,=,==等operator进行操作。
通过友援函数，实现矩阵支持和数字被动做运算。

## Part2 - Code and Functions
### myExp类的建立：
```cpp
class myExp : public std::exception {
public:
    string s;

    myExp(const char *what = nullptr) {

        if (what != NULL) { s = what; }
        else {
            s = "unrecognized error";
        }
    }
    
    const char *what() const throw() {
        return s.c_str();
    }

   // ~myExp() throw() {};
};
```

### constructor：

1)
```cpp
mat(const ssize_t row, const ssize_t col, const ssize_t channel = 1, T_ *data = nullptr) {
        if (row <= 0 || col <= 0 || roi_s < 0 || roi_p < 0 || channel < 0)
            throw myExp("Invalid input");
        try {
            this->row = row;
            this->col = col;
            if (!data) {
                this->data = new T_[row * col * sizeof(T_) * channel];
                memset(this->data, 0, sizeof(data));
            } else this->data = data;
            this->ref = new int;
            *(this->ref) = 1;
            this->roi_s = col;
            this->roi_p = 0;
            this->channel = channel;
        }
        catch (bad_alloc &ba) {
            throw ba;
        }
        catch (...) {
            throw myExp("unrecognized error");
        }
    }
```
2)
```cpp
 mat(const mat<T_> &m, const size_t roi_x, const size_t roi_y, const size_t row, const size_t col) {
        if (row <= 0 || col <= 0 || roi_x < 0 || roi_y < 0 || channel < 0)
            throw myExp("Invalid input");
        this->row = row;
        this->col = col;
        this->roi_p = (roi_x) * m.col + roi_y;
        this->roi_s = m.col;
        this->ref = m.ref;
        (*ref)++;
        this->data = m.data;
    }
```
3)
```cpp
mat(const mat<T_> &m) {
        if (row <= 0 || col <= 0 || roi_s < 0 || roi_p < 0 || channel < 0)
            throw myExp("Invalid input");
        this->row = m.row;
        this->col = m.col;
        this->roi_s = m.roi_s;
        this->roi_p = m.roi_p;
        this->ref = m.ref;
        *ref++;
        this->data = data;
    };//默认软拷贝，再提供一个硬拷贝的方案
```
4)
```cpp
mat<T_> &operator=(const mat<T_> &m) {
        if (m.row <= 0 || m.col <= 0 || m.roi_s < 0 || m.roi_p < 0 || m.channel < 0 || m.data == NULL ||
            m.ref == NULL)
            throw myExp("Invalid input for operator=");
        this->row = m.row;
        this->col = m.col;
        this->roi_p = m.roi_p;
        this->roi_s = m.roi_s;
        //(*this->ref)--;
        if (this->ref != NULL && *(this->ref) == 0) {
            free(this->ref);
            this->ref = NULL;
            free(this->data);
        }
        this->ref = m.ref;
        assert(this->ref == m.ref);
        (*this->ref)++;
        this->data = m.data;
        return *this;
    };//默认为soft copy
```

### destructor
```cpp
      ~mat() {
        if (this->ref != NULL && --(*this->ref)) {
            free(this->ref);
            free(this->data);
        }
    }
```

### check
1)
```cpp
bool check() const {
        try {
            if (this->ref == NULL || this->data == NULL || this->row <= 0 || this->col <= 0 || this->roi_p < 0 ||
                this->roi_s < 0 || this->channel < 0)
                return false;
            else return true;
        }
        catch (...) {
            return false;
        }
    }
```
2)
```cpp
    bool check(const mat &mat1) const {
        try {
            if (!this->check() || !mat1.check())return false;
            else if (this->row != mat1.row || this->col != mat1.col || this->channel != mat1.channel)return false;
            else return true;
        } catch (...) { return false; }
    };
```
3)
```cpp
    bool static check(const mat &mat1, const mat &mat2) {
        return mat1.check(mat2);
    };
```

### overload operator
```cpp
mat operator+(const mat &m) {
        if (!this->check(m))throw myExp("invalid operation in operator+");
        mat ans(this->row, this->col, this->channel);
        for (size_t i = 0; i < this->channel; ++i) {
            for (size_t j = 0; j < this->row; ++j) {
                for (size_t k = 0; k < this->col; ++k) {
                    ans.data[i * this->row * this->col + j * this->col + k] =
                            m.data[m.roi_p + i * (this->row * this->col) + j * m.roi_s + k] +
                            this->data[this->roi_p + i * this->row * this->col + j * this->roi_s + k];
                }
            }
        }
        return ans;
    };

    mat operator+(const long double m) {
        mat ans(this->row, this->col, this->channel);
        for (size_t i = 0; i < this->channel; ++i) {
            for (size_t j = 0; j < this->row; ++j) {
                for (size_t k = 0; k < this->col; ++k) {
                    ans.data[i * this->row * this->col + j * this->col + k] =
                            m + this->data[this->roi_p + i * this->row * this->col + j * this->roi_s + k];
                }
            }
        }
        return ans;
    };

    mat operator+(const long long m) {
        mat ans(this->row, this->col, this->channel);
        for (size_t i = 0; i < this->channel; ++i) {
            for (size_t j = 0; j < this->row; ++j) {
                for (size_t k = 0; k < this->col; ++k) {
                    ans.data[i * this->row * this->col + j * this->col + k] =
                            m + this->data[this->roi_p + i * this->row * this->col + j * this->roi_s + k];
                }
            }
        }
    };

    mat &operator+=(const mat &m) {
        if (!this->check(m))throw myExp("invalid operation in operator+=");
        for (size_t i = 0; i < this->channel; ++i) {
            for (size_t j = 0; j < this->row; ++j) {
                for (size_t k = 0; k < this->col; ++k) {
                    this->data[this->roi_p + i * this->row * this->col + j * this->roi_s + k] +=
                            m.data[m.roi_p + i * this->row * this->col + j * this->roi_s + k];
                }
            }
        }
        return *this;
    };

    mat operator+=(const long double m) {
        for (size_t i = 0; i < this->channel; ++i) {
            for (size_t j = 0; j < this->row; ++j) {
                for (size_t k = 0; k < this->col; ++k) {
                    this->data[this->roi_p + i * this->row * this->col + j * this->roi_s + k] += m;
                }
            }
        }
        return *this;
    };

    mat operator+=(const long long m) {
        for (size_t i = 0; i < this->channel; ++i) {
            for (size_t j = 0; j < this->row; ++j) {
                for (size_t k = 0; k < this->col; ++k) {
                    this->data[this->roi_p + i * this->row * this->col + j * this->roi_s + k] += m;
                }
            }
        }
        return *this;
    };
```
>减法与加法相似，遂不展示

```cpp
mat operator*(const mat &m) {
        if (!check(m) || this->col != m.row)throw myExp("invalid input for operator*");
        mat ans(this->row, this->col, this->channel);
#pragma omp parallel for
        for (size_t i = 0; i < this->channel; ++i) {
            for (size_t j = 0; j < this->row; ++j) {
                for (size_t k = 0; k < this->col; ++k) {
                    for (size_t l = 0; l < this->col; ++l) {
                        ans.data[j * ans.col + k + i * (ans.row * ans.col)] +=
                                this->data[this->roi_p + j * this->roi_s + l + i * (ans.row * ans.col)] *
                                m.data[m.roi_p + k + l * m.roi_s + i * (ans.row * ans.col)];
                    }
                }
            }
        }
        return ans;
    };

    mat operator*(const long double m) {
        mat ans(this->row, this->col, this->channel);
#pragma omp parallel for
        for (size_t i = 0; i < this->channel; ++i) {
            for (size_t j = 0; j < this->row; ++j) {
                for (size_t k = 0; k < this->col; ++k) {
                    ans.data[j * ans.col + k + i * (ans.row * ans.col)] =
                            this->data[this->roi_p + j * this->roi_s + k + i * (ans.row * ans.col)] *
                            m;
                }

            }
        }
        return ans;
    };

    mat operator*(const long long m) {
        mat ans(this->row, this->col, this->channel);
#pragma omp parallel for
        for (size_t i = 0; i < this->channel; ++i) {
            for (size_t j = 0; j < this->row; ++j) {
                for (size_t k = 0; k < this->col; ++k) {
                    ans.data[j * ans.col + k + i * (ans.row * ans.col)] =
                            this->data[this->roi_p + j * this->roi_s + k + i * (ans.row * ans.col)] *
                            m;
                }

            }
        }
        return ans;
    };
```


```cpp
    bool operator==(const mat &m) {
        if (!check(m) || !this->check(m))throw myExp("invalid input for operator*");
        if (typeid(T_) == typeid(float)) {
            for (int i = 0; i < this->channel; ++i) {
                for (int j = 0; j < this->row; ++j) {
                    for (int k = 0; k < this->col; ++k) {
                        if (fabs(this->data[roi_p + i * this->row * this->col + j * roi_s + k] -
                                 m.data[roi_p + i * this->row * this->col + j * roi_s + k]) > FLT_EPSILON)
                            return false;
                    }
                }
            }
            return true;
        }
        if (typeid(T_) == typeid(double)) {
            for (int i = 0; i < this->channel; ++i) {
                for (int j = 0; j < this->row; ++j) {
                    for (int k = 0; k < this->col; ++k) {
                        if (fabs(this->data[roi_p + i * this->row * this->col + j * roi_s + k] -
                                 m.data[roi_p + i * this->row * this->col + j * roi_s + k]) > DBL_EPSILON)
                            return false;
                    }
                }
            }
            return true;
        }else{
            for (int i = 0; i < this->channel; ++i) {
                for (int j = 0; j < this->row; ++j) {
                    for (int k = 0; k < this->col; ++k) {
                        if ((this->data[roi_p + i * this->row * this->col + j * roi_s + k] =
                                 m.data[roi_p + i * this->row * this->col + j * roi_s + k]) )
                            return false;
                    }
                }
            }
        }
    }
```

```cpp
    friend ostream &operator<<(ostream &o, const mat m) {
        o << "the row of matrix is " << m.row << endl;
        o << "the col of matrix is " << m.col << endl;
        o << "the channel of matrix is " << m.channel << endl;
        o << "data is as follows: ";
        for (size_t i = 0; i < m.channel; ++i) {
            o << "channel " << i << ": \n";
            for (size_t j = 0; j < m.row; ++j) {
                for (size_t k = 0; k < m.col; ++k) {
                    o << m.data[m.roi_p + i * m.row * m.col + j * m.roi_s + k] << " ";
                }
                o << endl;
            }
        }
    };

    friend mat<T_> operator+(long double t,  mat<T_> &m) {
        return m + t;
    };

    friend mat<T_> operator-(long double t,  mat<T_> &m) {
        return m - t;
    };

    friend mat<T_> operator*(long double t, mat<T_> &m) {
        return m * t;
    };
```

## Test and Verification

test.cpp:
```cpp
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
```
![截屏2022-12-18 19.16.49](../../../Library/Application Support/typora-user-images/截屏2022-12-18 19.16.49.png)

![截屏2022-12-18 19.17.03](../../../Library/Application Support/typora-user-images/截屏2022-12-18 19.17.03.png)

![截屏2022-12-18 19.17.10](../../../Library/Application Support/typora-user-images/截屏2022-12-18 19.17.10.png)

**MY SPECIFIC CODE,SEE IN**: https://github.com/zhousantai?tab=repositories









