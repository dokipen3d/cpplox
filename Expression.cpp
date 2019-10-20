#include <iostream>

struct Binary;
struct Literal;

//template <typename returnType>
struct exprVisitor {
    auto visitBeignet( Binary& p){
        std::cout << "B\n";
    }
    auto visitCruller( Literal& p){
         std::cout << "C\n";
    }

};

struct Expr {
    virtual void accept( exprVisitor& visitor){};
};

struct Binary : Expr {
    Binary(Expr A, Expr B) : A(A), B(B){}
    void accept( exprVisitor& visitor) override {
        visitor.visitBeignet(*this);
    }
    Expr A;
    Expr B;
};

struct Literal : Expr {
    void accept( exprVisitor& visitor) override {
        visitor.visitCruller(*this);
    }
    int a;
};


int main(){
    auto pastry = Binary(Literal{}, Literal{});
    exprVisitor pv;
    pastry.accept(pv);
}
