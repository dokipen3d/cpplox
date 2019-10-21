#include <iostream>

struct Binary;
struct Literal;

//template <typename returnType>

class Visitor
{
  public:
    virtual void visit(Binary& e) = 0;
    virtual void visit(Literal& e) = 0;
};


struct Expr {
    virtual void accept( Visitor& visitor){};
};

struct Binary : Expr {
    Binary(Expr A, Expr B) : A(A), B(B){}
    void accept( Visitor& visitor) override {
        visitor.visit(*this);
    }
    Expr A;
    Expr B;
};

struct Literal : Expr {
    void accept( Visitor& visitor) override {
        visitor.visit(*this);
    }
    int a;
};

struct exprVisitor :  Visitor{
    void  visit( Binary& p){
        std::cout << "B\n";
        p.A.accept(*this);
    }
    void visit( Literal& p){
         std::cout << "C\n";
    }

};

int main(){
    auto pastry = Binary(Literal{}, Literal{});
    exprVisitor pv;
    pastry.accept(pv);
}
