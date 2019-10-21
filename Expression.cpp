#include <variant>
#include <iostream>
#include <vector>

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
    std::vector<Expr> values;
} ;

struct Literal{
    Literal(){};
    int a;
} ;

struct visitor {
    void operator()(const Binary& b) const {
        std::cout << "bin\n";
        for (auto& e : b.values){
            std::visit(*this, static_cast<Expr>(e) );
        }
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
    Expr expr = Binary(Literal{}, Literal{});

    std::visit(v, expr);
}
