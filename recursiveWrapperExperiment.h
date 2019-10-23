template <typename T> struct recursive_wrapper3 {
  recursive_wrapper3(T t_) : 
     objRef([&]() -> T&  {
           typename std::aligned_storage<256>::type buffer_;
           ::new (&buffer_) T(std::move(static_cast<T>(t_)));
           return *reinterpret_cast<T*>(&buffer_);
      }()){}
  
  operator const T&()  { return objRef; }
  const T& objRef;
};
