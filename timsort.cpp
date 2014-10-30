#include <ctime>
#include <cstdio>
#include <vector>
#include <iostream>
#include <algorithm>
#include "timsort.h"
#include "tests.h"


class TimSortParametersTwo : public TimSortParametersDefault
{
public:
    static TimSortParametersTwo TimSortParametersTwoObject;
    MergeActionType getMergeAction(unsigned int sizeOfX, unsigned int sizeOfY, unsigned int sizeOfZ) const
    {
        if (sizeOfY <= sizeOfX)
        {
            return MERGE_YX;
        }
        else
        {
            return MERGE_NOTHING;
        }
    }
    MergeActionType getMergeAction(unsigned int sizeOfX, unsigned int sizeOfY) const
    {
        if (sizeOfY <= sizeOfX)
        {
            return MERGE_YX;
        }
        else
        {
            return MERGE_NOTHING;
        }
    }
};

TimSortParametersTwo TimSortParametersTwo::TimSortParametersTwoObject = TimSortParametersTwo();

class SpecialStringComparator
{
public:
    bool operator()(const std::string &first, const std::string &second) const
    {
        if (first.size() == second.size())
        {
            return first < second;
        }
        return first.size() < second.size();
    }
};

class SpecialPairComparator
{
public:
    template<class FirstType, class SecondType>
    bool operator()(const std::pair<FirstType, SecondType> &first, const std::pair<FirstType, SecondType> &second) const
    {
        return first.first < second.first;
    }
};


template<class ElementsType, class Compare = std::less<ElementsType> >
bool areEqual(const std::vector<ElementsType> &a, const std::vector<ElementsType> &b, Compare comp = Compare())
{
    if (a.size() != b.size())
    {
        return false;
    }
    for (size_t i = 0; i < a.size(); ++i)
    {
        if (comp(a[i], b[i]) || comp(b[i], a[i]))
            return false;
    }
    return true;
}

template <class ElementsType, class Compare = std::less<ElementsType> >
void test(std::vector<ElementsType> arrayToSort, unsigned int numberOfTest, Compare comp = Compare())
{
    std::vector<ElementsType> arrayToSortCopy = arrayToSort;
    clock_t stdSortBegin = clock();
    std::stable_sort(arrayToSortCopy.begin(), arrayToSortCopy.end(), comp);
    clock_t stdSortEnd = clock();
    
    clock_t timSortBegin = clock();
    timSort(arrayToSort.begin(), arrayToSort.end(), comp);
    clock_t timSortEnd = clock();
    
    if (areEqual(arrayToSort, arrayToSortCopy, comp))
    {
        printf("OK TEST %u\n", numberOfTest);
        double stdStableSortTime = (double)(stdSortEnd - stdSortBegin) / CLOCKS_PER_SEC;
        double timSortTime = (double)(timSortEnd - timSortBegin) / CLOCKS_PER_SEC;
        printf("std::stable_sort         %lf\n", stdStableSortTime);
        printf("timSort                  %lf\n", timSortTime);
        printf("timSort/std::stable_sort %lf\n", timSortTime / stdStableSortTime);
    }
    else
    {
        printf("WRONG\n");
    }
}


///argv = [name, numberOfTest, typeOfTest, parameters..]
///typeOfTest == 1: generateRandomIntArray; parameters = length
///typeOfTest == 2: generatePartlySortedIntArray; parameters = lengthOfEach, numberOfParts
///typeOfTest == 3: generateStringArray; parameters = length, stringSize, useSpecialComparator
///typeOfTest == 4: generatePartlySortedStringArray; parameters = lengthOfEach, numberOfParts, stringSize, useSpecialComparator
///typeOfTest == 5: generatePairArray; parameters = length, useSpecialComparator
///typeOfTest == 6: generatePartlySortePairArray; parameters = lengthOfEach, numberOfParts, useSpecialComparator
///typeOfTest == 7: generatePointArray; parameters = length
///typeOfTest == 8: generatePartlySortedPointArray; parameters = lengthOfEach, numberOfParts;
int main(int argc, char **argv)
{
    if (argc <= 2)
    {
        throw "Not enough parameters - I can't even distinguish type of a test\n";
    }
    
    unsigned int numberOfTest = atoi(argv[1]);
    unsigned int typeOfTest = atoi(argv[2]);
    
    timsort_rand::srand(numberOfTest);
    
    if (typeOfTest == 1)
    {
        if (argc <= 3)
        {
            throw "Not enough parameters - I can't distinguish length of an array\n";
        }
        unsigned int length = atoi(argv[3]);
        test(timsort_rand::generateRandomArray<int>(length), numberOfTest);
    }
    else if (typeOfTest == 2)
    {
        if (argc <= 4)
        {
            throw "Not enough parameters - I can't distinguish, how partlySorted arrays should look like\n";
        }
        unsigned int lengthOfEach = atoi(argv[3]);
        unsigned int numberOfParts = atoi(argv[4]);
        test(timsort_rand::generatePartlySortedArray<int>(lengthOfEach, numberOfParts), numberOfTest);
    }
    else if (typeOfTest == 3)
    {
        if (argc <= 5)
        {
            throw "Not enough parameters - I can't distinguish, how array of string should look like or what comparator to use\n";
        }
        unsigned int length = atoi(argv[3]);
        unsigned int stringLength = atoi(argv[4]);
        bool useSpecialComparator = atoi(argv[5]);
        if (useSpecialComparator)
        {
            test(timsort_rand::generateRandomArray<std::string>(length, stringLength), numberOfTest, SpecialStringComparator());
        }
        else
        {
            test(timsort_rand::generateRandomArray<std::string>(length, stringLength), numberOfTest);
        }
    }
    else if (typeOfTest == 4)
    {
        if (argc <= 6)
        {
            throw "Not enough parameters - I can't distinguish, how partly sorted array of string should look like or what comparator to use\n";
        }
        unsigned int lengthOfEach = atoi(argv[3]);
        unsigned int numberOfParts = atoi(argv[4]);
        unsigned int stringLength = atoi(argv[5]);
        bool useSpecialComparator = atoi(argv[6]);
        if (useSpecialComparator)
        {
            test(timsort_rand::generatePartlySortedArray<std::string>(lengthOfEach, numberOfParts, stringLength, SpecialStringComparator()), numberOfTest, SpecialStringComparator());
        }
        else
        {
            test(timsort_rand::generatePartlySortedArray<std::string>(lengthOfEach, numberOfParts, stringLength), numberOfTest);
        }
    }
    else if (typeOfTest == 5)
    {
        if (argc <= 4)
        {
            throw "Not enough parameters - I can't distinguish, how array of pairs should look like or what comparator to use\n";
        }
        unsigned int length = atoi(argv[3]);
        bool useSpecialComparator = atoi(argv[4]);
        if (useSpecialComparator)
        {
            test(timsort_rand::generateRandomArray<std::pair<unsigned int, int> >(length), numberOfTest, SpecialPairComparator());
        }
        else
        {
            test(timsort_rand::generateRandomArray<std::pair<unsigned int, int> >(length), numberOfTest);
        }
    }
    else if (typeOfTest == 6)
    {
        if (argc <= 5)
        {
            throw "Not enough parameters - I can't distinguish, how partly sorted array of pairs should look like or what comparator to use\n";
        }
        unsigned int lengthOfEach = atoi(argv[3]);
        unsigned int numberOfParts = atoi(argv[4]);
        bool useSpecialComparator = atoi(argv[5]);
        if (useSpecialComparator)
        {
            test(timsort_rand::generatePartlySortedArray<std::pair<unsigned int, int> >(lengthOfEach, numberOfParts, 0, SpecialPairComparator()), numberOfTest, SpecialPairComparator());
        }
        else
        {
            test(timsort_rand::generatePartlySortedArray<std::pair<unsigned int, int> >(lengthOfEach, numberOfParts), numberOfTest);
        }
    }
    else if (typeOfTest == 7)
    {
        if (argc <= 3)
        {
            throw "Not enough parameters - I can't distinguish length of a Point array\n";
        }
        unsigned int length = atoi(argv[3]);
        test(timsort_rand::generateRandomArray<Point>(length), numberOfTest, pointSortFunction());
    }
    else if (typeOfTest == 8)
    {
        if (argc <= 4)
        {
            throw "Not enough parameters - I can't distinguish, how partlySorted Point arrays should look like\n";
        }
        unsigned int lengthOfEach = atoi(argv[3]);
        unsigned int numberOfParts = atoi(argv[4]);
        test(timsort_rand::generatePartlySortedArray<Point>(lengthOfEach, numberOfParts, 0, pointSortFunction()), numberOfTest, pointSortFunction());
    }
    else
    {
        throw "No such test type\n";
    }   
}