// вставьте сюда ваш код для класса SimpleVector
// внесите необходимые изменения для поддержки move-семантики

#pragma once

#include <cassert>
#include <initializer_list>
#include <algorithm>
#include <stdexcept>
#include "array_ptr.h"



class ReserveProxyObj {
public:
    explicit ReserveProxyObj(size_t cap_to_res) :capacity_(cap_to_res) {
    }
    size_t GetCapacity() {
        return capacity_;
    }
private:
    size_t capacity_;
};



template <typename Type>
class SimpleVector {
public:
    using Iterator = Type*;
    using ConstIterator = const Type*;

    SimpleVector() noexcept = default;

    // Создаёт вектор из size элементов, инициализированных значением по умолчанию
    explicit SimpleVector(size_t size) :
        items_(size),
        size_(size),
        capacity_(size_) {
        std::generate(begin(), end(), [] {return Type(); });
    }

    // Создаёт вектор из size элементов, инициализированных значением value
    SimpleVector(size_t size, const Type& value) :
        items_(size),
        size_(size),
        capacity_(size) {
        std::fill(begin(), end(), value);
    }

    // Создаёт вектор из std::initializer_list
    SimpleVector(std::initializer_list<Type> init) :
        items_(init.size()),
        size_(init.size()),
        capacity_(init.size()) {
        std::copy(init.begin(), init.end(), begin());
    }

    // конструктор копирования
    SimpleVector(const SimpleVector& other) {
        ArrayPtr<Type> tmp(other.GetSize());
        std::copy(other.begin(), other.end(), tmp.Get());
        capacity_ = other.capacity_;
        size_ = other.size_;
        items_.swap(tmp);
    }

    SimpleVector(SimpleVector&& other) :
        items_(std::move(other.items_)) {
        size_ = std::exchange(other.size_, 0);
        capacity_ = std::exchange(other.capacity_, 0);
    }

    SimpleVector(ReserveProxyObj other) {
        Reserve(other.GetCapacity());
    }

    // Возвращает количество элементов в массиве
    size_t GetSize() const noexcept {
        return size_;
    }

    // Возвращает вместимость массива
    size_t GetCapacity() const noexcept {
        return capacity_;
    }

    // Сообщает, пустой ли массив
    bool IsEmpty() const noexcept {
        return (size_ == 0);
    }

    // Возвращает ссылку на элемент с индексом index
    Type& operator[](size_t index) noexcept {
        return items_[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    const Type& operator[](size_t index) const noexcept {
        return items_[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    // Выбрасывает исключение std::out_of_range, если index >= size
    Type& At(size_t index) {
        // Напишите тело самостоятельно
        if (index >= size_) {
            throw std::out_of_range("index is out of range");
        }
        return items_[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    // Выбрасывает исключение std::out_of_range, если index >= size
    const Type& At(size_t index) const {
        // Напишите тело самостоятельно
        if (index >= size_) {
            throw std::out_of_range("index is out of range");
        }
        return items_[index];
    }

    // Обнуляет размер массива, не изменяя его вместимость
    void Clear() noexcept {
        size_ = 0;
    }

    // Изменяет размер массива.
    // При увеличении размера новые элементы получают значение по умолчанию для типа Type
    void Resize(size_t new_size) {
        // Напишите тело самостоятельно
        if (new_size < size_) {
            size_ = new_size;
            return;
        }
        else if (new_size < capacity_ && size_ < new_size) {
            std::generate(end(), begin() + new_size, []() {return Type(); });
            size_ = new_size;
        }
        else if (new_size > capacity_) {
            new_size = std::max(new_size, capacity_ * 2);
            ArrayPtr<Type> tmp(new_size);
            std::generate(&tmp[0], &tmp[new_size], []() {return Type(); });
            std::move(begin(), end(), &tmp[0]);
            items_.swap(tmp);
            size_ = new_size;
            capacity_ = new_size;
        }
    }

    // Возвращает итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    Iterator begin() noexcept {
        return Iterator{ &items_[0] };
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    Iterator end() noexcept {
        return Iterator{ &items_[size_] };
    }

    // Возвращает константный итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator begin() const noexcept {
        return cbegin();
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator end() const noexcept {
        return cend();
    }

    // Возвращает константный итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator cbegin() const noexcept {
        return ConstIterator{ &items_[0] };
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator cend() const noexcept {
        return ConstIterator{ &items_[size_] };
    }

    SimpleVector& operator=(const SimpleVector& rhs) {
        if (this != &rhs) {
            SimpleVector tmp(rhs);
            swap(tmp);
        }
        return *this;
    }

    SimpleVector& operator=(const SimpleVector&& rhs) {
        ArrayPtr<Type> newptr(rhs.size_);
        std::move(rhs.begin(), rhs.end(), newptr.Get());
        items_.swap(newptr);
        size_ = std::move(rhs.size_);
        capacity_ = size_;
        return *this;
    }

    // Добавляет элемент в конец вектора
    // При нехватке места увеличивает вдвое вместимость вектора
    void PushBack(const Type& item) {
        if (size_ < capacity_) {
            items_[size_] = item;
            ++size_;
            return;
        }
        else {
            size_t new_capacity = (size_ == 0) ? 1 : capacity_ * 2;
            ArrayPtr<Type> tmp(new_capacity);
            std::copy(begin(), end(), tmp.Get());
            tmp[size_] = item;
            items_.swap(tmp);
            ++size_;
            capacity_ = new_capacity;
            return;
        }
    }

    void PushBack(Type&& item) {
        if (size_ < capacity_) {
            items_[size_] = std::move(item);
            ++size_;
            return;
        }
        else {
            size_t new_capacity = (size_ == 0) ? 1 : capacity_ * 2;
            ArrayPtr<Type> tmp(new_capacity);
            std::move(begin(), end(), tmp.Get());
            tmp[size_] = std::move(item);
            items_.swap(tmp);
            ++size_;
            capacity_ = new_capacity;
            return;
        }
    }

    // Вставляет значение value в позицию pos.
    // Возвращает итератор на вставленное значение
    // Если перед вставкой значения вектор был заполнен полностью,
    // вместимость вектора должна увеличиться вдвое, а для вектора вместимостью 0 стать равной 1
    Iterator Insert(ConstIterator pos, const Type& value) {
        assert(pos >= begin() && pos <= end());
        Iterator poz = const_cast<Iterator>(pos);
        auto index = std::distance(cbegin(), pos);
        if (size_ < capacity_) {
            std::copy_backward(poz, items_.Get() + size_, items_.Get() + size_ + 1);
            *poz = value;
        }
        else {
            size_t new_capacity = (size_ == 0) ? 1 : capacity_ * 2;
            ArrayPtr<Type> tmp(new_capacity);
            Iterator last_copy = std::copy(begin(), poz, tmp.Get());
            *last_copy = value;
            std::copy(poz, end(), std::next(last_copy));
            items_.swap(tmp);
            capacity_ = new_capacity;
        }
        ++size_;
        return &items_[index];
    }

    Iterator Insert(ConstIterator pos, Type&& value) {
        assert(pos >= begin() && pos <= end());
        Iterator poz = const_cast<Iterator>(pos);
        auto index = std::distance(cbegin(), pos);
        if (size_ < capacity_) {

            std::move_backward(poz, items_.Get() + size_, items_.Get() + size_ + 1);
            *poz = std::move(value);
        }
        else {
            size_t new_capacity = (size_ == 0) ? 1 : capacity_ * 2;
            ArrayPtr<Type> tmp(new_capacity);
            Iterator last_moved = std::move(begin(), poz, tmp.Get());
            *last_moved = std::move(value);
            std::move(poz, end(), std::next(last_moved));
            items_.swap(tmp);
            capacity_ = new_capacity;
        }
        ++size_;
        return &items_[index];
    }

    // "Удаляет" последний элемент вектора. Вектор не должен быть пустым
    void PopBack() noexcept {
        // Напишите тело самостоятельно
        if (size_) {
            --size_;
        }
    }

    // Удаляет элемент вектора в указанной позиции
    Iterator Erase(ConstIterator pos) {
        assert(pos >= begin() && pos <= end());
        Iterator poz = const_cast<Iterator>(pos);
        std::move(std::next(Iterator(pos)), end(), Iterator(pos));
        --size_;
        return poz;
    }

    // Обменивает значение с другим вектором
    void swap(SimpleVector& other) noexcept {
        items_.swap(other.items_);
        std::swap(size_, other.size_);
        std::swap(capacity_, other.capacity_);
    }

    void Reserve(size_t new_capacity) {
        if (new_capacity < capacity_) {
            return;
        }
        ArrayPtr<Type> tmp(new_capacity);
        std::copy(begin(), end(), tmp.Get());
        std::move(begin(), end(), tmp.Get());
        items_.swap(tmp);
        capacity_ = new_capacity;
    }

private:
    ArrayPtr<Type> items_;
    size_t size_ = 0;
    size_t capacity_ = 0;
};

ReserveProxyObj Reserve(size_t capacity_to_reserve) {
    return ReserveProxyObj(capacity_to_reserve);
}


template <typename Type>
inline bool operator==(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
inline bool operator!=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(lhs == rhs);
}

template <typename Type>
inline bool operator<(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
inline bool operator<=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(rhs < lhs);
}

template <typename Type>
inline bool operator>(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return (rhs < lhs);
}

template <typename Type>
inline bool operator>=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(rhs > lhs);
}