//
// Created by sphdx on 5/17/25.
//

#ifndef PAIR_H
#define PAIR_H

namespace hash::detail {
    template<typename Key, typename Val>
    class Pair {
        Key *key_ = nullptr;
        Val *val_ = nullptr;

    public:
        Pair(const Key &key, const Val &val) : key_(new Key(key)), val_(new Val(val)) {
        }

        ~Pair() {
            delete key_;
            delete val_;
        }

        Key *key() const { return key_; }
        Val *val() const { return val_; }
    };
}

#endif //PAIR_H
