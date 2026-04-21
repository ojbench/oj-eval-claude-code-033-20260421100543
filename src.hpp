#ifndef SJTU_LIST_HPP
#define SJTU_LIST_HPP

#include <cstddef>
#include <exception>

namespace sjtu {
/**
 * a data container like std::list
 * allocate random memory addresses for data and they are doubly-linked in a
 * list.
 */
template <typename T> class list {
protected:
  class node {
  public:
    T *data;
    node *prev;
    node *next;

    node(const T &value, node *p = nullptr, node *n = nullptr) : prev(p), next(n) {
        data = reinterpret_cast<T*>(operator new(sizeof(T)));
        new (data) T(value);
    }
    node(node *p = nullptr, node *n = nullptr) : data(nullptr), prev(p), next(n) {}
    ~node() {
        if (data) {
            data->~T();
            operator delete(data);
        }
    }
  };

protected:
  node *head;
  node *tail;
  size_t _size;

  /**
   * insert node cur before node pos
   * return the inserted node cur
   */
  node *insert(node *pos, node *cur) {
      cur->prev = pos->prev;
      cur->next = pos;
      pos->prev->next = cur;
      pos->prev = cur;
      _size++;
      return cur;
  }
  /**
   * remove node pos from list (no need to delete the node)
   * return the removed node pos
   */
  node *erase(node *pos) {
      pos->prev->next = pos->next;
      pos->next->prev = pos->prev;
      _size--;
      return pos;
  }

public:
  class const_iterator;
  class iterator {
  private:
    node *ptr;
    const list *owner;

  public:
    iterator(node *p = nullptr, const list *o = nullptr) : ptr(p), owner(o) {}
    iterator(const iterator &other) : ptr(other.ptr), owner(other.owner) {}

    iterator operator++(int) {
        if (!ptr || ptr == owner->tail) throw std::exception();
        iterator tmp = *this;
        ptr = ptr->next;
        return tmp;
    }
    iterator &operator++() {
        if (!ptr || ptr == owner->tail) throw std::exception();
        ptr = ptr->next;
        return *this;
    }
    iterator operator--(int) {
        if (!ptr || ptr->prev == owner->head) throw std::exception();
        iterator tmp = *this;
        ptr = ptr->prev;
        return tmp;
    }
    iterator &operator--() {
        if (!ptr || ptr->prev == owner->head) throw std::exception();
        ptr = ptr->prev;
        return *this;
    }

    /**
     * TODO *it
     * throw std::exception if iterator is invalid
     */
    T &operator*() const {
        if (!ptr || ptr == owner->head || ptr == owner->tail) throw std::exception();
        return *(ptr->data);
    }
    /**
     * TODO it->field
     * throw std::exception if iterator is invalid
     */
    T *operator->() const noexcept {
        if (!ptr || ptr == owner->head || ptr == owner->tail) return nullptr;
        return ptr->data;
    }

    /**
     * a operator to check whether two iterators are same (pointing to the same
     * memory).
     */
    bool operator==(const iterator &rhs) const {
        return ptr == rhs.ptr;
    }
    bool operator==(const const_iterator &rhs) const {
        return ptr == rhs.ptr;
    }

    /**
     * some other operator for iterator.
     */
    bool operator!=(const iterator &rhs) const {
        return ptr != rhs.ptr;
    }
    bool operator!=(const const_iterator &rhs) const {
        return ptr != rhs.ptr;
    }

    friend class list;
    friend class const_iterator;
  };

  /**
   * TODO
   * has same function as iterator, just for a const object.
   * should be able to construct from an iterator.
   */
  class const_iterator {
  private:
    const node *ptr;
    const list *owner;

  public:
    const_iterator(const node *p = nullptr, const list *o = nullptr) : ptr(p), owner(o) {}
    const_iterator(const const_iterator &other) : ptr(other.ptr), owner(other.owner) {}
    const_iterator(const iterator &other) : ptr(other.ptr), owner(other.owner) {}

    const_iterator operator++(int) {
        if (!ptr || ptr == owner->tail) throw std::exception();
        const_iterator tmp = *this;
        ptr = ptr->next;
        return tmp;
    }
    const_iterator &operator++() {
        if (!ptr || ptr == owner->tail) throw std::exception();
        ptr = ptr->next;
        return *this;
    }
    const_iterator operator--(int) {
        if (!ptr || ptr->prev == owner->head) throw std::exception();
        const_iterator tmp = *this;
        ptr = ptr->prev;
        return tmp;
    }
    const_iterator &operator--() {
        if (!ptr || ptr->prev == owner->head) throw std::exception();
        ptr = ptr->prev;
        return *this;
    }

    const T &operator*() const {
        if (!ptr || ptr == owner->head || ptr == owner->tail) throw std::exception();
        return *(ptr->data);
    }
    const T *operator->() const noexcept {
        if (!ptr || ptr == owner->head || ptr == owner->tail) return nullptr;
        return ptr->data;
    }

    bool operator==(const iterator &rhs) const {
        return ptr == rhs.ptr;
    }
    bool operator==(const const_iterator &rhs) const {
        return ptr == rhs.ptr;
    }

    bool operator!=(const iterator &rhs) const {
        return ptr != rhs.ptr;
    }
    bool operator!=(const const_iterator &rhs) const {
        return ptr != rhs.ptr;
    }

    friend class list;
  };

  /**
   * TODO Constructs
   * Atleast two: default constructor, copy constructor
   */
  list() : _size(0) {
      head = new node();
      tail = new node();
      head->next = tail;
      tail->prev = head;
  }
  list(const list &other) : _size(0) {
      head = new node();
      tail = new node();
      head->next = tail;
      tail->prev = head;
      for (const_iterator it = other.cbegin(); it != other.cend(); ++it) {
          push_back(*it);
      }
  }
  /**
   * TODO Destructor
   */
  virtual ~list() {
      clear();
      delete head;
      delete tail;
  }
  /**
   * TODO Assignment operator
   */
  list &operator=(const list &other) {
      if (this == &other) return *this;
      clear();
      for (const_iterator it = other.cbegin(); it != other.cend(); ++it) {
          push_back(*it);
      }
      return *this;
  }
  /**
   * access the first / last element
   * throw container_is_empty when the container is empty.
   */
  const T &front() const {
      if (_size == 0) throw std::exception();
      return *(head->next->data);
  }
  const T &back() const {
      if (_size == 0) throw std::exception();
      return *(tail->prev->data);
  }
  /**
   * returns an iterator to the beginning.
   */
  iterator begin() {
      return iterator(head->next, this);
  }
  const_iterator cbegin() const {
      return const_iterator(head->next, this);
  }
  /**
   * returns an iterator to the end.
   */
  iterator end() {
      return iterator(tail, this);
  }
  const_iterator cend() const {
      return const_iterator(tail, this);
  }
  /**
   * checks whether the container is empty.
   */
  virtual bool empty() const {
      return _size == 0;
  }
  /**
   * returns the number of elements
   */
  virtual size_t size() const {
      return _size;
  }

  /**
   * clears the contents
   */
  virtual void clear() {
      while (_size > 0) {
          pop_back();
      }
  }
  /**
   * insert value before pos (pos may be the end() iterator)
   * return an iterator pointing to the inserted value
   * throw if the iterator is invalid
   */
  virtual iterator insert(iterator pos, const T &value) {
      if (pos.owner != this) throw std::exception();
      node *new_node = new node(value);
      insert(pos.ptr, new_node);
      return iterator(new_node, this);
  }
  /**
   * remove the element at pos (the end() iterator is invalid)
   * returns an iterator pointing to the following element, if pos pointing to
   * the last element, end() will be returned. throw if the container is empty,
   * the iterator is invalid
   */
  virtual iterator erase(iterator pos) {
      if (pos.owner != this || pos.ptr == head || pos.ptr == tail || _size == 0) throw std::exception();
      node *next_node = pos.ptr->next;
      erase(pos.ptr);
      delete pos.ptr;
      return iterator(next_node, this);
  }
  /**
   * adds an element to the end
   */
  void push_back(const T &value) {
      node *new_node = new node(value);
      insert(tail, new_node);
  }
  /**
   * removes the last element
   * throw when the container is empty.
   */
  void pop_back() {
      if (_size == 0) throw std::exception();
      node *p = erase(tail->prev);
      delete p;
  }
  /**
   * inserts an element to the beginning.
   */
  void push_front(const T &value) {
      node *new_node = new node(value);
      insert(head->next, new_node);
  }
  /**
   * removes the first element.
   * throw when the container is empty.
   */
  void pop_front() {
      if (_size == 0) throw std::exception();
      node *p = erase(head->next);
      delete p;
  }
};

} // namespace sjtu

#endif // SJTU_LIST_HPP
