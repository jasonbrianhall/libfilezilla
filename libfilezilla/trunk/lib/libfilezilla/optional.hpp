#ifndef LIBFILEZILLA_OPTIONAL_HEADER
#define LIBFILEZILLA_OPTIONAL_HEADER

namespace fz {

/** \brief Similar to C++17's std::optional, but stores the data in
 *         dynamic memory.
 *
 * sparse_optional is useful to save memory if it is expected
 * that the object is rarely set and the object's size is bigger than
 * a simple pointer.
 */
template<typename T>
class sparse_optional final
{
	static_assert(sizeof(T) > sizeof(T*), "Trying to use sparse_optional with a small type. Use a regular optional instead for small types.");

public:
	sparse_optional();
	explicit sparse_optional(T const& v);
	sparse_optional(sparse_optional<T> const& v);
	sparse_optional(sparse_optional<T> && v) noexcept;
	~sparse_optional();

	void clear();

	explicit operator bool() const { return v_ != 0; };

	T& operator*() { return *v_; }
	T const& operator*() const { return *v_; }

	T* operator->() { return v_; }
	T const* operator->() const { return v_; }

	bool operator==(sparse_optional<T> const& cmp) const;
	inline bool operator!=(sparse_optional<T> const& cmp) const { return !(*this == cmp); }
	bool operator<(sparse_optional<T> const& cmp) const;

	sparse_optional<T>& operator=(sparse_optional<T> const& v);
	sparse_optional<T>& operator=(sparse_optional<T> && v) noexcept;
protected:
	T* v_;
};


template<typename T>
sparse_optional<T>::sparse_optional()
	: v_()
{
}

template<typename T>
sparse_optional<T>::sparse_optional(T const& v)
	: v_(new T(v))
{
}

template<typename T>
sparse_optional<T>::sparse_optional(sparse_optional<T> const& v)
{
	if (v) {
		v_ = new T(*v);
	}
	else {
		v_ = 0;
	}
}

template<typename T>
sparse_optional<T>::sparse_optional(sparse_optional<T> && v) noexcept
{
	v_ = v.v_;
	v.v_ = 0;
}

template<typename T>
sparse_optional<T>::~sparse_optional()
{
	delete v_;
}

template<typename T>
void sparse_optional<T>::clear()
{
	delete v_;
	v_ = 0;
}

template<typename T>
sparse_optional<T>& sparse_optional<T>::operator=(sparse_optional<T> const& v)
{
	if (this != &v) {
		delete v_;
		if (v.v_) {
			v_ = new T(*v.v_);
		}
		else {
			v_ = 0;
		}
	}

	return *this;
}

template<typename T>
sparse_optional<T>& sparse_optional<T>::operator=(sparse_optional<T> && v) noexcept
{
	if (this != &v) {
		delete v_;
		v_ = v.v_;
		v.v_ = 0;
	}

	return *this;
}

template<typename T>
bool sparse_optional<T>::operator==(sparse_optional<T> const& cmp) const
{
	if (!v_ && !cmp.v_) {
		return true;
	}

	if (!v_ || !cmp.v_) {
		return false;
	}

	return *v_ == *cmp.v_;
}

template<typename T>
bool sparse_optional<T>::operator<(sparse_optional<T> const& cmp) const
{
	if (!v_ || !cmp.v_) {
		return cmp.v_ != 0;
	}

	return *v_ < *cmp.v_;
}

}

#endif
