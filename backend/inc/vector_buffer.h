#ifndef _VECTOR_BUFFER_H_
#define _VECTOR_BUFFER_H_

template<class T>
class vectorbuf {
public:
    //----------------迭代器--------------------------
    typedef vectorbuf<T> Self;
    typedef T* iterator;
    typedef const T* const_iterator;

    iterator begin() const {
        return _start;
    }
    iterator end() const {
        return _finish;
    }

public:
    //-----------------init-------------------------------
    vectorbuf()
        :_start(nullptr)
        , _finish(nullptr)
        , _endofstor(nullptr)
    {}
    vectorbuf(unsigned int n, const T& data)
        :_start(new T[n])
    {
        for (unsigned int i = 0;i < n;i++) {
            _start[i] = data;
        }
        _finish = _endofstor = _start + n;
    }
    template<class Iterator>
    vectorbuf(Iterator frist, Iterator last) {
        size_t count = 0;
        auto it = frist;
        while (it != last) {
            it++;
            count++;
        }
        _start = new T[count];

        for (size_t i = 0; i < count; i++) {
            _start[i] = *frist++;
        }
        _finish = _endofstor = _start + count;
    }
    vectorbuf(const Self& v) {
        _start = _finish = new T[v.size()];
        _endofstorage = _start + v.size();
        iterator it = v.begin();
        while (_finish != _endofstor) {
            *_finish = *it;
            _finish++;
            it++;
        }
    }
#if __cplusplus >= 201103L
    // c++11支持右值引用
    vectorbuf(Self&& v) {
        std::move(_start, v._start)
        std::move(_finish, v._finish)
        std::move(_endofstor, v._endofstor)
    }
#endif
    ~vectorbuf() {
        clear();
    }
public:
    //----------------重载-----------------------------
    T& operator[](unsigned int i) {
        return *(_finish + i);
    }

    size_t size()const {
        return _finish - _start;
    }
    size_t capacity()const {
        return _endofstor - _start;
    }
    bool empty() {
        return _start == _finish;
    }
    void reserve(size_t n) {
        size_t oldsize = size();
        if (n <= capacity()) {
            return;
        }
        T* p = new T[n];
        memcpy(p, _start, sizeof(T)*size());
        delete[] _start;
        _start = p;
        _finish = p + oldsize;
        _endofstor = _start + n;
    }
    void resize(size_t newsize, const T& data) {
        reserve(newsize);
        auto it = newsize - size() + _finish;
        for (;_finish < it;_finish++) {
            *_finish = data;
        }
    }
    iterator insert(iterator pos, const T& data) {
        if (_finish == _endofstor) {
            reserve(capacity() * 2);
        }
        auto it = _finish();
        while (it != pos) {
            *it = *(it - 1);
            it--;
        }
        *it = data;
        ++_finish;
        return pos;
    }
    void swap(Self& v) {
        T* _Tmp = _start;
        _start = v._start;
        v._start = _Tmp;

        _Tmp = _finish;
        _finish = v._finish;
        v._finish = v._Tmp;

        _Tmp = _endofstor;
        _endofstor = v._endofstor;
        v._endofstor = _Tmp;
    }
    void push_back(const T& data) {
        if (_finish == _endofstor) {
            reserve(size() * 2);
        }
        *_finish = data;
        _finish++;
    }
    void pop_back() {
        _finish--;
    }
    T front() {
        return *_start;
    }
    T back() {
        return *(_finish - 1);
    }
    void clear() {
        if (_start == nullptr) {
            return;
        }
        delete[] _start;
        _start = _endofstor = _finish = nullptr;
    }

private:
    T* _start;
    T* _finish;
    T* _endofstor;
};

#endif
