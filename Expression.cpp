#include <variant>
#include <iostream>
#include <vector>
#include <array>

struct Binary;
struct Literal;

template <typename T> struct recursive_wrapper {
  // construct from an existing object
  recursive_wrapper(T t_) { t.emplace_back(std::move(t_)); }
  // cast back to wrapped type
  operator const T &()  { return t.front(); }
  // store the value
  std::vector<T> t;
};

using Expr = std::variant<recursive_wrapper<Binary>, recursive_wrapper<Literal>>;

struct Binary{

    Binary(Expr A, Expr B) : values{A, B}{}

    std::array<Expr,2> values;

} ;

struct Literal{
    Literal(){};
    int a;

} ;

template<class F, class...Args>
F for_each_arg(F f, Args&&...args) {
  (f(std::forward<Args>(args)),...);
  return f;
}



struct visitor {
    template<typename... Args>
    void parenthesize(Args&&... exprs) {


        for_each_arg([this]( auto e){
            std::visit(*this, static_cast<Expr>(e) );
        }, exprs...);
                      
    } 
    void operator()( const Binary& b)  {
        std::cout << "bin\n";
        //for (  Expr& e : b.values){
           // std::visit(*this, static_cast<Expr>(e) );
           parenthesize( b.values[0],  b.values[1] );
        //}
    }
     void operator()( const Literal& b)  {
        std::cout << "lit\n";
    }
};

int main(){
    visitor v;
    auto l1 = Literal{};
    auto l2 = Literal{};
    //auto expr = Binary(l1, l2);
    Expr expr = Binary(Literal{}, Literal{});

    std::visit(v, expr);
}
