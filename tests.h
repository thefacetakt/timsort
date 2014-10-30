#ifndef _TIM_SORT_TESTS
#define _TIM_SORT_TESTS

#include <cstdlib>
#include <cstdio>
#include <algorithm>
#include <vector>
#include <string>
#include <typeinfo>

namespace timSortTestClasses
{
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

    class PointComparator
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
};


namespace timsortRand
{
    ///Implemented my own rand to guarantee, that equal tests will be generated on different compilers
    unsigned int seed = 1951u;

    void srand(unsigned int seed)
    {
        timsortRand::seed = seed;
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
    class GenerateElement;
    
    template<>
    class GenerateElement<int>
    {    
    public:
        GenerateElement(int additionalParameter=0) 
        {
        }
        
        int operator()() const
        {
            return generateInt();
        }
    };
    
    template<>
    class GenerateElement<std::pair<unsigned int, int>>
    {    
    public:
        GenerateElement(int additionalParameter=0) 
        {
        }
        
        std::pair<unsigned int, int> operator()() const
        {
            static const int FIRST_ELEMENT_MAX_VALUE = 10; ///made to check is sort is stable
            unsigned int first = generateUnsignedInt() % FIRST_ELEMENT_MAX_VALUE;
            int second = generateInt();
            return std::pair<unsigned int, int> (first, second);
        }
    };
    
    template<>
    class GenerateElement<std::string>
    {
        int length;
        
    public:
        GenerateElement()
        {
            throw "No length provided\n";
        }
        
        GenerateElement(int length) : length(length)
        {
        }
        
        std::string operator()() const
        {
            std::string result(length, '\0');
            std::generate(result.begin(), result.end(), generateChar);
            return result;
        }
    };
    
    template<>
    class GenerateElement<timSortTestClasses::Point>
    {    
    public:
        GenerateElement(int additionalParameter=0) 
        {
        }
        
        timSortTestClasses::Point operator()() const
        {
            return timSortTestClasses::Point(generateInt(), generateInt());
        }
    };
    
    
    
    template<class ElementType, class Compare=std::less<ElementType> >
    std::vector <ElementType> generatePartlySortedArray(unsigned int lengthOfEach, unsigned int numberOfParts, unsigned int additionalParameter=0, Compare comp = Compare())
    {
        std::vector <ElementType> result(lengthOfEach * numberOfParts);
        
        for (unsigned int i = 0; i < numberOfParts; ++i)
        {
            std::vector <ElementType> temporaryArray(lengthOfEach);
            std::generate(temporaryArray.begin(), temporaryArray.end(), GenerateElement<ElementType>(additionalParameter));
            std::stable_sort(temporaryArray.begin(), temporaryArray.end(), comp);
            std::copy(temporaryArray.begin(), temporaryArray.end(), result.begin() + i * lengthOfEach);
        }
        
        return result;
    }
};

#endif