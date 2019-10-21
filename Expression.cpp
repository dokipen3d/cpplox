
#include <variant>
#include <iostream>
#include <vector>

struct Null   { };
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

//using Expr = x3::variant<x3::forward_ast<Binary>, x3::forward_ast<Literal>>;
using Expr = std::variant<recursive_wrapper<Binary>, recursive_wrapper<Literal>>;

struct Binary{
    //Binary() = default;
    //using Expr::Expr;
    Binary(Expr A, Expr B) : values{A, B}{}

    std::vector<Expr> values;

    Expr asVariant(){
        return *this;
    }

} ;

struct Literal{
    //using Expr::Expr;
    //Literal() : Expr(*this){}
    Literal(){};
    int a;
} ;

struct visitor {
    void operator()(const Binary& b) const {
        std::cout << "bin\n";
    }
     void operator()(const Literal& b) const {
        std::cout << "lit\n";
    }

};

int main(){
    visitor v;
    auto l1 = Literal{};
    auto l2 = Literal{};
    //auto expr = Binary(l1, l2);
     auto expr = Binary(Literal{}, Literal{});

    std::visit(v, expr.asVariant());

}




