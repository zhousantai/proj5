//
// Created by winnie泰 on 2022/12/4.
//

#ifndef PROJ5_mat_H
#define PROJ5_mat_H

#include "iostream"
#include "cstdlib"
//#define NDEBUG
#include "cassert"
#include "cmath"
#include "cfloat"

using namespace std;

#include "omp.h"

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

template<typename T_>
class mat {

    size_t row;
    size_t col;
    int *ref;
    size_t roi_s;
    size_t roi_p;
    size_t channel;
    T_ *data;
public:
    //mat(const ssize_t row,const ssize_t col,const ssize_t roi_x,const ssize_t roi_y,const ssize_t channel,T_* data= nullptr)
    mat(const ssize_t row, const ssize_t col, const ssize_t channel = 1, T_ *data = nullptr) {
        if (row <= 0 || col <= 0 || roi_s < 0 || roi_p < 0 || channel < 0 )
            throw myExp("Invalid input");
        try {
            this->row = row;
            this->col = col;
            if (!data)this->data = (T_ *) malloc(row * col * sizeof(T_) * channel);
            this->ref = new int;
            *ref++;
            this->roi_s = col;
            this->roi_p = 0;
        }
        catch (bad_alloc &ba) {
            throw ba;
        }
        catch (...) {
            throw myExp("unrecognized error");
        }
    }

    mat(const mat<T_> &m,const size_t roi_x, const size_t roi_y,const size_t row, const size_t col) {
        if (row <= 0 || col <= 0 || roi_x < 0 || roi_y < 0 || channel < 0 )
            throw myExp("Invalid input");
        this->row = row;
        this->col = col;
        this->roi_p = (roi_x) * m.col + roi_y;
        this->roi_s = m.col;
        this->ref = m.ref;
        *ref++;
        this->data = data;
    }


    mat(const mat<T_> &m) {
        if (row <= 0 || col <= 0 || roi_s < 0 || roi_p < 0 || channel < 0 )
            throw myExp("Invalid input");
        this->row = m.row;
        this->col = m.col;
        this->roi_s = m.roi_s;
        this->roi_p = m.roi_p;
        this->ref = m.ref;
        *ref++;
        this->data = data;
    };//默认软拷贝，再提供一个硬拷贝的方案

    bool hCopy(const mat<T_> &m) {
        if (m.row <= 0 || m.col <= 0 || m.roi_s < 0 || m.roi_p < 0 || m.channel < 0 || m.data == NULL ||
            m.ref == NULL)
            throw myExp("Invalid input");
        this->row = m.row;
        this->col = m.col;
        this->roi_p = m.roi_p;
        this->roi_s = m.roi_s;
        *this->ref++;
        if (this->ref == 0) {
            delete[]this->data;
            delete this->data;
        }
        this->ref = (int *) malloc(sizeof(int));
        *this->ref = 0;
        try {
            this->data == (T_ *) malloc(m.row * m.col * sizeof(T_) * channel);
        }
        catch (bad_alloc &ba) {
            throw ba;
        }
        for (size_t i = 0; i < m.channel; ++i) {
            for (size_t j = 0; j < m.row; ++j) {
                for (size_t k = 0; k < m.col; ++k) {
                    this->data = m.data[roi_p+k + j * roi_s + i * m.row * m.col];
                }
            }
        }

    }//提供一个硬拷贝的方案

    mat<T_> &operator=(const mat<T_> &m) {
        if (m.row <= 0 || m.col <= 0 || m.roi_s < 0 || m.roi_p < 0 || m.channel < 0 || m.data == NULL ||
            m.ref == NULL)
            throw myExp("Invalid input for operator=");
        this->row = m.row;
        this->col = m.col;
        this->roi_p = m.roi_p;
        this->roi_s = m.roi_s;
        *this->ref--;
        if (this->ref == 0) {
            delete[]this->data;
            delete this->data;
        }
        this->ref = m.ref;
        assert(this->ref == m.ref);
        *(this->ref)++;
        this->data = m.data;
        return *this;
    };//默认为soft copy

    bool myFree() {
        try {
            this->ref--;
            if (*this->ref == 0) {
                delete this->ref;
                delete[]this->data;
                delete this->data;
            }
            return true;
        } catch (...) {
            return false;
        }
    };

    ~mat() {
        *(this->ref)--;
        if (*(this->ref)) {
            delete this->ref;
            delete[]this->data;
            delete this->data;
        }
    }

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

    bool check(const mat &mat1) const {
        try {
            if (!this->check() || !mat1.check())return false;
            else if (this->row != mat1.row || this->col != mat1.col || this->channel != mat1.channel)return false;
            else return true;
        } catch (...) { return false; }
    };

    bool static check(const mat &mat1, const mat &mat2) {
        return mat1.check(mat2);
    };

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

    ///
    mat<T_> operator-(const mat<T_> &m) {
        if (!this->check(m))throw myExp("invalid operation in operator-");
        mat ans(this->row, this->col, this->channel);
        for (size_t i = 0; i < this->channel; ++i) {
            for (size_t j = 0; j < this->row; ++j) {
                for (size_t k = 0; k < this->col; ++k) {
                    ans.data[i * this->row * this->col + j * this->col + k] =
                            this->data[this->roi_p + i * this->row * this->col + j * this->roi_s + k] -
                            m.data[m.roi_p + i * this->row * this->col + j * this->roi_s + k];
                }
            }
        }
        return ans;
    };

    mat operator-(const long double m) {
        mat ans(this->row, this->col, this->channel);
        for (size_t i = 0; i < this->channel; ++i) {
            for (size_t j = 0; j < this->row; ++j) {
                for (size_t k = 0; k < this->col; ++k) {
                    ans.data[i * this->row * this->col + j * this->col + k] =
                            this->data[this->roi_p + i * this->row * this->col + j * this->roi_s + k] - m;
                }
            }
        }
        return ans;
    };

    mat operator-(const long long m) {
        mat ans(this->row, this->col, this->channel);
        for (size_t i = 0; i < this->channel; ++i) {
            for (size_t j = 0; j < this->row; ++j) {
                for (size_t k = 0; k < this->col; ++k) {
                    ans.data[i * this->row * this->col + j * this->col + k] =
                            this->data[this->roi_p + i * this->row * this->col + j * this->roi_s + k] - m;
                }
            }
        }
    };

    mat &operator-=(const mat &m) {
        if (!this->check(m))throw myExp("invalid operation in operator-=");
        for (size_t i = 0; i < this->channel; ++i) {
            for (size_t j = 0; j < this->row; ++j) {
                for (size_t k = 0; k < this->col; ++k) {
                    this->data[this->roi_p + i * this->row * this->col + j * this->roi_s + k] -= m.data[m.roi_p +
                                                                                                        i * this->row *
                                                                                                        this->col +
                                                                                                        j * m.roi_s +
                                                                                                        k];
                }
            }
        }
        return *this;
    };

    mat operator-=(const long double m) {
        for (size_t i = 0; i < this->channel; ++i) {
            for (size_t j = 0; j < this->row; ++j) {
                for (size_t k = 0; k < this->col; ++k) {
                    this->data[this->roi_p + i * this->row * this->col + j * this->roi_s + k] -= m;
                }
            }
        }
        return *this;
    };

    mat operator-=(long long m) {
        for (size_t i = 0; i < this->channel; ++i) {
            for (size_t j = 0; j < this->row; ++j) {
                for (size_t k = 0; k < this->col; ++k) {
                    this->data[this->roi_p + i * this->row * this->col + j * this->roi_s + k] -= m;
                }
            }
        }
        return *this;
    };

    ///

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

    size_t getRow() const {
        return row;
    }

    void setRow(size_t row) {
        mat::row = row;
    }

    size_t getCol() const {
        return col;
    }

    void setCol(size_t col) {
        mat::col = col;
    }

    int *getRef() const {
        return ref;
    }

    void setRef(int *ref) {
        mat::ref = ref;
    }

    size_t getRoiS() const {
        return roi_s;
    }

    void setRoiS(size_t roiS) {
        roi_s = roiS;
    }

    size_t getRoiP() const {
        return roi_p;
    }

    void setRoiP(size_t roiP) {
        roi_p = roiP;
    }

    size_t getChannel() const {
        return channel;
    }

    void setChannel(size_t channel) {
        mat::channel = channel;
    }

    T_ *getData() const {
        return data;
    }

    void setData(T_ *data) {
        mat::data = data;
    }


};



//template class Intmat<int>;

#endif //PROJ5_mat_H
