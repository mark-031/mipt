/*!
    @file

    Заголовочный файл stack.h
*/

#include<assert.h>
#include<stdlib.h>
#include<stdio.h>

#ifdef STACKDEBUG
#define $ondebug if(1)
#define $debugcode(code) code

#define STACKASSERT(stack)    \
if (StackOk(stack) != OK)     \
{                             \
    assert(!StackDump(stack));\
}

#else
#define $ondebug if(0)
#define $debugcode(code)

#define STACKASSERT(stack)    \
assert(stack)

#endif


typedef int canary_t;
#define CANARY_T "%d"
typedef int stackunit_t;
#define STACKUNIT_T "%d"
typedef int hash_t;
#define HASH_T "%d"

const size_t MINSTACKSIZE = 1;

$debugcode(
    const canary_t CANARY = 0x48151623;
    const stackunit_t POISON = -815;
);

enum StackErrNo
{
    OK,
    BadCounter,
    BadPoison,
    BadCanary,
    BadHash
};

/*!
    Структура стека
*/

struct stack_t
{
    $debugcode(
        hash_t   hash;
        canary_t canary;
    );                               ///< hash_t hash - Хэш для дебага\n canary_t canary - Канарейка для дебага

    char*        name;               ///< Имя стека
    size_t       size;               ///< Текущий размер
    size_t       counter;            ///< Количество элементов в буфере
    int          state;              ///< Состояние буфера
    stackunit_t  data[MINSTACKSIZE]; ///< Буфер
};

/*!
    Инциализирует стек
    @param self Стек
    @param size Размер стека
    @return Код ошибки
*/
int       StackInit(stack_t** self, size_t size, char* name);

/*!
    Освобождает стек
    @param self Стек
    @return Код ошибки
*/
int       StackFinilize(stack_t* self);

/*!
    Проверяет стек на валидность
    @param self Стек
    @return Код ошибки
*/
int       StackOk(stack_t* self);

/*!
    Достает верхний элемент стека
    @param self Стек
    @param item Указатель на блок памяти, куда записывается верхний элемент стека
    @return Код ошибки
*/
int       StackPop(stack_t* self, stackunit_t* item);

/*!
    Заносит в стек новый элемент
    @param self Стек
    @param item Новый элемент стека
    @return Код ошибки
*/
int       StackPush(stack_t* self, stackunit_t item);

/*!
    Изменяет размер стека
    @param self Указатель на стек
    @param newSize Новый размер стека
    @return Код ошибки
*/
int       StackRealloc(stack_t** self, size_t newSize);

/*!
    Выводит дамп в стандартный поток
    @param self Стек
    @return Состояние стека
*/
int       StackDump(stack_t* self);

/*!
    Вычисляет количество памяти занимаемой стеком
    @param dataSize Размер стека
    @return Количество памяти в байтах
*/
size_t   StackSizeCalc(size_t dataSize);

/*!
    Вычисляет хэш стека
    @param self Стек
    @return Хэш
*/
hash_t   StackHashCalc(const stack_t* self);