#pragma once

#include <iostream>
#include <functional>
#include <vector>



template <typename T>
class shared_ptr {
public:
    using DeleteFunctionType = std::function<void(T*)>;
    using element_type = T;
    explicit shared_ptr(): 
        val_(nullptr), 
        ctrlBlock_(nullptr) 
    {
    }

    shared_ptr(std::nullptr_t,
                       DeleteFunctionType = [](T* val) {
                           delete val;
                       }): 
        val_(nullptr),
        ctrlBlock_(nullptr) 
    {
    }

    explicit shared_ptr(T* val, 
                       DeleteFunctionType deleteFunction = [](T* val) {
                           delete val;
                       }):
        val_(val),
        ctrlBlock_(new ControlBlock(1, std::move(deleteFunction))) 
    {
    }

    ~shared_ptr() {
        if (val_ == nullptr) {
            return;
        }
        if (--ctrlBlock_->refCount <= 0) {
            ctrlBlock_->deleteFunction(val_);
            delete ctrlBlock_;

            val_ = nullptr;
            ctrlBlock_ = nullptr;
        }
    }

    shared_ptr(const shared_ptr& rhs):
        val_(rhs.val_), 
        ctrlBlock_(rhs.ctrlBlock_) 
    {
        if (ctrlBlock_ != nullptr) {
            ++ctrlBlock_->refCount;
        }
    }

    shared_ptr& operator=(shared_ptr rhs) {
        swap(rhs);
        return *this;
    }

    shared_ptr& operator=(std::nullptr_t rhs) {
        val_(nullptr);
        ctrlBlock_(nullptr);
        return *this;
    }


    void swap(shared_ptr& rhs) {
        using std::swap;
        swap(val_, rhs.val_);
        swap(ctrlBlock_, rhs.ctrlBlock_);
    }

    bool operator==(const shared_ptr& rhs) const {
        return val_ == rhs.val_ && ctrlBlock_ == rhs.ctrlBlock_;
    }

    bool operator==(const std::nullptr_t& rhs) const {
        return val_ == nullptr;
    }

    bool operator!=(const std::nullptr_t& rhs) const {
        return val_ != nullptr;
    }

    // void reset() {
    //     --ctrlBlock_->refCount;
    //     val_ = nullptr;
    //     if (--ctrlBlock_->refCount <= 0) {
    //         ctrlBlock_->deleteFunction(val_);
    //         delete ctrlBlock_;

    //         val_ = nullptr;
    //         ctrlBlock_ = nullptr;
    //     }

    // }


    T* get() const {
        return val_;
    }

    T& operator*() const {
        return *val_;
    }

    T* operator->() const {
        return val_;
    }

    friend void swap(shared_ptr& lhs, shared_ptr& rhs) {
        lhs.swap(rhs);
    }

    operator bool() const {
        return val_ != nullptr;
    }

    std::size_t use_count() const {
        return ctrlBlock_->refCount;
    }

private: 
    struct ControlBlock {
        ControlBlock(int cnt, DeleteFunctionType fnc): 
            refCount(cnt),
            deleteFunction(fnc) 
        {
        }

        int refCount;
        DeleteFunctionType deleteFunction;
    };

    T* val_;
    ControlBlock* ctrlBlock_;
};


template <typename T, typename... Args>
shared_ptr<T> MakeShared(Args&&... args) {
    return shared_ptr<T>(new T(std::forward<Args>(args)...));
}