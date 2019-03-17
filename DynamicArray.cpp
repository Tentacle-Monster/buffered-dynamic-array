#include <iostream>
#include <time.h>
using namespace std;

template <typename T>
class DArray
{

  private:
    T *RealArray;     // Array whith buffers
    T *data;          // Useful part
    float buffersize; // buffer persent
    int length;       // useful length
    int start;
    int reallength;
    void create(int n)
    {
        create(n, 0);
    }

    void create(int n, float b)
    {
        try
        {
            if (RealArray != NULL)
            {
                delete[] RealArray;
            }
            if (n <= 0)
            {
                length = 1;
            }
            else
            {
                length = n;
            }
            buffersize = 0;
            setBufferSize(b);
            RealArray = newdata();
            data = &RealArray[start];
        }
        catch (std::bad_alloc &ba)
        {
            std::cerr << ba.what() << std::endl;
        }
    }

    T *newdata(int length)
    {
        int fullbuffer = (int)(length * buffersize);
        start = fullbuffer;
        reallength = (fullbuffer << 1) + length;
        T *res = new T[reallength];
        return res;
    }
    T *newdata()
    {
        int fullbuffer = (int)(length * buffersize);
        start = fullbuffer;
        reallength = (fullbuffer << 1) + length;
        T *res = new T[reallength];
        return res;
    }
    void arrayswap(T *res)
    {
        delete[] RealArray;
        RealArray = res;
        data = &RealArray[start];
    }

    void CopyArr(T *origin, T *target, int count)
    {
        for (int i = 0; i < count; i++)
        {
            *(target + i) = *(origin + i);
        }
        return;
    }
    void CopyAll(T *origin, int count, int fora)
    {
        for (int i = 0; i < count; i++)
        {
            data[i + fora] = *(origin + i);
        }
        return;
    }
    int _tolimit(int inp) // Array index out of bounds protection
    {
        if (inp < 0)
            return 0;
        if (inp >= length)
            return length - 1;
        return inp;
    }
    int _tolimit_excluding(int inp)
    {
        if (inp < 0)
            return 0;
        if (inp > length)
            return length;
        return inp;
    }
    void MoveArr(int partstart, int dist, int partlength)
    {
        if (dist > 0)
        {
            for (int i = partstart + partlength; i >= partstart; i--)
            {
                if ((i >= 0) && (i < length))
                {
                    if ((i + dist >= 0) && (i + dist < length))
                    {
                        data[i + dist] = data[i];
                    }
                }
            }
        }
        else
        {
            for (int i = partstart; i < partstart + partlength; i++)
            {
                if ((i >= 0) && (i < length))
                {
                    if ((i + dist >= 0) && (i + dist < length))
                    {
                        data[i + dist] = data[i];
                    }
                }
            }
        }
    }
    int checkbufferexcess()
    {
        if (reallength > length * (1 + 3 * buffersize)) //1,5 - maximum array size
            return 1;
        return 0;
    }
    int checkbufferexcess(int sub)
    {
        float persent = (float)(length - sub) / reallength;
        if ((1 - persent) > buffersize * 3)
            return 1;
        return 0;
    }

  public:
    //===================================constructors=================================
    DArray(int n)
    {
        RealArray = NULL;
        create(n);
    }

    DArray(int n, float buf)
    {
        RealArray = NULL;
        create(n, buf);
    }
    DArray()
    {
        RealArray = NULL;
        create(1, 0);
    }
    DArray(DArray<T> *origin)
    {
        try
        {
            buffersize = origin->getBufferSize();
            length = origin->getLength();
            RealArray = NULL;
            create(length, buffersize);
            CopyArr(&(origin->operator[](0)), &data[0], length);
        }
        catch (std::bad_alloc &ba)
        {
            DArray();
            std::cerr << ba.what() << std::endl;
        }
    }
    ~DArray()
    {
        delete[] RealArray;
    }
    //================================================operators===============================
    T &operator[](int i)
    {
        i = _tolimit(i);
        return data[i];
    }
    DArray *operator+(T a)
    {
        DArray<T> *res(new DArray(length + 1));
        res->CopyAll(data, length, 0);
        res->operator[](length) = a;
        return res;
    }
    DArray *operator+(DArray<T> *value)
    {

        DArray<T> *res(new DArray(this));
        res->insert(value, length);
        return res;
    }
    //==============================================geters&seters=================
    int getMemory()
    {
        int i = sizeof(T) * reallength;
        return i;
    }
    int getLength()
    {
        return length;
    }
    float getBufferSize()
    {
        return buffersize * 100;
    }
    int getBufferLeft()
    {
        return (start);
    }
    int getBufferRight()
    {
        return (reallength - start - length);
    }
    void setBufferSize(float in)
    {
        if (in >= 0)
        {
            buffersize = in / 100;
        }
    }
    //====================================insertion=====================
    DArray &PushStart(T a)
    {
        try
        {
            length++;
            if (getBufferLeft() > 0)
            {
                start--;
                data = &RealArray[start];
            }
            else
            {
                T *res = newdata();
                CopyArr(&data[0], &res[start + 1], length - 1);
                arrayswap(res);
            }
            data[0] = a;
        }
        catch (std::bad_alloc &ba)
        {
            std::cerr << ba.what() << std::endl;
        }

        return *this;
    }

    DArray &PushEnd(T a)
    {
        try
        {
            length++;
            if (getBufferRight() <= -1)
            {
                T *res = newdata();
                CopyArr(&data[0], &res[start], length - 1);
                arrayswap(res);
            }
            data[length - 1] = a;
        }
        catch (std::bad_alloc &ba)
        {
            std::cerr << ba.what() << std::endl;
        }

        return *this;
    }

    DArray &insert(T value, int position)
    {
        position = _tolimit_excluding(position);

        try
        {
            int movesize = 0;
            int movestart = 0;
            int movepartsize = 0;
            if ((position - 0 <= length - position) && getBufferLeft())
            {
                start--;
                data = &RealArray[start];
                movesize = -1;
                movestart = 0;
                movepartsize = position + 1;
            }

            else if (getBufferRight())
            {
                movesize = 1;
                movestart = position - 1;
                movepartsize = length - position + 1;
            }

            else
            {
                T *res = newdata(length + 1);
                CopyArr(&data[0], &res[start], position);
                CopyArr(&data[position], &res[start + position + 1], length - position);
                res[position + start] = value;
                arrayswap(res);
                length++;
                return *this;
            }
            length++;
            MoveArr(movestart, movesize, movepartsize);
            data[position] = value;
        }
        catch (std::bad_alloc &ba)
        {
            std::cerr << ba.what() << std::endl;
        }

        return *this;
    }
    DArray &insert(DArray<T> *value, int position)
    {
        int len2 = value->getLength();
        position = _tolimit_excluding(position);

        try
        {
            int movesize = 0;
            int movestart = 0;
            int movepartsize = 0;
            if ((position <= length - position) && (getBufferLeft() >= len2))
            {
                start -= len2;
                data = &RealArray[start];
                movesize = -len2;
                movestart = 0;
                movepartsize = position + len2;
            }

            else if (getBufferRight() >= len2)
            {
                movesize = len2;
                movestart = position;
                movepartsize = length - position;
            }

            else
            {
                length += len2;
                T *res = newdata();
                CopyArr(&data[0], &res[start], position);
                CopyArr(&(value->operator[](0)), &res[position + start], len2);
                CopyArr(&data[position], &res[start + position + len2], length - len2 - position);
                arrayswap(res);

                return *this;
            }
            length += len2;
            MoveArr(movestart, movesize, movepartsize);
            CopyArr(&(value->operator[](0)), &data[position], len2);
        }
        catch (std::bad_alloc &ba)
        {
            std::cerr << ba.what() << std::endl;
        }
        return *this;
    }

    //=====================================extraction===================

    T PopEnd()
    {
        T a;
        if (length < 2)
        {
            a = data[0];
            create(0);
        }
        else
        {
            length--;
            a = data[length];
            if (checkbufferexcess())
            {
                T *res = newdata();
                CopyArr(&data[0], &res[start], length);
                arrayswap(res);
            }
        }
        return a;
    }
    T PopStart()
    {
        T a = data[0];
        if (length < 2)
        {

            create(0);
        }
        else
        {
            length--;
            data = &data[1];
            if (checkbufferexcess())
            {
                T *res = newdata();
                CopyArr(data, &res[start], length);
                arrayswap(res);
            }
        }
        return a;
    }

    T extract(int position)
    {
        position = _tolimit(position);
        T a = data[position];
        if (!checkbufferexcess())
        {
            int movesize = 0;
            int movestart = 0;
            int movepartsize = 0;
            if (position <= length - position)
            {
                movesize = 1;
                movestart = 0;
                movepartsize = position - 1;
            }
            else
            {
                movesize = -1;
                movestart = position + 1;
                movepartsize = length - position - 1;
            }
            MoveArr(movestart, movesize, movepartsize);
            if (movesize > 0)
            {
                data = &data[1];
            }
            length--;
        }
        else
        {
            try
            {
                T *res = newdata(length - 1);
                CopyArr(&data[0], &res[start], position);
                CopyArr(&data[position], &res[start + position], length - position);
                arrayswap(res);
            }
            catch (std::bad_alloc &ba)
            {
                std::cerr << ba.what() << std::endl;
            }
        }

        return a;
    }
    DArray *extract(int position, int count)
    {
        position = _tolimit(position);
        if (count < 0)
            count = 0;
        if (count + position >= length)
            count = length - position;
        DArray<T> *a(new DArray(count));
        a->CopyAll(&data[position], count, 0);
        if (!checkbufferexcess(count))
        {
            int movesize = 0;
            int movestart = 0;
            int movepartsize = 0;
            if (position <= length - position)
            {
                movesize = count;
                movestart = 0;
                movepartsize = position - 1;
            }
            else
            {
                movesize = -count;
                movestart = position + count;
                movepartsize = length - position - count;
            }

            MoveArr(movestart, movesize, movepartsize);

            if (movesize > 0)
            {
                data = &data[count];
            }
        }
        else
        {

            try
            {
                T *res = newdata(length - count);
                CopyArr(&data[0], &res[start], position);
                CopyArr(&data[position + count], &res[start + position], length - position - count);
                arrayswap(res);
            }
            catch (std::bad_alloc &ba)
            {
                std::cerr << ba.what() << std::endl;
            }
        }
        length -= count;
        return a;
    }
    //========================test=functions==================================
    DArray &Invert()
    {
        T a;
        for (int i = 0; i < length / 2; i++)
        {
            a = data[i];
            data[i] = data[length - 1 - i];
            data[length - i - 1] = a;
        }
        return *this;
    }
};