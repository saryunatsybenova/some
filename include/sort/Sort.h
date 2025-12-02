//
// Created by sphdx on 6/21/25.
//

#ifndef SORT_H
#define SORT_H

#include <iosfwd>
#include <iostream>
#include <random>


namespace sort {
    enum class Order {
        ASC,
        DESC
    };

    template<typename T>
    void insertionSort(T *arr, const int arraySize, const Order order = Order::ASC) {
        for (int i = 1; i < arraySize; ++i) {
            const T key = arr[i];
            int j = i - 1;

            if (order == Order::ASC) {
                while (j >= 0 && arr[j] > key) {
                    arr[j + 1] = arr[j];
                    --j;
                }
            } else {
                while (j >= 0 && arr[j] < key) {
                    arr[j + 1] = arr[j];
                    --j;
                }
            }
            arr[j + 1] = key;
        }
    }

    // increment generates Sedjvick dist for shell sort
    inline int increment(long long inc[], const long size) {
        int p1 = 1, p2 = 1, p3 = 1;
        int s = -1;

        do {
            if (++s % 2) {
                inc[s] = 8 * p1 - 6 * p2 + 1;
            } else {
                inc[s] = 9 * p1 - 9 * p3 + 1;
                p2 *= 2;
                p3 *= 2;
            }
            p1 *= 2;
        } while (3 * inc[s] < size);

        return s > 0 ? --s : 0;
    }

    template<typename T>
    void shellSort(T *arr, const int arraySize, const Order order = Order::ASC) {
        long j;
        long long seq[30];

        int s = increment(seq, arraySize);

        while (s >= 0) {
            const long inc = seq[s--];

            for (long i = inc; i < arraySize; i++) {
                T temp = arr[i];

                if (order == Order::ASC) {
                    for (j = i - inc; j >= 0 && arr[j] > temp; j -= inc) {
                        arr[j + inc] = arr[j];
                    }
                } else {
                    for (j = i - inc; j >= 0 && arr[j] < temp; j -= inc) {
                        arr[j + inc] = arr[j];
                    }
                }

                arr[j + inc] = temp;
            }
        }
    }


    namespace test {
        struct testStruct {
            int key = 0, id = 0;

            bool operator<(const testStruct &other) const {
                return key < other.key;
            }

            bool operator>(const testStruct &other) const {
                return key > other.key;
            }

            bool operator==(const testStruct &other) const {
                return key == other.key;
            }

            bool operator!() const {
                return !key;
            }

            friend std::ostream &operator<<(std::ostream &os, const testStruct &ts) {
                os << ts.key << ' ' << ts.id;
                return os;
            }
        };

        constexpr int testArraySize = 1000;

        // isSortStable checks the stability of the sorting. it creates an array and checks the order
        // of the marked elements. if the marked elements have changed the order, the sorting is unstable,
        // otherwise it is stable. for a detailed description of the work, you must pass debug = true.
        inline void isSortStableCout(void sort(testStruct *arr, int arraySize, Order),
                                     const Order order = Order::ASC, const bool debug = false) {
            auto fillArray = [](testStruct *array, const int size) {
                std::random_device rd;
                std::mt19937 gen(rd());
                std::uniform_int_distribution dist(0, 2000);

                for (int i = 0; i < size; ++i) {
                    array[i].key = dist(gen);
                    if (array[i].key == 500) array[i].key = 12;
                    array[i].id = i;
                }

                for (int i = 0; i < 10; ++i) {
                    array[i].key = 500;
                }
            };

            auto testArray = new testStruct[testArraySize]{};
            fillArray(testArray, testArraySize);

            sort(testArray, testArraySize, order);

            auto isStable = [](const testStruct *array, const int size, const bool isDebug) {
                for (int i = 0; i < size; i++) {
                    if (array[i].key == 500) {
                        if (isDebug) std::cout << "Found the marked element. Start checking ids.." << std::endl;
                        for (int j = i; j < i + 9; j++) {
                            if (isDebug)
                                std::cout << "Marked element [j]: " << array[j].id <<
                                        " and Marked element [j+1]: " << array[j + 1].id << std::endl;
                            if (array[j].id > array[j + 1].id) {
                                std::cout << "Found difference: " << array[j].id << " and " << array[j + 1].id <<
                                        std::endl;
                                return false;
                            }
                        }
                        break;
                    }
                }
                return true;
            };

            const std::string output = isStable(testArray, testArraySize, debug)
                                           ? "Sort is stable"
                                           : "Sort is not stable";
            std::cout << output << std::endl;

            delete[] testArray;
        }
    }
}

#endif //SORT_H
