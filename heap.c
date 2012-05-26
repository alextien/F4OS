#include "types.h"
#include "mem.h"
#include "heap.h"

extern uint32_t _skernelheap;
extern uint32_t _ekernelheap;


typedef struct heap_node_struct{
    struct heap_node_struct* next_node;
    uint8_t mem[HEAP_MEM_SIZE];
}heapNode;

#define HEAP_BLOCK_SIZE (HEAP_MEM_SIZE + sizeof(heapNode*)) /* Total size (in bytes) of one heapNode for use in allocing mem. */

typedef struct heap_list_struct{
    heapNode* head;
    heapNode* tail;
}heapList;

static heapList k_heapList = {  NULL ,
                                NULL };

void* kmalloc(int size){
    int needed_blocks = size/(HEAP_BLOCK_SIZE)+ 1;
    heapNode* break_node = k_heapList.head;         /* Node at which we jump to after the break */
    heapNode* ret_node = NULL;
    /* Find which node will be new head, then return old head, which is first mem location in newly alloc'd mem */
    for(int i = 0; i < needed_blocks; i++){
        break_node = break_node->next_node;
    }
    ret_node = k_heapList.head;                     /* Head now points to alloc'd mem. Get ready to return that mem. */
    k_heapList.head = break_node;                   /* Move head forward to break node. Heap has grown upwards. */
    return ret_node;
}

void init_kheap(void){
    k_heapList.head = (heapNode *)&_skernelheap;                /* Set heap to first address in heap area. */
    heapNode* heap_end  = (heapNode *)&_ekernelheap;                /* This is the end of the heap. We don't frack with mem after this address. */
    heapNode* curr_node = k_heapList.head;          /* Pointer to heap block we are setting up. */
    while(curr_node < heap_end){
        curr_node->next_node = curr_node + sizeof(heapNode);
        curr_node = curr_node->next_node;
    }
}

void* malloc_test(void){
    void* stuff = kmalloc(20);
    return stuff;
}