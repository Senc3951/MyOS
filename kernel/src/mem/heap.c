#include <mem/heap.h>
#include <mem/vmm.h>
#include <arch/spinlock.h>
#include <panic.h>
#include <libc/string.h>
#include <log.h>

#define xstr(a) str(a)
#define str(a)  #a

#define LIBALLOC_MAGIC  0xc001c0de
#define LIBALLOC_DEAD   0xdeaddead
//#define DEBUG_HEAP

#define ALIGN_SIZE  sizeof(uintptr_t)
#define ALIGN_TYPE  char
#define ALIGN_INFO  sizeof(ALIGN_TYPE) * 16

#define USE_CASE1
#define USE_CASE2
#define USE_CASE3
#define USE_CASE4
#define USE_CASE5

#define ALIGN( ptr, alignment )                                 \
if ( alignment > 1 )											\
{																\
    uintptr_t diff;												\
    ptr = (void*)((uintptr_t)ptr + ALIGN_INFO);					\
    diff = (uintptr_t)ptr & (alignment-1);						\
    if ( diff != 0 )											\
    {															\
        diff = alignment - diff;								\
        ptr = (void*)((uintptr_t)ptr + diff);					\
    }															\
    *((ALIGN_TYPE*)((uintptr_t)ptr - ALIGN_INFO)) = 			\
        diff + ALIGN_INFO;										\
}															

#define UNALIGN( ptr, alignment )                               \
if ( alignment > 1 )											\
{																\
    uintptr_t diff = *((ALIGN_TYPE*)((uintptr_t)ptr - ALIGN_INFO));	\
    if ( diff < (alignment + ALIGN_INFO) )						\
    {															\
        ptr = (void*)((uintptr_t)ptr - diff);					\
    }															\
}

typedef struct liballoc_major
{
    struct liballoc_major *prev;
    struct liballoc_major *next;
    unsigned int pages;
    unsigned int size;
    unsigned int usage;
    struct liballoc_minor *first;
} liballoc_major;

typedef struct liballoc_minor
{
    struct liballoc_minor *prev;
    struct liballoc_minor *next;
    struct liballoc_major *block;
    unsigned int magic;
    unsigned int size;
    unsigned int req_size;
} liballoc_minor;

static liballoc_major *g_memRoot = NULL;
static liballoc_major *g_bestBet = NULL;
static spinlock_t g_lock = SPINLOCK_INLINE_INIT();

static uint32_t g_pageSize = PAGE_SIZE, g_pageCount = 1;
static size_t g_allocated = 0, g_inuse = 0;    
static size_t g_warningCount = 0, g_errorCount = 0, g_possibleOverruns = 0;

static inline int heap_lock()
{
    spinlock_acquire(&g_lock);
    return 0;
}

static inline int heap_unlock()
{
    spinlock_release(&g_lock);
    return 0;
}

static inline void *heap_alloc(size_t pc)
{
    return pt_alloc_pages(get_kernel_pd(), pc, PAGING_READ_WRITE);
}

static inline int heap_free(void *ptr, size_t pc)
{
    pt_free_pages(get_kernel_pd(), ptr, pc);
    return 0;
}

static void liballoc_dump()
{
    #ifndef DEBUG_HEAP
        return;
    #endif

    liballoc_major *maj = g_memRoot;
    liballoc_minor *min = NULL;
    
    LOG("------ Memory data ---------------\n");
    LOG("System memory allocated: %i bytes\n", g_allocated );
    LOG("Memory in used (malloc'ed): %i bytes\n", g_inuse );
    LOG("Warning count: %i\n", g_warningCount );
    LOG("Error count: %i\n", g_errorCount );
    LOG("Possible overruns: %i\n", g_possibleOverruns );
    
    while ( maj != NULL )
    {
        LOG("%p: total = %i, used = %i\n",
                    (uintptr_t)maj, 
                    maj->size,
                    maj->usage );

        min = maj->first;
        while ( min != NULL )
        {
            LOG("   %p: %i bytes\n",
                        (uintptr_t)min, 
                        min->size );
            min = min->next;
        }

        maj = maj->next;
    }
}

static liballoc_major *allocate_new_page( unsigned int size )
{
    unsigned int st;
    liballoc_major *maj;

    // This is how much space is required.
    st = size + sizeof(liballoc_major);
    st += sizeof(liballoc_minor);

            // Perfect amount of space?
    if ((st % g_pageSize) == 0)
        st = st / (g_pageSize);
    else
        st = st / (g_pageSize) + 1;
                        // No, add the buffer. 

    
    // Make sure it's >= the minimum size.
    if (st < g_pageCount)
        st = g_pageCount;
    
    maj = (liballoc_major *)heap_alloc(st);
    if (maj == NULL)
    {
        g_warningCount += 1;
        #ifdef DEBUG_HEAP
        LOG("WARNING: liballoc_alloc( %i ) return NULL\n", st );
        #endif
        return NULL;	// uh oh, we ran out of memory.
    }
        
    maj->prev = NULL;
    maj->next = NULL;
    maj->pages = st;
    maj->size = st * g_pageSize;
    maj->usage = sizeof(liballoc_major);
    maj->first = NULL;
    g_allocated += maj->size;
    
    #ifdef DEBUG_HEAP
    LOG("Resource allocated %p of %i pages (%i bytes) for %i size.\n", (uintptr_t)maj, st, maj->size, size );
    LOG("Total memory usage = %i KB\n", (int)((g_allocated / (1024))));
    #endif
    
    return maj;
}

__MALLOC__ void *kmalloc(size_t rsize)
{
    int startedBet = 0;
    unsigned long long bestSize = 0;
    void *p = NULL;
    uintptr_t diff;
    liballoc_major *maj;
    liballoc_minor *min;
    liballoc_minor *new_min;
    unsigned long size = rsize;
    
    // For alignment, we adjust size so there's enough space to align.
    if (ALIGN_SIZE > 1)
    {
        size += ALIGN_SIZE + ALIGN_INFO;
    }
                // So, ideally, we really want an alignment of 0 or 1 in order
                // to save space.
    
    heap_lock();
    if (!size)
    {
        g_warningCount += 1;
        #ifdef DEBUG_HEAP
        LOG("WARNING: alloc( 0 ) called from %x\n",
                            __builtin_return_address(0) );
        #endif
        heap_unlock();
        return NULL;
    }

    if (g_memRoot == NULL)
    {
        #ifdef DEBUG_HEAP
        LOG("initialization of liballoc\n" );
        #endif
        liballoc_dump();
            
        // This is the first time we are being used.
        g_memRoot = allocate_new_page( size );
        if ( g_memRoot == NULL )
        {
        heap_unlock();
        #ifdef DEBUG_HEAP
        LOG("initial l_memRoot initialization failed\n"); 
        #endif
        return NULL;
        }

        #ifdef DEBUG_HEAP
        LOG("set up first memory major %x\n", (uintptr_t)g_memRoot );
        #endif
    }

    // Now we need to bounce through every major and find enough space....

    maj = g_memRoot;
    startedBet = 0;
    
    // Start at the best bet....
    if ( g_bestBet != NULL )
    {
        bestSize = g_bestBet->size - g_bestBet->usage;

        if ( bestSize > (size + sizeof(liballoc_minor)))
        {
            maj = g_bestBet;
            startedBet = 1;
        }
    }
    
    while ( maj != NULL )
    {
        diff  = maj->size - maj->usage;	
                                        // free memory in the block

        if ( bestSize < diff )
        {
            // Hmm.. this one has more memory then our bestBet. Remember!
            g_bestBet = maj;
            bestSize = diff;
        }
        
        
#ifdef USE_CASE1
            
        // CASE 1:  There is not enough space in this major block.
        if ( diff < (size + sizeof( liballoc_minor )) )
        {
            #ifdef DEBUG_HEAP
            LOG("CASE 1: Insufficient space in block %x\n", (uintptr_t)maj);
            #endif
                
                // Another major block next to this one?
            if ( maj->next != NULL ) 
            {
                maj = maj->next;		// Hop to that one.
                continue;
            }

            if ( startedBet == 1 )		// If we started at the best bet,
            {							// let's start all over again.
                maj = g_memRoot;
                startedBet = 0;
                continue;
            }

            // Create a new major block next to this one and...
            maj->next = allocate_new_page( size );	// next one will be okay.
            if ( maj->next == NULL ) break;			// no more memory.
            maj->next->prev = maj;
            maj = maj->next;

            // .. fall through to CASE 2 ..
        }

#endif

#ifdef USE_CASE2
        
        // CASE 2: It's a brand new block.
        if ( maj->first == NULL )
        {
            maj->first = (liballoc_minor*)((uintptr_t)maj + sizeof(liballoc_major) );

            
            maj->first->magic 		= LIBALLOC_MAGIC;
            maj->first->prev 		= NULL;
            maj->first->next 		= NULL;
            maj->first->block 		= maj;
            maj->first->size 		= size;
            maj->first->req_size 	= rsize;
            maj->usage 	+= size + sizeof( liballoc_minor );


            g_inuse += size;
            
            
            p = (void*)((uintptr_t)(maj->first) + sizeof( liballoc_minor ));

            ALIGN( p, ALIGN_SIZE );
            
            #ifdef DEBUG_HEAP
            LOG("CASE 2: returning %x\n", (uintptr_t)p); 
            #endif
            heap_unlock();		// release the lock
            return p;
        }

#endif
                
#ifdef USE_CASE3

        // CASE 3: Block in use and enough space at the start of the block.
        diff =  (uintptr_t)(maj->first);
        diff -= (uintptr_t)maj;
        diff -= sizeof(liballoc_major);

        if ( diff >= (size + sizeof(liballoc_minor)) )
        {
            // Yes, space in front. Squeeze in.
            maj->first->prev = (liballoc_minor*)((uintptr_t)maj + sizeof(liballoc_major) );
            maj->first->prev->next = maj->first;
            maj->first = maj->first->prev;
                
            maj->first->magic 	= LIBALLOC_MAGIC;
            maj->first->prev 	= NULL;
            maj->first->block 	= maj;
            maj->first->size 	= size;
            maj->first->req_size 	= rsize;
            maj->usage 			+= size + sizeof( liballoc_minor );

            g_inuse += size;

            p = (void*)((uintptr_t)(maj->first) + sizeof( liballoc_minor ));
            ALIGN( p, ALIGN_SIZE );

            #ifdef DEBUG_HEAP
            LOG("CASE 3: returning %x\n", (uintptr_t)p); 
            #endif
            heap_unlock();		// release the lock
            return p;
        }
        
#endif


#ifdef USE_CASE4

        // CASE 4: There is enough space in this block. But is it contiguous?
        min = maj->first;
        
            // Looping within the block now...
        while ( min != NULL )
        {
                // CASE 4.1: End of minors in a block. Space from last and end?
                if ( min->next == NULL )
                {
                    // the rest of this block is free...  is it big enough?
                    diff = (uintptr_t)(maj) + maj->size;
                    diff -= (uintptr_t)min;
                    diff -= sizeof( liballoc_minor );
                    diff -= min->size; 
                        // minus already existing usage..

                    if ( diff >= (size + sizeof( liballoc_minor )) )
                    {
                        // yay....
                        min->next = (liballoc_minor*)((uintptr_t)min + sizeof( liballoc_minor ) + min->size);
                        min->next->prev = min;
                        min = min->next;
                        min->next = NULL;
                        min->magic = LIBALLOC_MAGIC;
                        min->block = maj;
                        min->size = size;
                        min->req_size = rsize;
                        maj->usage += size + sizeof( liballoc_minor );

                        g_inuse += size;
                        
                        p = (void*)((uintptr_t)min + sizeof( liballoc_minor ));
                        ALIGN( p, ALIGN_SIZE );

                        #ifdef DEBUG_HEAP
                        LOG("CASE 4.1: returning %x\n", (uintptr_t)p); 
                        #endif
                        heap_unlock();		// release the lock
                        return p;
                    }
                }



                // CASE 4.2: Is there space between two minors?
                if ( min->next != NULL )
                {
                    // is the difference between here and next big enough?
                    diff  = (uintptr_t)(min->next);
                    diff -= (uintptr_t)min;
                    diff -= sizeof( liballoc_minor );
                    diff -= min->size;
                                        // minus our existing usage.

                    if ( diff >= (size + sizeof( liballoc_minor )) )
                    {
                        // yay......
                        new_min = (liballoc_minor*)((uintptr_t)min + sizeof( liballoc_minor ) + min->size);
                        
                        new_min->magic = LIBALLOC_MAGIC;
                        new_min->next = min->next;
                        new_min->prev = min;
                        new_min->size = size;
                        new_min->req_size = rsize;
                        new_min->block = maj;
                        min->next->prev = new_min;
                        min->next = new_min;
                        maj->usage += size + sizeof( liballoc_minor );
                        
                        g_inuse += size;
                        
                        p = (void*)((uintptr_t)new_min + sizeof( liballoc_minor ));
                        ALIGN( p, ALIGN_SIZE );


                        #ifdef DEBUG_HEAP
                        LOG("CASE 4.2: returning %x\n", (uintptr_t)p); 
                        #endif
                        
                        heap_unlock();		// release the lock
                        return p;
                    }
                }	// min->next != NULL

                min = min->next;
        } // while min != NULL ...


#endif

#ifdef USE_CASE5

        // CASE 5: Block full! Ensure next block and loop.
        if ( maj->next == NULL ) 
        {
            #ifdef DEBUG_HEAP
            LOG("CASE 5: block full\n");
            #endif

            if ( startedBet == 1 )
            {
                maj = g_memRoot;
                startedBet = 0;
                continue;
            }
                
            // we've run out. we need more...
            maj->next = allocate_new_page( size );		// next one guaranteed to be okay
            if ( maj->next == NULL ) break;			//  uh oh,  no more memory.....
            maj->next->prev = maj;

        }

#endif

        maj = maj->next;
    } // while (maj != NULL)


    
    heap_unlock();		// release the lock

    #ifdef DEBUG_HEAP
    LOG("All cases exhausted. No memory available.\n");
    #endif
    #ifdef DEBUG_HEAP
    LOG("WARNING: malloc( %i ) returning NULL.\n", size);
    liballoc_dump();
    #endif
    return NULL;
}

__MALLOC__ void *kcalloc(size_t nobj, size_t size)
{
    size_t realSize = nobj * size;
    void *p = kmalloc(realSize);

    memset(p, 0, realSize);
    return p;
}

__MALLOC__ void *krealloc(void *p, size_t size)
{
    void *ptr;
    liballoc_minor *min;
    unsigned int realSize;
    
    // Honour the case of size == 0 => free old and return NULL
    if (!size)
    {
        kfree(p);
        return NULL;
    }

    // In the case of a null pointer, return a simple malloc.
    if (p == NULL)
        return kmalloc(size);
    
    // Unalign the pointer if required.
    ptr = p;
    UNALIGN(ptr, ALIGN_SIZE);
    
    heap_lock();
    min = (liballoc_minor *)((uintptr_t)ptr - sizeof(liballoc_minor));
    
    // Ensure it is a valid structure.
    if (min->magic != LIBALLOC_MAGIC)
    {
        g_errorCount += 1;

        // Check for overrun errors. For all bytes of LIBALLOC_MAGIC 
        if ( 
            ((min->magic & 0xFFFFFF) == (LIBALLOC_MAGIC & 0xFFFFFF)) || 
            ((min->magic & 0xFFFF) == (LIBALLOC_MAGIC & 0xFFFF)) || 
            ((min->magic & 0xFF) == (LIBALLOC_MAGIC & 0xFF)) 
        )
        {
            g_possibleOverruns += 1;
            #ifdef DEBUG_HEAP
            LOG("ERROR: Possible 1-3 byte overrun for magic %x != %x\n",
                                min->magic,
                                LIBALLOC_MAGIC );
            #endif
            panic("(LIBALLOC) Kernel memory error!");
        }                            
        if (min->magic == LIBALLOC_DEAD)
        {
            panic("(LIBALLOC) Kernel memory error!");
        }
        else
        {
            panic("(LIBALLOC) Kernel memory error!");
        }
                    
        // being lied to...
        heap_unlock();		// release the lock
        return NULL;
    }	
    
    // Definitely a memory block.
    realSize = min->req_size;
    if (realSize >= size) 
    {
        min->req_size = size;
        heap_unlock();

        return p;
    }

    heap_unlock();

    // If we got here then we're reallocating to a block bigger than us.
    ptr = kmalloc(size);					// We need to allocate new memory
    memcpy(ptr, p, realSize);
    
    kfree(p);        
    return ptr;
}

void kfree(void *ptr)
{
    liballoc_minor *min;
    liballoc_major *maj;

    if (ptr == NULL)
    {
        g_warningCount += 1;
        #ifdef DEBUG_HEAP
        LOG("WARNING: free( NULL ) called from %x\n",
                            __builtin_return_address(0) );
        #endif
        return;
    }
    
    UNALIGN(ptr, ALIGN_SIZE);
    heap_lock();

    min = (liballoc_minor *)((uintptr_t)ptr - sizeof(liballoc_minor));
    if (min->magic != LIBALLOC_MAGIC) 
    {
        g_errorCount += 1;

        // Check for overrun errors. For all bytes of LIBALLOC_MAGIC 
        if ( 
            ((min->magic & 0xFFFFFF) == (LIBALLOC_MAGIC & 0xFFFFFF)) || 
            ((min->magic & 0xFFFF) == (LIBALLOC_MAGIC & 0xFFFF)) || 
            ((min->magic & 0xFF) == (LIBALLOC_MAGIC & 0xFF)) 
        )
        {
            g_possibleOverruns += 1;
            #ifdef DEBUG_HEAP
            LOG("ERROR: Possible 1-3 byte overrun for magic %x != %x\n",
                                min->magic,
                                LIBALLOC_MAGIC );
            #endif
            panic("(LIBALLOC) Kernel memory error!");
        }
                        
                        
        if (min->magic == LIBALLOC_DEAD)
        {
            #ifdef DEBUG_HEAP
            LOG("ERROR: multiple " xstr(free) "() attempt on %x from %x.\n", 
                                    ptr,
                                    __builtin_return_address(0) );
            #endif
            panic("(LIBALLOC) Kernel memory error!");
        }
        else
        {
            panic("(LIBALLOC) Kernel memory error!");
        }
            
        // being lied to...
        heap_unlock();		// release the lock
        return;
    }

    maj = min->block;
    g_inuse -= min->size;
    maj->usage -= (min->size + sizeof( liballoc_minor ));
    min->magic = LIBALLOC_DEAD;		// No mojo.

    if (min->next != NULL)
        min->next->prev = min->prev;
    if (min->prev != NULL)
        min->prev->next = min->next;
    if (min->prev == NULL)
        maj->first = min->next;	
                        // Might empty the block. This was the first
                        // minor.

    // We need to clean up after the majors now....

    if (maj->first == NULL)	// Block completely unused.
    {
        if (g_memRoot == maj)
            g_memRoot = maj->next;
        if (g_bestBet == maj)
            g_bestBet = NULL;
        if (maj->prev != NULL)
            maj->prev->next = maj->next;
        if (maj->next != NULL)
            maj->next->prev = maj->prev;
        
        g_allocated -= maj->size;
        heap_free(maj, maj->pages);
    }
    else
    {
        if (g_bestBet != NULL)
        {
            int bestSize = g_bestBet->size - g_bestBet->usage;
            int majSize = maj->size - maj->usage;
            if (majSize > bestSize)
                g_bestBet = maj;
        }
    }
    
    #ifdef DEBUG_HEAP
    LOG("OK\n");
    #endif
    
    heap_unlock();		// release the lock
}