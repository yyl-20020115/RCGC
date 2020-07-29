[2020-07-30-02:20:15] By Yilin Yang
The algorithm of RCGC

When we use Reference-Counting algorithm such as shared_ptr, the circular reference issue
is just a big headache. So we invented weak_ptr, however, it's not so convient as we almost
use shared_ptr everywhere.
When we use Garbage-Collection algorithm such as GC in Java/C#, the STOP-THE-WORLD issue
is just another big headache. So we invented Generation algorithm, however, you still have to 
stop something.

Is it possible for C++ to use GC algorithms? Or is it possible for C# to use Reference-Counting
algorithms?

Here is the algorithm(It has to be native if we code this algorithm for C#. Therefore we use C++).

How does this algorithm work?
we designed rcgc_shared_ptr which is quite like shared_ptr for C++. But we don't acutally free
_ptr pointed objects instantly when the reference count is zero. We do not call delete or free(ptr)
function while decreasing the count of the reference. However, we let the rcgc_shared_ptr does its work
on every level of decreasing count, without delete them.

When we actually delete something (finally by calling free(ptr)), we just ignore rcgc_shared_ptrs, so that
they just look like plain data. 

And when things are ready, we check the refs cache, examing every pointer (to memory) to see which one has 
a reference count of 0. If found, we just delete it.

This part is just like GC algorithm. But there is a big bonus: we can just copy out the refs cache which
takes much less time, and start a new thread working on the copied cache. A dedicated thread would be working
on the copied cache even without interferring other working threads because freeing memory blocks can be 
done simutanously: the pointers with zero reference count can no longer be accessed, therefore they can be 
deleted safely in another thread (dedicated gc thread).

This will be much faster then GC mark-and-swap or generation method, also much easier than using weak_ptr.
The total memory used in this algorithm is just like shared_ptr, no more. 
And the speed is much faster than conventional GC.

I didn't see fatal design defact for now. However, due to limited time, I haven't done enough test
for performance or usibility efficiency.

Ideas are all welcomed, and please write me:
yyl_20050115@hotmail.com

Best Regards,
Yilin Yang
