#pragma once
#include <functional>
#include <iostream>
#include <utility>
#include <vector>
#include "thirdparty/plf_colony.h"
#include "boost/smart_ptr/make_local_shared.hpp"

template <typename T> class sparestack {

  public:
    void reserve(std::size_t count) {
        _data.reserve(count);
    }

    // returns the position the item was inserted into

    template <typename U, typename Callable>
    std::size_t push(U&& env, Callable&& callable) {
        if (spareIds.size() == 0) {
            // std::cout << "creating new " << "\n";

            _data.emplace_back(std::forward<U>(env));
            //_data.push_back(env);

            // std::cout << "pushing " << _data.size() << " \n";

            auto existing_size = _data.size() - 1;
            // std::cout << "existing_size " << existing_size << "\n";

            callable(existing_size, _data[existing_size]);

            return existing_size;
        } else {

            std::size_t accessElement = spareIds.back();

            // std::cout << "reusing "
            //           << "\n";

            _data[accessElement] = std::forward<U>(env);
            //_data.emplace(_data.begin()+accessElement, std::forward<U>(env));
            //_data.at(accessElement) = env;

            spareIds.pop_back();
            callable(accessElement, _data[accessElement]);
            return accessElement;
        }
    }

    template <typename Callable> std::size_t retrieve(Callable&& callable) {
        if (spareIds.size() == 0) {

            auto& element = _data.emplace_back();
            //_data.push_back(env);

            std::size_t existing_size = _data.size() - 1;

            callable(existing_size, _data[existing_size]);

            return existing_size;
        } else {

            std::size_t accessElement = spareIds.back();

            spareIds.pop_back();
            callable(accessElement, _data[accessElement]);
            return accessElement;
        }
    }

    std::size_t size() {
        return _data.size() - spareIds.size();
    }

    void eraseAt(std::size_t position) {
        // //if (position < _data.size()) { // if position is within range
        //         if (position == _data.size()-1) { // if UIView id is at the
        //         end exactly
        //             //_data.at(id)->deRegisterChildren();
        //             _data.pop_back();
        //         } else { // item is in middle of data somewhere
        //             spareIds.push_back(position); // add the spare slot to
        //         }
        //     //}
        // std::cout << "erasing "
        //           << "\n";
        spareIds.push_back(position); // add the spare slot to sparestack
    }

    template <typename U> std::size_t push(U&& in) {
        return push(std::forward<U>(in), [](auto a, auto b) { return; });
    }

    T& operator[](std::size_t idx) {
        return _data[idx];
    }

    T& operator[](std::size_t idx) const {
        return _data[idx];
    }

  private:
    std::vector<T> _data;
    std::vector<std::size_t> spareIds; // index of spare slots to fill ids
};

template <typename T> class spare_refcount_stack {

  public:
    void reserve(std::size_t count) {
        _data.reserve(count);
        refCountVec.reserve(count);
    }

    // returns the position the item was inserted into

    template <typename U, typename Callable>
    std::size_t push(U&& env, Callable&& callable) {
        if (spareIds.size() == 0) {
            // std::cout << "creating new " << "\n";

            _data.emplace_back(std::forward<U>(env));
            refCountVec.push_back({0});
            //_data.push_back(env);

            // std::cout << "pushing " << _data.size() << " \n";

            auto existing_size = _data.size() - 1;
            // std::cout << "existing_size " << existing_size << "\n";

            callable(existing_size, _data[existing_size]);

            return existing_size;
        } else {

            std::size_t accessElement = spareIds.back();

            // std::cout << "reusing "
            //           << "\n";

            _data[accessElement] = std::forward<U>(env);
            refCountVec[accessElement] = 0;
            //_data.emplace(_data.begin()+accessElement, std::forward<U>(env));
            //_data.at(accessElement) = env;

            spareIds.pop_back();
            callable(accessElement, _data[accessElement]);
            return accessElement;
        }
    }


    template <typename U>
    inline std::size_t push_in(U&& env) {
        if (spareIds.size() == 0) {
            // std::cout << "creating new " << "\n";

            _data.emplace_back(std::forward<U>(env));
            refCountVec.push_back({0});
            //_data.push_back(env);

            // std::cout << "pushing " << _data.size() << " \n";

            auto existing_size = _data.size() - 1;
            // std::cout << "existing_size " << existing_size << "\n";

            return existing_size;
        } else {

            std::size_t accessElement = spareIds.back();

            // std::cout << "reusing "
            //           << "\n";

            _data[accessElement] = std::forward<U>(env);
            refCountVec[accessElement] = 0;
            //_data.emplace(_data.begin()+accessElement, std::forward<U>(env));
            //_data.at(accessElement) = env;

            spareIds.pop_back();
            return accessElement;
        }
    }

    template <typename Callable> std::size_t retrieve(Callable&& callable) {
        if (spareIds.size() == 0) {

            auto& element = _data.emplace_back();
            refCountVec.push_back({0});

            //_data.push_back(env);

            std::size_t existing_size = _data.size() - 1;

            callable(existing_size, _data[existing_size]);

            return existing_size;
        } else {

            std::size_t accessElement = spareIds.back();
            refCountVec[accessElement] = 0;

            spareIds.pop_back();
            callable(accessElement, _data[accessElement]);
            return accessElement;
        }
    }

    std::size_t size() {
        return _data.size() - spareIds.size();
    }

    void eraseAt(std::size_t position) {
        // //if (position < _data.size()) { // if position is within range
        //         if (position == _data.size()-1) { // if UIView id is at the
        //         end exactly
        //             //_data.at(id)->deRegisterChildren();
        //             _data.pop_back();
        //         } else { // item is in middle of data somewhere
        //             spareIds.push_back(position); // add the spare slot to
        //         }
        //     //}
        // std::cout << "erasing "
        //           << "\n";
        spareIds.push_back(position); // add the spare slot to sparestack
        refCountVec[position] = 0;
    }

    template <typename U> std::size_t push(U&& in) {
        return push(std::forward<U>(in), [](auto a, auto b) { return; });
    }

    T& operator[](std::size_t idx) {
        return _data[idx];
    }

    T& operator[](std::size_t idx) const {
        return _data[idx];
    }

    inline std::vector<std::size_t>& refCounts() {
        return refCountVec;
    }

  private:
    std::vector<T> _data;
    std::vector<std::size_t> refCountVec;

    std::vector<std::size_t> spareIds; // index of spare slots to fill ids
};

template <typename T> class uniquestack {

  public:
    void reserve(std::size_t count) {
        _data.reserve(count);
    }

    // returns the position the item was inserted into

    template <typename Callable>
    std::size_t push(T&& env, Callable&& callable) {
        if (spareIds.size() == 0) {
            // std::cout << "creating new "
            //           << "\n";

            _data.emplace_back(std::forward<T>(env));
            //_data.push_back(env);

            std::size_t existing_size = _data.size() - 1;
            // std::cout << "existing_size " << existing_size << "\n";

            callable(existing_size, _data[existing_size]);

            return existing_size;
        } else {

            std::size_t accessElement = spareIds.back();

            _data[accessElement].reset(env);
            //_data.at(accessElement) = env;

            spareIds.pop_back();
            callable(accessElement, _data[accessElement]);
            return accessElement;
        }
    }

    template <typename Callable> std::size_t retrieve(Callable&& callable) {
        if (spareIds.size() == 0) {

            _data.emplace_back(std::make_unique<typename T::element_type>());
            // _data.push_back(
            //     boost::make_local_shared_noinit<typename T::element_type>());
            //_data.push_back(env);

            std::size_t existing_size = _data.size() - 1;

            callable(existing_size, _data[existing_size]);

            return existing_size;
        } else {

            std::size_t accessElement = spareIds.back();

            spareIds.pop_back();
            callable(accessElement, _data[accessElement]);
            return accessElement;
        }
    }

    std::size_t size() {
        return _data.size() - spareIds.size();
    }

    void eraseAt(std::size_t position) {
        // // if (position < _data.size()) { // if position is within range
        // if (position ==
        //     _data.size() - 1) { // if UIView id is at the end exactly
        //     //_data.at(id)->deRegisterChildren();
        //     _data.pop_back();
        // } else {                          // item is in middle of data
        // somewhere
        //     spareIds.push_back(position); // add the spare slot to sparestack
        // }
        // // }
        spareIds.push_back(position); // add the spare slot to sparestack
    }

    // template<typename U>
    // std::size_t push(U&& in)
    // {
    //     return push(std::forward<U>(in), [](auto a, auto b) { return; });
    // }

    T& operator[](std::size_t idx) {
        return _data[idx];
    }

    T& operator[](std::size_t idx) const {
        return _data[idx];
    }

  private:
    std::vector<T> _data;
    std::vector<std::size_t> spareIds; // index of spare slots to fill ids
};


template <typename T> class stablestack {

  public:
    void reserve(std::size_t count) {
        _data.reserve(count);
    }

    using Iter = plf::colony<T>::iterator;

    template <typename Callable> Iter retrieve(Callable&& callable) {
        if (spareIts.size() == 0) {

            auto it = _data.insert(T{});
            // _data.push_back(
            //     boost::make_local_shared_noinit<typename T::element_type>());
            //_data.push_back(env);
            //std::cout << "pb\n";
            callable(*it);

            return it;
        } else {
            //std::cout << "rt\n";

            auto accessIt = spareIts.back();

            spareIts.pop_back();
            callable(*accessIt);
            return accessIt;
        }
    }

    std::size_t size() {
        return _data.size() - spareIts.size();
    }

    void eraseAt(Iter it) {
                   // std::cout << "er\n";

        spareIts.push_back(it); // add the spare slot to sparestack
    }

    T& operator[](std::size_t idx) {
        return _data[idx];
    }

    T& operator[](std::size_t idx) const {
        return _data[idx];
    }

  private:
    plf::colony<T> _data;
    std::vector<Iter> spareIts; // index of spare slots to fill ids
};