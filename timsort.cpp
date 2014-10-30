#include <ctime>
#include <cstdio>
#include <vector>
#include <iostream>
#include <algorithm>
#include "timsort.h"
#include "tests.h"

namespace timSortFunctionsAndClasses
{
    class TimSortParametersTwo : public TimSortParametersDefault
    {
    public:
        static TimSortParametersTwo TimSortParametersTwoObject;
        
        MergeActionType getMergeAction(unsigned int sizeOfX, unsigned int sizeOfY, unsigned int sizeOfZ) const
        {
            return getMergeAction(sizeOfX, sizeOfY);
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
};

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

class TestParameters
{
public:
    int argc;
    
    char **argv;
    
    unsigned int numberOfTest;
    
    unsigned int lengthOfEach;
    
    unsigned int numberOfParts;
    
    unsigned int argumentsShift;    
};


template<class ElementsType, class Compare>
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

template <class ElementsType, class Compare>
double sortAndGetTime(std::vector <ElementsType> &arrayToSort, void (*sort)(typename std::vector<ElementsType>::iterator, typename std::vector<ElementsType>::iterator, Compare), Compare comp)
{
    clock_t begin = clock();
    sort(arrayToSort.begin(), arrayToSort.end(), comp);
    clock_t end = clock();
    return double(end - begin) / CLOCKS_PER_SEC;
}

template <class ElementsType, class Compare = std::less<ElementsType> >
void proceedTest(std::vector<ElementsType> arrayToSort, unsigned int numberOfTest, Compare comp = Compare())
{
    std::vector<ElementsType> arrayToSortCopy = arrayToSort;
    double stdStableSortTime = sortAndGetTime(arrayToSortCopy, &std::stable_sort, comp);
    double timSortTime = sortAndGetTime(arrayToSort, &timSort, comp);
    
    if (areEqual(arrayToSort, arrayToSortCopy, comp))
    {
        printf("OK TEST %u\n", numberOfTest);
        printf("std::stable_sort         %lf\n", stdStableSortTime);
        printf("timSort                  %lf\n", timSortTime);    
        printf("timSort/std::stable_sort %lf\n", timSortTime / stdStableSortTime);
    }
    else
    {
        printf("WRONG\n");
    }
}

template<class ElementsType>
void test(TestParameters currentParameters)
{
    proceedTest(timsortRand::generatePartlySortedArray<ElementsType>(currentParameters.lengthOfEach, currentParameters.numberOfParts), currentParameters.numberOfTest);
}

template<>
void test<std::string>(TestParameters currentParameters)
{
    if (currentParameters.argc <= currentParameters.argumentsShift)
    {
        throw "Not enough parameters - I can't distinguish string length\n";
    }
    
    unsigned int stringLength = atoi(currentParameters.argv[currentParameters.argumentsShift++]);
    bool useSpecialComparator = (currentParameters.argc != currentParameters.argumentsShift);
    
    if (useSpecialComparator)
    {
        proceedTest(timsortRand::generatePartlySortedArray<std::string>(
                                                                        currentParameters.lengthOfEach, 
                                                                        currentParameters.numberOfParts, stringLength, 
                                                                        SpecialStringComparator()
                                                                       ),
                    currentParameters.numberOfTest, SpecialStringComparator()
                   );
    }
    else
    {
        proceedTest(timsortRand::generatePartlySortedArray<std::string>(
                                                                        currentParameters.lengthOfEach, 
                                                                        currentParameters.numberOfParts, stringLength
                                                                        ),
                    currentParameters.numberOfTest
                   );
    }
}

template<>
void test<std::pair<unsigned int, int> >(TestParameters currentParameters)
{
    bool useSpecialComparator = (currentParameters.argc != currentParameters.argumentsShift);
    if (useSpecialComparator)
    {
        proceedTest(timsortRand::generatePartlySortedArray<std::pair<unsigned int, int> >(
                                                                                        currentParameters.lengthOfEach, 
                                                                                        currentParameters.numberOfParts, 0, 
                                                                                        SpecialPairComparator()
                                                                                        ),
                    currentParameters.numberOfTest, SpecialPairComparator()
                   );
    }
    else
    {
        proceedTest(timsortRand::generatePartlySortedArray<std::pair<unsigned int, int> >(
                                                                                        currentParameters.lengthOfEach, 
                                                                                        currentParameters.numberOfParts, 0
                                                                                        ),
                    currentParameters.numberOfTest
                   );
    }
}

template<>
void test<timSortTestClasses::Point>(TestParameters currentParameters)
{
    proceedTest(timsortRand::generatePartlySortedArray<timSortTestClasses::Point>(
                                                                                  currentParameters.lengthOfEach, 
                                                                                  currentParameters.numberOfParts, 0, 
                                                                                  timSortTestClasses::PointComparator()
                                                                                ),
                    currentParameters.numberOfTest, timSortTestClasses::PointComparator()
                   );
}


///argv = [name, numberOfTest, typeOfTest, parameters..]
///typeOfTest == 1: generateRandomIntArray; parameters = length
///typeOfTest == 2: generatePartlySortedIntArray; parameters = numberOfParts, lengthOfEach
///typeOfTest == 3: generateStringArray; parameters = length, stringSize, useSpecialComparator
///typeOfTest == 4: generatePartlySortedStringArray; parameters = numberOfParts, lengthOfEach, stringSize, useSpecialComparator
///typeOfTest == 5: generatePairArray; parameters = length, useSpecialComparator
///typeOfTest == 6: generatePartlySortePairArray; parameters = numberOfParts, lengthOfEach, useSpecialComparator
///typeOfTest == 7: generatePointArray; parameters = length
///typeOfTest == 8: generatePartlySortedPointArray; parameters = numberOfParts, lengthOfEach


int main(int argc, char **argv)
{
    if (argc <= 3)
    {
        throw "Not enough parameters - I can't even distinguish type or number of a test, or how array should look like\n";
    }
    
    unsigned int numberOfTest = atoi(argv[1u]);
    unsigned int typeOfTest = atoi(argv[2u]);
    unsigned int numberOfParts = atoi(argv[3u]);
    unsigned int lengthOfEach = 1u;
    unsigned int argumentsShift = 4u;
    
    timsortRand::srand(numberOfTest);
    
    if (typeOfTest % 2u == 0u)
    {
        if (argc <= 4)
        {
            throw "Not enough parameters - I can't distinguish number of parts in partly sorted array\n";
        }
        lengthOfEach = atoi(argv[argumentsShift++]);
    }
    
    TestParameters currentParameters = {argc, argv, numberOfTest, lengthOfEach, numberOfParts, argumentsShift};    
    
    typeOfTest = (typeOfTest + 1) / 2u;
   
    switch (typeOfTest)
    {
        case 1u:
            test<int>(currentParameters);
            break;
        case 2u:
            test<std::string>(currentParameters);
            break;
        case 3u:
            test<std::pair<unsigned int, int> >(currentParameters);
            break;
        case 4u:
            test<timSortTestClasses::Point>(currentParameters);
            break;
        default:
            throw "No such test type\n";
    }
    return 0;
}