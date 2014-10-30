#ifndef _TIM_SORT_TESTS
#define _TIM_SORT_TESTS

#include <cstdlib>
#include <cstdio>
#include <algorithm>
#include <vector>
#include <string>


class Point
{
public:
    long long x, y;
    
    Point(){}
    
    template<typename T>
    Point(T x, T y) : x(x), y(y)
    {
    }
    
    bool isUp() const
    {
        return (y > 0 || (y == 0 && x >= 0));
    }
    
    long long operator*(const Point &a) const
    {
        return (x * a.y - y * a.x);
    }
    
    long long sqLen() const
    {
        return (x * x + y * y);
    }
};

class pointSortFunction
{
public:
    bool operator()(const Point &a, const Point &b) const
    {
        if (a.isUp() && !b.isUp())
            return true;
        if (!a.isUp() && b.isUp())
            return false;
        if ((a * b) == 0)
        return a.sqLen() < b.sqLen();
        return (a * b) > 0;
    }
};

namespace timsort_rand
{
    ///Implemented my own rand to guarantee, that equal tests will be generated on different compilers
    unsigned int seed = 1951u;

    void srand(unsigned int seed)
    {
        timsort_rand::seed = seed;
    }

    const unsigned int firstRandCoeff = 211u;
    const unsigned int secondRandCoeff = 1481u;
    const unsigned int modulo = 4294967291u;

    unsigned int rand()
    {
        static unsigned int x = seed;
        return (x = 1ll * firstRandCoeff * x + secondRandCoeff) % modulo;
    }
    

    int generateInt()
    {
        return rand();
    }
    
    unsigned int generateUnsignedInt()
    {
        return rand();
    }

    char generateChar()
    {
        static const unsigned int ALPHABET_SIZE = 26;
        static const char START_SYMBOL = 'a';
        return (rand() % ALPHABET_SIZE) + START_SYMBOL;
    }
    
    
    template<class ElementType>
    ElementType generateElement(unsigned int additionalParameter=0)
    {
    }
    
    template<> 
    int generateElement<int>(unsigned int additionalParameter)
    {
        return generateInt();
    }
    
    template<> 
    std::pair<unsigned int, int> generateElement<std::pair<unsigned int, int> >(unsigned int additionalParameter)
    {
        static const int FIRST_ELEMENT_MAX_VALUE = 10; ///made to check is sort is stable
        unsigned int first = generateUnsignedInt() % FIRST_ELEMENT_MAX_VALUE;
        int second = generateInt();
        return std::pair<unsigned int, int> (first, second);
    }
    
    template<>
    std::string generateElement<std::string>(unsigned int length)
    {
        std::string result(length, '\0');
        for (unsigned int i = 0; i < length; ++i)
        {
            result[i] = generateChar();
        }
        return result;
    }
    
    template<>
    Point generateElement<Point>(unsigned int additionalParameter)
    {
        return Point(generateInt(), generateInt());
    }
    
    template<class ElementType> 
    std::vector <ElementType> generateRandomArray(unsigned int length, unsigned int additionalParameter=0)
    {
        std::vector <ElementType> result(length);
        for (unsigned int i = 0; i < length; ++i)
        {
            result[i] = generateElement<ElementType>(additionalParameter);
        }
        return result;
    }
    
    template<class ElementType, class Compare=std::less<ElementType> >
    std::vector <ElementType> generatePartlySortedArray(unsigned int lengthOfEach, unsigned int numberOfParts, unsigned int additionalParameter=0, Compare comp = Compare())
    {
        std::vector <ElementType> result(lengthOfEach * numberOfParts);
        for (unsigned int i = 0; i < numberOfParts; ++i)
        {
            std::vector <ElementType> temporaryArray = generateRandomArray<ElementType>(lengthOfEach, additionalParameter);
            std::stable_sort(temporaryArray.begin(), temporaryArray.end(), comp);
            std::copy(temporaryArray.begin(), temporaryArray.end(), result.begin() + i * lengthOfEach);
        }
        return result;
    }
};
#endif