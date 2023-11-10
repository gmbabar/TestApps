#!/usr/bin/python3

# [https://realpython.com/async-io-python/]

import asyncio

# "async def" defines a native coroutine/asynchronous generator
# @asyncio.coroutine is another way of defining a native coroutine/async generator
# "await":
#    1. suspends the execution of the coroutine until event expected occurs
#    2. await can only call another coroutine and vice versa

async def count(id):
    print ('count: start', id)
    await asyncio.sleep(1)
    print ('count: end', id)

async def main():
    await asyncio.gather(count(1), count(2), count(3))

if __name__ == '__main__':
    import time
    s1 = time.perf_counter()
    asyncio.run(main())
    s2 = time.perf_counter()
    print (f'main: time elapsed: {s2-s1} seconds')

