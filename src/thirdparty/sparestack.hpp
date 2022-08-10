#pragma once
#include <functional>
#include <utility>
#include <vector>

template <typename T>
class sparestack {

public:

    void reserve(std::size_t count) {
        _data.reserve(count);
    }

    // returns the position the item was inserted into

    template <typename Callable>
    std::size_t push(T&& env, Callable&& callable)
    {
        if (spareIds.size() == 0) {
            std::cout << "creating new " << "\n";

            _data.emplace_back(std::forward<T>(env));
            //_data.push_back(env);

            auto existing_size = _data.size()-1;
            std::cout << "existing_size " << existing_size << "\n";

            callable(existing_size, _data[existing_size]);

            return existing_size;
        } else {

            int accessElement = spareIds.back();


            _data.at(accessElement) = env;
            //_data.at(accessElement) = env;

            spareIds.pop_back();
            callable(accessElement, _data[accessElement]);
            return accessElement - 1;
        }
    }

    template <typename Callable> 
    std::size_t retrieve(Callable&& callable) {
        if (spareIds.size() == 0) {

            auto& element = _data.emplace_back();
            //_data.push_back(env);

            auto existing_size = _data.size() - 1;

            callable(existing_size, _data[existing_size]);

            return existing_size;
        } else {

            int accessElement = spareIds.back();

            spareIds.pop_back();
            callable(accessElement, _data[accessElement]);
            return accessElement;
        }
    }

    std::size_t size()
    {
        return _data.size() - spareIds.size();
    }

    void eraseAt(std::size_t position)
    {
        if (position < _data.size()) { // if position is within range
            if (position == _data.size()) { // if UIView id is at the end exactly
                //_data.at(id)->deRegisterChildren();
                _data.pop_back();
            } else { // item is in middle of data somewhere
                spareIds.push_back(position); // add the spare slot to sparestack
            }
        }
    }

    // template<typename U>
    // std::size_t push(U&& in)
    // {
    //     return push(std::forward<U>(in), [](auto a, auto b) { return; });
    // }

    T& operator[](int idx)
    {
        return _data[idx];
    }

    T operator[](int idx) const
    {
        return _data[idx];
    }

private:
    std::vector<T> _data;
    std::vector<int> spareIds; // index of spare slots to fill ids
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
            std::cout << "creating new "
                      << "\n";

            _data.emplace_back(std::forward<T>(env));
            //_data.push_back(env);

            auto existing_size = _data.size() - 1;
            std::cout << "existing_size " << existing_size << "\n";

            callable(existing_size, _data[existing_size]);

            return existing_size;
        } else {

            int accessElement = spareIds.back();

            _data.at(accessElement) = env;
            //_data.at(accessElement) = env;

            spareIds.pop_back();
            callable(accessElement, _data[accessElement]);
            return accessElement - 1;
        }
    }

    template <typename Callable> std::size_t retrieve(Callable&& callable) {
        if (spareIds.size() == 0) {

            _data.push_back(std::make_unique<typename T::element_type>());
            //_data.push_back(env);

            auto existing_size = _data.size() - 1;

            callable(existing_size, _data[existing_size]);

            return existing_size;
        } else {

            int accessElement = spareIds.back();

            spareIds.pop_back();
            callable(accessElement, _data[accessElement]);
            return accessElement;
        }
    }

    std::size_t size() {
        return _data.size() - spareIds.size();
    }

    void eraseAt(std::size_t position) {
        if (position < _data.size()) { // if position is within range
            if (position ==
                _data.size()) { // if UIView id is at the end exactly
                //_data.at(id)->deRegisterChildren();
                _data.pop_back();
            } else { // item is in middle of data somewhere
                spareIds.push_back(
                    position); // add the spare slot to sparestack
            }
        }
    }

    // template<typename U>
    // std::size_t push(U&& in)
    // {
    //     return push(std::forward<U>(in), [](auto a, auto b) { return; });
    // }

    T& operator[](int idx) {
        return _data[idx];
    }

    T operator[](int idx) const {
        return _data[idx];
    }

  private:
    std::vector<T> _data;
    std::vector<int> spareIds; // index of spare slots to fill ids
};


