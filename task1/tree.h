#pragma once

#include <memory>

namespace bintree {
    template <typename T>

    // enable_shared_from_this позволяет избежать создания
    // shared_ptr из this, чтобы не создавать дублирующих
    // shared_ptr.
    struct TNode : public std::enable_shared_from_this<TNode<T>> {
        using TNodePtr = std::shared_ptr<TNode<T>>;
        using TNodeWeakPtr = std::weak_ptr<TNode<T>>;
        using TNodeConstPtr = std::shared_ptr<const TNode<T>>;

        bool hasLeft() const {
            return bool(left);
        }

        bool hasRight() const {
            return bool(right);
        }

        bool hasParent() const {
            return bool(parent);
        }

        T& getValue() {
            return value;
        }

        const T& getValue() const {
            return value;
        }

        TNodePtr getLeft() {
            return left;
        }

        TNodeConstPtr getLeft() const {
            return left;
        }

        TNodePtr getRight() {
            return right;
        }

        TNodeConstPtr getRight() const {
            return right;
        }

        TNodePtr getParent() {
            return parent.lock();
        }

        TNodeConstPtr getParent() const {
            return parent.lock();
        }

        static TNodePtr createLeaf(T v) {
            struct TConcreteNode : public TNode {
                TConcreteNode(T v) : TNode(v) {}
            }; // с помощью этой вспомогательной структуры мы можем создавать
               // объект класса с приватным конструктором в make_shared

            return std::make_shared<TConcreteNode>(v);
        }

        static TNodePtr fork(T v, TNodePtr left, TNodePtr right) {
            struct TConcreteNode : public TNode {
                TConcreteNode(T v, TNodePtr left, TNodePtr right) : TNode(v, left, right) {}
            }; // с помощью этой вспомогательной структуры мы можем создавать
               // объект класса с приватным конструктором в make_shared

            TNodePtr ptr = std::make_shared<TConcreteNode>(v, left, right);
            setParent(ptr->getLeft(), ptr);
            setParent(ptr->getRight(), ptr);
            return ptr;
        }

        TNodePtr replaceLeft(TNodePtr l) {
            // shared_from_this() не создаёт новый shared_ptr на объект,
            // а использует уже существующий, так избегается повторное создание указателя.
            setParent(l, this->shared_from_this());
            setParent(left, nullptr);
            std::swap(l, left);
            return l;
        }

        TNodePtr replaceRight(TNodePtr r) {
            // shared_from_this() не создаёт новый shared_ptr на объект,
            // а использует уже существующий, так избегается повторное создание указателя.
            setParent(r, this->shared_from_this());
            setParent(right, nullptr);
            std::swap(r, right);
            return r;
        }

        TNodePtr replaceRightWithLeaf(T v) {
            return replaceRight(createLeaf(v));
        }

        TNodePtr replaceLeftWithLeaf(T v) {
            return replaceLeft(createLeaf(v));
        }

        TNodePtr removeLeft() {
            return replaceLeft(nullptr);
        }
        TNodePtr removeRight() {
            return replaceRight(nullptr);
        }

    private:
        T value;

        TNodePtr left = nullptr;
        TNodePtr right = nullptr;
        // weak_ptr позволяет избавиться от циклической зависимости
        TNodeWeakPtr parent;

        TNode(T v)
            : value(v)
        {
        }

        // использование здесь shared_ptr вместо сырых указателей
        // позволяет избежать создания дубликатов shared_ptr, а также
        // упрощает создание объектов (не нужно вызывать метод get())
        TNode(T v, TNodePtr left, TNodePtr right)
            : value(v)
            , left(left)
            , right(right)
        {
        }

        static void setParent(TNodePtr node, TNodePtr parent) {
            if (node)
                node->parent = parent;
        }
    };
}
