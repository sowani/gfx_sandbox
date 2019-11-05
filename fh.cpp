#include <math.h>
#include "fh.h"

#define MAX_CLOSEST_ITEMS 10

bool handleComponentFocus (KeypadKey key, ComponentBase* componentStack[],
                           uint8_t stackSize)
{
    int indx = findClosestChild (key, componentStack, stackSize);
    PRINT("***** closest child returned index = [%d] \n", indx);

    if ((indx >= 0) && (indx < stackSize))
    {
        if (getComponentIndexInFocus (componentStack, stackSize) != indx)
        {
            // Component lost focus
            ComponentBase *componentInFocus =
                getComponentInFocus (componentStack, stackSize);
            componentInFocus->setComponentFocusState (UNFOCUSED);

            // Component in focus
            componentStack[indx]->setComponentFocusState (FOCUSED);
            return true;
        }
    }
    return false;
}

/////////////////////////////////////////////////////////////////////////////
// **************************************************************************
//
// IMPORTANT NOTE: Please NEVER delete any of the commented lines
//                 in findClosestChild().
//
// **************************************************************************
/////////////////////////////////////////////////////////////////////////////

int findClosestChild (KeypadKey key, ComponentBase* componentStack[],
                      uint8_t stackSize)
{
    if (componentStack == NULL)
    {
        PRINT("Error: findClosestChild: componentStack is NULL\n");
        return -1;
    }

    //Screen* topScreen = getTopScreen();
    int dist[MAX_COMPONENTS] = {0};
    int candidateArray1[MAX_CLOSEST_ITEMS] = {0};
    int candidateArray2[MAX_CLOSEST_ITEMS] = {0};
    int leastDist = 200;
    Box box1, box2;
    int i, j, k, currCompIdx;
    i = j = k = currCompIdx = 0;
    int selectedCandidateIndex = -1;
    int newX, newY, leastX, leastY, numCandidates, newDist;
    newX = newY = leastX = leastY = numCandidates = newDist = 0;
    int minX, minY;
    minX = minY = 0;

    //PRINT("component stack --------------------- \n");
    //dumpComponentStack(componentStack, stackSize);
    //PRINT("component stack --------------------- \n");
    //PRINT ("\n\n");

    /////////////////////////////////////////////////////////////////////////
    // get index of current focused component
    //
    // part 1:
    // foreach component in components array
    //    calculate distance between current, component --> distance array
    // tag component with least non-zero distance.
    //

    currCompIdx = getComponentIndexInFocus (componentStack, stackSize);
    if (currCompIdx < 0)
    {
        PRINT("Component in Focus is None.");
        return -1;
    }
    //PRINT("1. current component in focus = [%d]", currCompIdx);

    ComponentBase *currentFComponent = componentStack[currCompIdx];
    box1 = currentFComponent->getBoundingBox();
    currentFComponent->dumpComponent();
    minX = (currentFComponent->getBoundingBox()).x;
    minY = (currentFComponent->getBoundingBox()).y;

    for (i = 0; i < stackSize; i++)
    {
        if (currCompIdx == i)
        {
            //PRINT("skipping self [%d] \n", i);
            dist[i] = 0;
        }
        else
        {
            dist[i] = calculateDistance(currentFComponent, componentStack[i]);
            if ((componentStack[i])->getComponentFocusPolicy() == NON_FOCUSABLE)
            {
                //PRINT("i = [%d] is non-focusable. \n", i);
            }
            if ( (dist[i] > 0) && (dist[i] < leastDist) )
                leastDist = dist[i];
        }
    }
    //PRINT ("dist array: \n");
    //for (int xx = 0; xx < stackSize; xx++)
    //  PRINT ("dist[%d] = [%d] \n", xx, dist[xx]);
    //PRINT ("\n");

    /////////////////////////////////////////////////////////////////////////
    // part 2:
    // foreach component in components array
    //     find component with select x/y criteria --> component-array2
    //

    selectedCandidateIndex = -1;
    switch (key)
    {
        case CURSOR_UP:
            // x == x-curr, y < y-curr
#if 0
            j = 0;
            for (i = 0; i < stackSize; i++)
            {
                if (i != currCompIdx)
                {
                    box2 = componentStack[i]->getBoundingBox();
                    //if ( ((box2.x == box1.x) && (box2.y < box1.y)) && (dist[i] == leastDist) )
                    if ( (box2.y < box1.y) && (box2.x <= box1.x) )
                        candidateArray1[j++] = i;
                }
            }
#endif

            // step 1: select all elements lying to top side.
            j = 0;
            for (i = 0; i < stackSize; i++)
            {
                if (i != currCompIdx)
                {
                    box2 = (componentStack[i])->getBoundingBox();
                    //if ( ((box2.x == box1.x) && (box2.y < box1.y)) && (dist[i] == leastDist) )
                    if ((box2.y < box1.y) && ((componentStack[i])->getComponentFocusPolicy() != NON_FOCUSABLE))
                        candidateArray1[j++] = i;  // store only index
                }
            }
            numCandidates = j;
            //PRINT ("step 1: all top candidates \n");
            //for (i = 0; i < numCandidates; i++)
            //{
            //  PRINT ("[%d] i.e. %d : ", i, candidateArray1[i];
            //  (componentStack[candidateArray1[i]])->dumpComponent();
            //}
            //PRINT ("\n");

            // step 2:
            // from all "top" candidates, select the ones which are immediately at top.
            newY = 0;
            k = 0;
            leastY = ((componentStack[candidateArray1[0]])->getBoundingBox()).y;
            for (int i = 0; i < numCandidates; i++)
            {
                newY = ((componentStack[candidateArray1[i]])->getBoundingBox()).y;
                if (newY >= leastY)
                    candidateArray2[k++] = candidateArray1[i];
            }
            numCandidates = k;
            //cout << "step 2: all immediate top candidates" << endl;
            //for (i = 0; i < k; i++)
            //{
            //  cout << "[" << i << "] i.e. " << candidateArray2[i] << ": ";
            //  (componentStack[candidateArray2[i]])->dumpComponent();
            //}
            //cout << endl;

            // step 3:
            // from all immediate "top" candidates, select the candidate with least X.
            newX = 0;
            leastX = ((componentStack[candidateArray2[0]])->getBoundingBox()).x;
            leastDist = dist [candidateArray2[0]];
            for (int i = 0; i < numCandidates; i++)
            {
                newX = ((componentStack[candidateArray2[i]])->getBoundingBox()).x;
                newDist = dist[candidateArray2[i]];
                //if (newX < leastX)
                if (newDist <= leastDist)
                {
                    selectedCandidateIndex = candidateArray2[i];
                    leastDist = newDist;
                }
            }
            //cout << "step 3: search exact candidate" << endl;
            //cout << "[" << selectedCandidateIndex << "] i.e. " << selectedCandidateIndex << ": ";
            //(componentStack[selectedCandidateIndex])->dumpComponent();

            break;
        case CURSOR_DOWN:
            // x == x-curr, y > y-curr
#if 0
            j = 0;
            for (i = 0; i < stackSize; i++)
            {
                if (i != currCompIdx)
                {
                    box2 = (componentStack[i])->getBoundingBox();
                    //if ( ((box2.x == box1.x) && (box2.y > box1.y)) && (dist[i] == leastDist) )
                    if ( (box2.y > box1.y) && (box2.x >= box1.x) )
                        candidateArray1[j++] = i;
                }
            }
#endif

            // step 1: select all elements lying to bottom side.
            j = 0;
            for (int i = 0; i < stackSize; i++)
            {
                if (i != currCompIdx)
                {
                    box2 = (componentStack[i])->getBoundingBox();
                    //if ( ((box2.x == box1.x) && (box2.y > box1.y)) && (dist[i] == leastDist) )
                    if ((box2.y > box1.y) && ((componentStack[i])->getComponentFocusPolicy() != NON_FOCUSABLE))
                        candidateArray1[j++] = i;  // store only index
                }
            }
            numCandidates = j;
            //cout << "step 1: all bottom candidates" << endl;
            //for (i = 0; i < numCandidates; i++)
            //{
            //  cout << "[" << i << "] i.e. " << candidateArray1[i] << ": ";
            //  (componentStack[candidateArray1[i]])->dumpComponent();
            //}
            //cout << endl;

            // step 2:
            // from all "bottom" candidates, select the ones which are immediately at bottom.
            newY = 0;
            k = 0;
            leastY = ((componentStack[candidateArray1[0]])->getBoundingBox()).y;
            for (int i = 0; i < numCandidates; i++)
            {
                newX = ((componentStack[candidateArray1[i]])->getBoundingBox()).x;
                newY = ((componentStack[candidateArray1[i]])->getBoundingBox()).y;
                if ((newY <= leastY) || (minX == newX))
                    candidateArray2[k++] = candidateArray1[i];
            }
            numCandidates = k;
            //cout << "step 2: all immediate bottom candidates" << endl;
            //for (i = 0; i < k; i++)
            //{
            //  cout << "[" << i << "] i.e. " << candidateArray2[i] << ": ";
            //  (componentStack[candidateArray2[i]])->dumpComponent();
            //}
            //cout << endl;

            // step 3:
            // from all immediate "bottom" candidates, select the candidate with least X.
            newX = 0;
            leastX = ((componentStack[candidateArray2[0]])->getBoundingBox()).x;
            leastDist = dist [candidateArray2[0]];
            for (int i = 0; i < numCandidates; i++)
            {
                newX = ((componentStack[candidateArray2[i]])->getBoundingBox()).x;
                newDist = dist [candidateArray2[i]];
                //cout << "3. [" << candidateArray2[i] << "]: ";
                //cout << " L: " << leastDist << " N: " << newDist;
                //cout << " Lx: " << leastX << " Nx: " << newX << endl;
                //if (newX < leastX)
                if (newDist <= leastDist)
                {
                    //xif (newDist == leastDist)
                    //x{
                        //xif (newX < leastX)
                        //x{
                            //xselectedCandidateIndex = candidateArray2[i];
                            leastX = newX;
                        //x}
                    //x}
                    //xelse
                    //x{
                        selectedCandidateIndex = candidateArray2[i];
                        leastDist = newDist;
                    //x}
                }
            }
            //cout << "step 3: search exact candidate" << endl;
            //cout << "[" << selectedCandidateIndex << "] i.e. " << selectedCandidateIndex << ": ";
            //(componentStack[selectedCandidateIndex])->dumpComponent();

            break;
        case CURSOR_LEFT:
            // x < x-curr
            // step 1: select all elements lying to left side.
            j = 0;
            for (int i = 0; i < stackSize; i++)
            {
                if (i != currCompIdx)
                {
                    box2 = (componentStack[i])->getBoundingBox();
                    if ((box2.x < box1.x) && ((componentStack[i])->getComponentFocusPolicy() != NON_FOCUSABLE))
                        candidateArray1[j++] = i;  // store only index
                }
            }
            numCandidates = j;
            //cout << "step 1: all left candidates" << endl;
            //for (i = 0; i < numCandidates; i++)
            //{
            //  cout << "[" << i << "] i.e. " << candidateArray1[i] << ": ";
            //  (componentStack[candidateArray1[i]])->dumpComponent();
            //}
            //cout << endl;

            // step 2:
            // from all "left" candidates, select the ones which are immediately at left.
            newX = 0;
            k = 0;
            leastX = ((componentStack[candidateArray1[0]])->getBoundingBox()).x;
            for (int i = 0; i < numCandidates; i++)
            {
                newX = ((componentStack[candidateArray1[i]])->getBoundingBox()).x;
                if (newX >= leastX)
                    candidateArray2[k++] = candidateArray1[i];
            }
            numCandidates = k;
            //cout << "step 2: all immediate left candidates" << endl;
            //for (i = 0; i < k; i++)
            //{
            //  cout << "[" << i << "] i.e. " << candidateArray2[i] << ": ";
            //  (componentStack[candidateArray2[i]])->dumpComponent();
            //}
            //cout << endl;

            // step 3:
            // from all immediate "left" candidates, select the candidate with least Y.
            newY = 0;
            leastY = ((componentStack[candidateArray2[0]])->getBoundingBox()).y;
            leastDist = dist [candidateArray2[0]];
            for (int i = 0; i < numCandidates; i++)
            {
                newY = ((componentStack[candidateArray2[i]])->getBoundingBox()).y;
                newDist = dist[candidateArray2[i]];
                if (newDist <= leastDist)
                {
                    if (newDist == leastDist)
                    {
                        if (newY <= leastY)
                        {
                            selectedCandidateIndex = candidateArray2[i];
                            leastY = newY;
                        }
                    }
                    else if (newDist < leastDist)
                    {
                        if (newY <= minY)
                            selectedCandidateIndex = candidateArray2[i];
                    }
                    else
                    {
                        selectedCandidateIndex = candidateArray2[i];
                        leastDist = newDist;
                    }
                }
            }
            //cout << "step 3: search exact candidate" << endl;
            //cout << "[" << selectedCandidateIndex << "] i.e. " << selectedCandidateIndex << ": ";
            //(componentStack[selectedCandidateIndex])->dumpComponent();

            break;
        case CURSOR_RIGHT:
            // x > x-curr
            // step 1: select all elements lying to right side.
            j = 0;
            for (int i = 0; i < stackSize; i++)
            {
                if (i != currCompIdx)
                {
                    box2 = (componentStack[i])->getBoundingBox();
                    //if ( ((box2.x > box1.x) && (box2.y == box1.y)) && (dist[i] == leastDist) )
                    if ((box2.x > box1.x) && ((componentStack[i])->getComponentFocusPolicy() != NON_FOCUSABLE))
                        candidateArray1[j++] = i;  // store only index
                }
            }
            numCandidates = j;
            //cout << "step 1: all right candidates" << endl;
            //for (i = 0; i < numCandidates; i++)
            //{
            //  cout << "[" << i << "] i.e. " << candidateArray1[i] << ": ";
            //  (componentStack[candidateArray1[i]])->dumpComponent();
            //}
            //cout << endl;

            // step 2:
            // from all "right" candidates, select the ones which are immediately at right.
            newX = 0;
            k = 0;
            leastX = ((componentStack[candidateArray1[0]])->getBoundingBox()).x;
            for (int i = 0; i < numCandidates; i++)
            {
                newX = ((componentStack[candidateArray1[i]])->getBoundingBox()).x;
                newY = ((componentStack[candidateArray1[i]])->getBoundingBox()).y;
                if ((newX <= leastX) || (minY == newY))
                    candidateArray2[k++] = candidateArray1[i];
            }
            numCandidates = k;
            //cout << "step 2: all immediate right candidates" << endl;
            //for (i = 0; i < k; i++)
            //{
            //  cout << "[" << i << "] i.e. " << candidateArray2[i] << ": ";
            //  (componentStack[candidateArray2[i]])->dumpComponent();
            //}
            //cout << endl;

            // step 3:
            // from all immediate "right" candidates, select the candidate with least Y.
            newY = 0;
            leastY = ((componentStack[candidateArray2[0]])->getBoundingBox()).y;
            leastDist = dist [candidateArray2[0]];
            for (int i = 0; i < numCandidates; i++)
            {
                newY = ((componentStack[candidateArray2[i]])->getBoundingBox()).y;
                newDist = dist[candidateArray2[i]];
                //cout << "[" << candidateArray2[i] << "]: Ld = " << leastDist << ", Nd = " << newDist;
                //cout << " Ly = " << leastY << " newY = " << newY;
                //if ( (newY < leastY) && (newDist < leastDist) )
                if (newDist <= leastDist)
                {
                    if (newDist == leastDist)
                    {
                        if (newY <= leastY)
                        {
                            selectedCandidateIndex = candidateArray2[i];
                            leastY = newY;
                        }
                    }
                    else if (newDist < leastDist)
                    {
                        if (newY <= minY)
                            selectedCandidateIndex = candidateArray2[i];
                    }
                    else
                    {
                        selectedCandidateIndex = candidateArray2[i];
                        leastDist = newDist;
                    }
                }
                //leastY = newY;
                //cout << " selected: " << selectedCandidateIndex << endl;
            }
            //cout << "step 3: search exact candidate" << endl;
            //cout << "[" << selectedCandidateIndex << "] i.e. "
            //     << selectedCandidateIndex << ": ";
            //(componentStack[selectedCandidateIndex])->dumpComponent();

            break;
        default:
            break;
    }

    //PRINT ("selected closest item = %d \n",
    //       candidateArray2[selectedCandidateIndex]);
    //(componentStack[candidateArray2[selectedCandidateIndex]])->dumpComponent();
    //PRINT ("--------------\n\n");

#if 0
    //int maxElib = j;
    //std::cout << "2. built array of eligible candidates [" << maxElib << "]:" << std::endl;
    //for (int ii = 0; ii < maxElib; ii++)
    //{
        //std::cout << "candid[" << ii << "] = " << candidateArray1[ii] << " D = " << dist[ candidateArray1[ii] ] << " ";
        //(componentStack[candidateArray1[ii]])->dumpComponent();
    //}

    /////////////////////////////////////////////////////////////////////////
    // part 3
    // select tagged component from part 1 in component-array2
    // return component
    // TODO: add check for more than 1 candidates.

    int ld2 = dist[ candidateArray1[0] ];
    int idx2 = 1;
    for (int i = 1; i < maxElib; i++)
    {
        //if (candidateArray1[i] == closestItem)
            //break;
        if (ld2 > dist[ candidateArray1[i] ])
            idx2 = i;
    }
#endif

    return (selectedCandidateIndex);
}

uint16_t calculateDistance (ComponentBase *comp1, ComponentBase* comp2)
{
    if (comp1 == 0 || comp2 == 0)
    {
        PRINT ("calculateDistance: comp1 or comp2 is NULL");
        return 0;
    }

    uint16_t X= pow((comp1->getBoundingBox().x - comp2->getBoundingBox().x), 2);
    uint16_t Y= pow((comp1->getBoundingBox().y - comp2->getBoundingBox().y), 2);
    return (uint16_t)sqrt(X + Y);
}

int getComponentIndexInFocus (ComponentBase* componentStack[],
                              uint8_t stackSize)
{
    if (componentStack == NULL)
        return -1;

    for (int index = 0; index < stackSize; index++)
    {
        if (componentStack[index] == 0)
            return -1;
        if (componentStack[index]->getComponentFocusState() == FOCUSED)
            return index;
    }
    return -1;
}

ComponentBase* getComponentInFocus (ComponentBase* componentStack[],
                                    uint8_t stackSize)
{
    if (componentStack == NULL)
        return NULL;

    for (int index = 0; index < stackSize; index++)
    {
        if (componentStack[index] == 0)
            return NULL;

        if (componentStack[index]->getComponentFocusState() == FOCUSED)
            return componentStack[index];
    }
    return NULL;
}

void dumpComponentStack (ComponentBase* componentStack[], uint8_t stackSize)
{
    if (componentStack == NULL)
        return;

    for (int idx = 0; idx < stackSize; idx++)
    {
        //PRINT ("[%d] -> ", idx);
        componentStack[idx]->dumpComponent();
    }
}
