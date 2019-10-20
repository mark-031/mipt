#include"stack.h"

int StackInit(stack_t** self, size_t size, char* name)
{
    size_t gb = 1;
    gb <<= 31;

    if (size < 1 || size * sizeof(stackunit_t) > gb)
        return 1;

    size_t dataSize = _StackSizeCalc(size);
    assert(dataSize > 0);
    
    *self = (stack_t*) calloc(1, dataSize);
    if (*self == nullptr)
        return 2;

    (*self)->name    = name;
    (*self)->size    = size;
    (*self)->counter = 0;
    (*self)->state   = OK;
    
    $debugcode(
        (*self)->canary  = CANARY;
        
        for(size_t i = 0; i < size; ++i)
            (*self)->data[i] = POISON;
            

        *(canary_t*) &(*self)->data[size] = CANARY;

        (*self)->hash = _StackHashCalc(*self);
    );
    
    return 0;
}
#define StackInit(name, size) StackInit(&name, size, #name)


int StackFinilize(stack_t** self)
{
    STACKASSERT(*self);

    if (StackOk(*self) != OK)
        return 1;

    free(*self);
    *self = nullptr;

    return 0;
}

int StackOk(stack_t* self)
{
    assert(self);
    
    if (self->state != OK)
        ;

    else if (self->counter < 0 || self->counter > self->size)
        self->state = BadCounter;

    $debugcode(
        else if (self->canary != CANARY || self->data[self->size] != CANARY)
            self->state = BadCanary;
        
        else if (self->hash != _StackHashCalc(self))
            self->state = BadHash;

        for(size_t i = self->counter; i < self->size; ++i)
        {
            if (self->data[i] != POISON)
            {
                self->state = BadPoison;
                break;
            }
        }
    );

    return self->state;
}

int StackPop(stack_t* self, stackunit_t* item)
{
    STACKASSERT(self);

    if (StackOk(self) != OK)
        return 1;

    if (self->counter == 0)
        return 2;

    *item = self->data[--self->counter];

    $debugcode(
        self->data[self->counter] = POISON;
        self->hash = _StackHashCalc(self);
    );

    return 0;
}

int StackPush(stack_t* self, stackunit_t item)
{
    STACKASSERT(self);

    if (StackOk(self) != OK)
        return 1;

    if (self->counter == self->size)
    {
        size_t newSize = self->size + self->size / 2;
        if (StackRealloc(&self, newSize))
            return 2;
    }

    self->data[self->counter++] = item;

    $debugcode(
        self->hash = _StackHashCalc(self);
    );

    return 0;
}

int StackRealloc(stack_t** self, size_t newSize)
{
    STACKASSERT(*self);

    if (StackOk(*self) != OK)
        return 1;

    stack_t* newStack = (stack_t*) realloc(*self, _StackSizeCalc(newSize));

    if (newStack == nullptr)
        return 2;

    if (*self != newStack)
    {
        free(*self);
        *self = newStack;
    }

    $debugcode(
        for(size_t i = (*self)->size; i < newSize; i++)
        {
            (*self)->data[i] = POISON;
        }

        (*self)->data[newSize] = CANARY;
    );

    (*self)->size = newSize;

    $debugcode(
        (*self)->hash = _StackHashCalc(*self);
    );

    assert(StackOk(*self) == OK);

    return 0;
}

int StackDump(stack_t* self)
{
    printf("Stack_t dump \"%s\"\n", self->name);
    printf("{\n");
    
    $debugcode(
        printf("    hash = " HASH_T "\n", self->hash);
        printf("    canary = " CANARY_T "\n", self->canary);
    );

    printf("    size = %lu\n", self->size);
    printf("    counter = %ld\n", self->counter);
    printf("    state = %d\n", self->state);
    printf("    {\n");
    for(size_t i = 0; i < self->size; ++i)
    {
        printf("        data[%ld] = " STACKUNIT_T, i, self->data[i]);

        $debugcode(
        if (i >= self->counter)
            if (self->data[i] == POISON)
                printf(" (POISON)");
            else
                printf(" (CORRUPTED)");
        );

        printf("\n");
    }
    printf("    }\n");

    $debugcode(
        printf("    canary = " CANARY_T "\n", (canary_t) self->data[self->size]);
    );

    printf("}\n");

    return (self->state);
} 

size_t _StackSizeCalc(size_t dataSize)
{
    return sizeof(stack_t) + sizeof(stackunit_t) * (dataSize - MINSTACKSIZE) $debugcode( + sizeof(canary_t));
}

hash_t _StackHashCalc(const stack_t* self)
{
    unsigned char* start = (unsigned char*) self;
    unsigned char* end   = (unsigned char*) (&self->data[self->size]);

    $debugcode(
        start += sizeof(canary_t);
    );

    assert(end > start);

    hash_t hash = *start;
    unsigned char mask = *start;
    ++start;

    while (start < end)
    {
        mask <<= 1;
        mask += hash % 2;
        hash += *start ^ mask;

        start++;
    }

    return hash;
}