#ifndef _TIM_SORT
#define _TIM_SORT

#include <algorithm>
#include <iterator>


namespace TimSortFunctionsAndClasses
{
    enum BoundType
    {
        EBT_LOWER_BOUND,
        EBT_UPPER_BOUND
    };

    ///We consider X to be the top element of stack, Y to be an element, previous to X, Z is an element, previous to Y
    enum MergeActionType
    {
        MERGE_ZY,
        MERGE_YX,
        MERGE_NOTHING
    };
    
    class ITimSortParameters
    {
    public:
        virtual MergeActionType getMergeAction(unsigned int sizeOfX, unsigned int sizeOfY, unsigned int sizeOfZ) const = 0;
        
        virtual MergeActionType getMergeAction(unsigned int sizeOfX, unsigned int sizeOfY) const = 0;
        
        virtual unsigned int getMinRun(unsigned int numberOfElements) const  = 0;
        
        virtual unsigned int getMergeStupidIterationsLimit() const = 0;
    };


    class TimSortParametersDefault: public ITimSortParameters
    {
    private:
        static const unsigned int MIN_RUN_CALC_BORDER = 64;
        
        static const unsigned int MERGE_STUPID_ITERATIONS_LIMIT = 7;
    public:
        
        TimSortParametersDefault(){}
        
        MergeActionType getMergeAction(unsigned int sizeOfX, unsigned int sizeOfY, unsigned int sizeOfZ) const
        {
            if (sizeOfZ && sizeOfZ <= sizeOfY + sizeOfX)
            {
                if (sizeOfZ <= sizeOfX)
                {
                    return MERGE_ZY;
                }
                else
                {
                    return MERGE_YX;
                }
            }
            else if (sizeOfY <= sizeOfX)
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
        
        unsigned int getMinRun(unsigned int numberOfElements) const
        {
            bool shallWeAddOneToMinRun = 0;
            while (numberOfElements >= MIN_RUN_CALC_BORDER)
            {
                numberOfElements >>= 1;
                shallWeAddOneToMinRun |= (numberOfElements & 1);
            }
            return numberOfElements + shallWeAddOneToMinRun;
        }
        
        unsigned int getMergeStupidIterationsLimit() const
        {
            return MERGE_STUPID_ITERATIONS_LIMIT;
        }
    };


#ifdef _DEBUG_CNT_OPERATIONS
    int insertionSortIterationsCnt;
    int mergeOperationsCnt;
#endif

    template<class RandomAccessIterator>
    class Run
    {
        RandomAccessIterator iterator;
        
        unsigned int size;
    public:
        Run()
        {
        }
        
        Run(RandomAccessIterator iterator, unsigned int size) : iterator(iterator), size(size)
        {    
        }
        
        RandomAccessIterator getFirstIterator() const
        {
            return iterator;
        }
        
        RandomAccessIterator getLastIterator() const
        {
            return iterator + size;
        }
        
        unsigned int getSize() const
        {
            return size;
        }
        
        void incrementSize()
        {
            ++size;
        }
        
        void addToSize(unsigned int addition)
        {
            size += addition;
        }
    };

        
    template<class RandomAccessIterator>
    class StackOfRuns
    {
        std::vector <Run<RandomAccessIterator> > body;
    public:
        void push(const Run<RandomAccessIterator> &element)
        {
            body.push_back(element);
        }

        void pop()
        {
            body.pop_back();
        }

        unsigned int size() const
        {
            return body.size();
        }

        const Run<RandomAccessIterator> &operator[](int i) const
        {
            return body[static_cast<int> (size()) + i];
        }
        
        template<class Compare>
        void mergeRuns(int indexOfSecondMergingElement, Compare comp, const ITimSortParameters* const params)
        {
            if (indexOfSecondMergingElement < -2 || indexOfSecondMergingElement > -1)
                throw "unsupported merging";
            Run<RandomAccessIterator> saved = operator[](-1);
            if (indexOfSecondMergingElement != -1)
            {
                pop();
            }
            merge(
                  operator[](-2).getFirstIterator(), operator[](-1).getFirstIterator(),
                  operator[](-1).getLastIterator(),
                  comp,
                  params
                 );
            body[static_cast<int>(size()) - 2].addToSize(operator[](-1).getSize());
            pop();
            if (indexOfSecondMergingElement != -1)
            {
                push(saved);
            }
        }
    };

    
    template<class RandomAccessIterator, class Compare>
    bool compareElementWithPrevious(const RandomAccessIterator &iterator, Compare comp)
    {
        return comp(*iterator, *(iterator - 1));
    }

    template<class RandomAccessIterator>
    void swapElementWithPrevious(const RandomAccessIterator &iterator)
    {
        std::swap(*iterator, *(iterator - 1));
    }
        
    template <class RandomAccessIterator, class Compare>
    void insertionSort(const RandomAccessIterator &first, const RandomAccessIterator &last, Compare comp, unsigned int sortedSize)
    {
        for (RandomAccessIterator currentElement = first + sortedSize; currentElement != last; ++currentElement)
        {
            RandomAccessIterator elementToSwapBack = currentElement;
            while (elementToSwapBack != first && compareElementWithPrevious(elementToSwapBack, comp))
            {
    #ifdef _DEBUG_CNT_OPERATIONS
                ++insertionSortIterationsCnt;
    #endif
                //std::swap(*elementToSwapBack, *(elementToSwapBack - 1));
                swapElementWithPrevious(elementToSwapBack--);
            }
        }
    }

    
    template<class RandomAccessIterator, class OutputIterator, class TypeOfValueToCompareWith, class Compare>
    void doMove(
                RandomAccessIterator &begin, const RandomAccessIterator &end, const TypeOfValueToCompareWith &valueToCompareWith,
                OutputIterator &output, BoundType boundType, Compare comp
               )
    {
        RandomAccessIterator nextElementIterator;
        
        if (boundType == EBT_LOWER_BOUND)
        {
            nextElementIterator = std::lower_bound(begin, end, valueToCompareWith, comp);
        }
        else
        {
            nextElementIterator = std::upper_bound(begin, end, valueToCompareWith, comp);
        }
        
        std::copy(begin, nextElementIterator, output);
        output += (nextElementIterator - begin);
        begin = nextElementIterator;
    }


    template <class RandomAccessIterator, class Compare>
    void mergeLeft(
                   const RandomAccessIterator &first, const RandomAccessIterator &middle,
                   const RandomAccessIterator &last, Compare comp,
                   const ITimSortParameters* const params
                  )
    {
        
#ifdef _USE_STD_INPLACE_MERGE
        return void(std::inplace_merge(first, middle, last, comp));
#endif
        
        std::vector <typename std::iterator_traits<RandomAccessIterator>::value_type> temporary(first, middle);
        typename std::vector <typename std::iterator_traits<RandomAccessIterator>::value_type>::iterator pointerToElementInFirstArray = temporary.begin();
        
        RandomAccessIterator pointerToElementInSecondArray = middle;
        RandomAccessIterator placeToInsert = first;
        
        unsigned int sameLastMove = 0;
        bool typeOfLastMove = 0;
        
        while (pointerToElementInFirstArray != temporary.end() && pointerToElementInSecondArray != last)
        {
            bool typeOfCurrentMove = comp(*pointerToElementInSecondArray, *pointerToElementInFirstArray);
            if (typeOfCurrentMove != typeOfLastMove)
                sameLastMove = 0;
            ++sameLastMove;
            typeOfLastMove = typeOfCurrentMove;
            
#ifndef _DISABLE_GALOP
            if (sameLastMove != params->getMergeStupidIterationsLimit())
#else
            if (true)
#endif
            {
                *(placeToInsert++) = (typeOfCurrentMove ? *(pointerToElementInSecondArray++) : *(pointerToElementInFirstArray++));
            }
            else
            {
                if (typeOfLastMove)
                {
                    doMove(pointerToElementInSecondArray, last, *pointerToElementInFirstArray, placeToInsert, EBT_LOWER_BOUND, comp);
                }
                else
                {
                    doMove(pointerToElementInFirstArray, temporary.end(), *pointerToElementInSecondArray, placeToInsert, EBT_UPPER_BOUND, comp);
                }
                sameLastMove = 0;
            }
        }
        
        std::copy(pointerToElementInFirstArray, temporary.end(), placeToInsert);
    }

    template<class Compare>
    class ReverseComparator
    {
        Compare comp;

    public:   
        ReverseComparator(const Compare &comp) : comp(comp)
        {
        }
    
        template<class ElementType>
        bool operator()(const ElementType &first, const ElementType &second) const
        {
            return comp(second, first);
        }
    };

    template <class RandomAccessIterator, class Compare>
    void mergeRight(
                    const RandomAccessIterator &first, const RandomAccessIterator &middle,
                    const RandomAccessIterator &last, Compare comp,
                    const ITimSortParameters* const params
                   )
    {
        typedef std::reverse_iterator<RandomAccessIterator> RIter;
        mergeLeft(RIter(last), RIter(middle), RIter(first), ReverseComparator<decltype(comp)>(comp), params);
    }
    

    template <class RandomAccessIterator, class Compare>
    void merge(
               const RandomAccessIterator &first, const RandomAccessIterator &middle,
               const RandomAccessIterator &last, Compare comp,
               const ITimSortParameters* const params
              )
    {   
#ifdef _DEBUG_CNT_OPERATIONS
        mergeOperationsCnt += (last - first);
#endif
        
        if (middle - first <= last - middle)
        {
            mergeLeft(first, middle, last, comp, params);
        }
        else
        {
            mergeRight(first, middle, last, comp, params);
        }
    }


    ///Takes currentElement iterator and stack
    ///Pushes next Run into stack
    ///After procedure, currentElement iterator points to the first element of next run or to the last element
    template<class RandomAccessIterator, class Compare>
    void pushNextRun(
                     RandomAccessIterator &currentElement, const RandomAccessIterator &last,
                     StackOfRuns<RandomAccessIterator> &runs, unsigned int minRun, Compare comp
                    )
    {
            Run<RandomAccessIterator> nextRun(currentElement++, 1u);
            
            if (currentElement != last)
            {
                bool compareResult = compareElementWithPrevious(currentElement, comp);
                while (currentElement != last && compareElementWithPrevious(currentElement, comp) == compareResult)
                {
                    ++currentElement;
                    nextRun.incrementSize();
                }
                
                if (compareResult)
                {
                    std::reverse(nextRun.getFirstIterator(), nextRun.getLastIterator());
                }
                
                if (currentElement != last && nextRun.getSize() < minRun)
                {
                    unsigned int sizeDifference = std::min(static_cast<unsigned int> (last - currentElement), minRun - nextRun.getSize());
                    currentElement += sizeDifference;
                    nextRun.addToSize(sizeDifference);
                    insertionSort(nextRun.getFirstIterator(), nextRun.getLastIterator(), comp, nextRun.getSize() - sizeDifference);
                }
            }
            
            runs.push(nextRun);
    }
    
    template <class RandomAccessIterator, class Compare>
    void processCurrentStackOfRuns(
                                   StackOfRuns<RandomAccessIterator> &runs,
                                   const ITimSortParameters* const params, Compare comp = Compare()
                                  )
    {
        while (runs.size() > 1)
        {
            MergeActionType action;
            
            if (runs.size() != 2u)
            {
                action = params->getMergeAction(runs[-1].getSize(), runs[-2].getSize(), runs[-3].getSize());
            }
            else
            {
                action = params->getMergeAction(runs[-1].getSize(), runs[-2].getSize());
            }
            
            switch (action)
            {
                case MERGE_YX:
                    runs.mergeRuns(-1, comp, params);
                    break;
                case MERGE_ZY:
                    runs.mergeRuns(-2, comp, params);
                    break;
                case MERGE_NOTHING:
                    return;
                default:
                    throw "Bad timSort parameters\n";
            }
        }
    }
};


template <class RandomAccessIterator, class Compare>
void timSort(
             RandomAccessIterator first, RandomAccessIterator last, 
             const TimSortFunctionsAndClasses::ITimSortParameters* const params, Compare comp
            ) // comp(a, b) <=> a < b;
{    
    unsigned int numberOfElements = last - first;
    unsigned int minRun = params->getMinRun(numberOfElements);

    TimSortFunctionsAndClasses::StackOfRuns<RandomAccessIterator> runs;
    
#ifdef _DEBUG_CNT_OPERATIONS
    mergeOperationsCnt = 0;
    insertionSortIterationsCnt = 0;
#endif
    
    for (RandomAccessIterator currentElement = first; currentElement != last;)
    {   
        TimSortFunctionsAndClasses::pushNextRun(currentElement, last, runs, minRun, comp);
        TimSortFunctionsAndClasses::processCurrentStackOfRuns(runs, params, comp);
    }
    
#ifdef _DEBUG_CNT_OPERATIONS
    printf("diffMergeOperations: %lf\n", 1.0 * mergeOperationsCnt / numberOfElements);
    printf("diffInsertSortOperations: %lf\n", 1.0 * insertionSortIterationsCnt / numberOfElements);
#endif
    
    while (runs.size() != 1)
    {
        runs.mergeRuns(-1, comp, params);
    }
}

template <class RandomAccessIterator, class Compare>
void timSort(RandomAccessIterator first, RandomAccessIterator last, Compare comp) /// comp(a, b) <=> a < b;
{
    TimSortFunctionsAndClasses::TimSortParametersDefault params;
    timSort(first, last, &params, comp);
}

template<class RandomAccessIterator>
void timSort(RandomAccessIterator first, RandomAccessIterator last)
{
    TimSortFunctionsAndClasses::TimSortParametersDefault params;
    timSort(first, last, &params, std::less<typename std::iterator_traits<RandomAccessIterator>::value_type>());
}

#endif