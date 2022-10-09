#!/usr/bin/python3

import asyncio

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

