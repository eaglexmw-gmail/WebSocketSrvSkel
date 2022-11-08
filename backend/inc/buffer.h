#ifndef _BUFFER_H_
#define _BUFFER_H_

template <class T>
class Buffer
{
public:
	explicit Buffer(std::size_t size):
		size_(size),
        _cap(size), 
		_ptr(new T[size]) {
	}
	~Buffer() {
		delete [] _ptr;
	}
	void resize(std::size_t newSize, bool preserveContent = true) {
		T* ptr = new T[newSize];
		if (preserveContent) {
			std::size_t n = newSize > size_ ? size_ : newSize;
			std::memcpy(ptr, _ptr, n);
		}
		delete [] _ptr;
		_ptr  = ptr;
		size_ = newSize;
	}
	std::size_t size() const {
		return size_;
	}
	T* begin() {
		return _ptr;
	}
	const T* begin() const {
		return _ptr;
	}
	T* end() {
		return _ptr + size_;
	}
	const T* end() const {
		return _ptr + size_;
	}
	T& operator [] (std::size_t index) {
		assert (index < size_);
		
		return _ptr[index];
	}
	const T& operator [] (std::size_t index) const {
		assert (index < size_);
		
		return _ptr[index];
	}

private:
	Buffer();
	Buffer(const Buffer& src);
	Buffer& operator = (const Buffer&);

	std::size_t size_;
	T* _ptr;
};

#endif
