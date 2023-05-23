#include <string.h>
#include <mem/pmm.h>
#include <mem/heap.h>

typedef long Align;

union HEADER
{
    struct
    {
        union HEADER *next;
        uint32_t size;
    } s;
    Align x;
};

static union HEADER base;
static union HEADER *freep = NULL;

static void *morecore(const size_t nunits)
{
    size_t nu = nunits < PAGE_SIZE ? PAGE_SIZE : nunits;
    char *cp = (char *)pmm_alloc(nu * sizeof(union HEADER));
    if (cp == NULL)
        return NULL;

    union HEADER *up = (union HEADER *)cp;
    up->s.size = nu;
    kfree((void *)(up + 1));
    
    return freep;
}

void *kmalloc(const size_t size)
{
    union HEADER *p, *prevp;
    size_t nunits = (size + sizeof(union HEADER) - 1) / sizeof(union HEADER) + 1;

    if ((prevp = freep) == NULL)
    {
        base.s.next = freep = prevp = &base;
        base.s.size = 0;
    }
    for (p = prevp->s.next; ; prevp = p, p = p->s.next)
    {
        if (p->s.size >= nunits)
        {
            if (p->s.size == nunits)
                prevp->s.next = p->s.next;
            else
            {
                p->s.size -= nunits;
                p += p->s.size;
                p->s.size = nunits;
            }

            freep = prevp;
            return (void *)(p + 1);
        }
        if (p == freep)
        {
            if ((p = morecore(nunits)) == NULL)
                return NULL;
        }
    }
    
    return NULL;
}

void *kcalloc(const size_t size)
{
    void *addr = kmalloc(size);
    if (addr == NULL)
        return NULL;

    memset(addr, 0, size);
    return addr;
}

void kfree(void *addr)
{
    union HEADER *p, *bp = (union HEADER *)addr - 1;
    for (p = freep; !(bp > p && bp < p->s.next); p = p->s.next)
    {
        if (p >= p->s.next && (bp > p || bp < p->s.next))
            break;
    }

    if (bp + bp->s.size == p->s.next)
    {
        bp->s.size += p->s.next->s.size;
        bp->s.next = p->s.next->s.next;
    }
    else
        bp->s.next = p->s.next;

    if (p + p->s.size == bp)
    {
        p->s.size += bp->s.size;
        p->s.next = bp->s.next;
    }
    else
        p->s.next = bp;
    
    freep = p;
    addr = NULL;
}