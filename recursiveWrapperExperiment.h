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


// recursive wrapper with single vector for each type
template <typename T> struct recursive_wrapper {
  // construct from an existing object
  recursive_wrapper(T t_){ 
      t.emplace_back(std::move(t_));
    index = t.size()-1;
  }
  // cast back to wrapped type
  //operator const T &()  { return t.front(); }
  operator const T &()  { return t[index]; }

  // store the value
  static std::vector<T> t;
  int index;
  //std::basic_string<T> t;

};

template <typename T>
std::vector<T> recursive_wrapper<T>::t ;

/////adding nullptr check to Object
using var = std::variant<void*, int>;
struct Object : var{
      using var::var;
      bool operator ==(const std::nullptr_t& other){ return this->index() == 0;}
};

/// even better....


using var = std::variant<void*, int>;
struct Object : var{
      using var::var;
      //bool operator ==(const std::nullptr_t& other){ return this->index() == 0;}
    bool operator ==(const std::nullptr_t& other){ 
        return std::holds_alternative<void*>(*this);}

};
