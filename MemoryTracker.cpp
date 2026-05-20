#include <iostream>
#include <cstdlib>
#include <stdio.h>

// عدادات برمجية لمراقبة الحالة حركياً
static size_t global_allocated_memory = 0;
static size_t global_allocation_count = 0;

// إحلال دالة new العالمية
void* operator new(size_t size) throw(std::bad_alloc) {
    global_allocated_memory += size;
    global_allocation_count++;
    
    // طباعة تفاصيل العملية باستخدام استدعاءات منخفضة المستوى لتجنب الحجز الداخلي
    // تنبيه: لا تستخدم std::cout أو std::string هنا لأنها قد تقوم بعمليات new داخلية وتسبب Infinite Loop
    printf("[ALLOC] Size: %lu bytes | Total Allocated: %lu bytes | Count: %lu\n", 
                size, global_allocated_memory, global_allocation_count);

    void* ptr = std::malloc(size);
    if (!ptr) throw std::bad_alloc();
    return ptr;
}

// إحلال دالة delete العالمية
void operator delete(void* ptr) throw() {
    if (!ptr) return;
    
    // في C++98 القياسية، دالة delete العالمية لا تستقبل الحجم، 
    /`/ لذا نقوم فقط بتقليص عدد العمليات النشطة أو تتبع المؤشر إن لزم الأمر.
    if (global_allocation_count > 0) {
        global_allocation_count--;
    }
    printf("[FREE] Active Allocations Remaining: %lu\n", global_allocation_count);
    std::free(ptr);
}