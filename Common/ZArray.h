#pragma once
#include "logger.h"
#include "ZAllocEx.h"

template <typename T>
class ZArray
{
private:
	T* a;

public:
	/// <summary>
	/// Default constructor
	/// </summary>
	ZArray()
	{
		this->a = nullptr;
	}

	/// <summary>
	/// Allocating constructor
	/// </summary>
	/// <param name="uSize">Number of spaces to allocate for in the array</param>
	ZArray(size_t uSize)
	{
		this->a = nullptr;
		this->Alloc(uSize);
	}

	ZArray(ZArray<T>* r)
	{
		this->a = nullptr;
		this = r; // TODO make sure this is using the overloaded operator
	}

	~ZArray()
	{
		this->RemoveAll();
	}

	ZArray<T>* operator=(ZArray<T>* r)
	{
		if (this == r) return this;

		this->RemoveAll();

		size_t uCount = r->GetCount();

		if (uCount)
		{
			/* Allocate new memory for array copy */
			PVOID pAlloc = ZAllocEx<ZAllocAnonSelector>::Alloc(sizeof(T) * uCount + sizeof(T));

			/* Assign array count to head address */
			*(DWORD*)pAlloc = uCount;

			pAlloc += 1; // first address holds count, second address is pointer to start of ZArray

			T* pHead = &r->a[uCount];

			if (r->a < pHead)
			{
				for (int i = 0; i < uCount; i++)
				{
					this->a[i] = T(r->a[i]); // copy constructors are required for classes to be used here
				}
			}
		}


		return this;
	}

	T& operator[](size_t i)
	{
		return this->a[i];
	}

	BOOL IsEmpty()
	{
		return this->GetCount() == 0;
	}

	T* Insert(T* e, int nIdx = -1)
	{
		T* result = this->InsertBefore(e); // TODO
		*result = *e; // operator= overloading is required to make it function as the PDB does
		return result;
	}

	T* InsertBefore(int nIdx = -1)
	{
		BOOL   bAllocateMoreMemory;
		size_t uSizeToAllocate;
		size_t uAllocationBytes;
		size_t uCount = this->GetCount();

		if (nIdx == -1) nIdx = uCount;

		/* Determine if more space is required to fit another T object into the array */
		if (this->a)
		{
			/* Grab size of allocation block -- remember, ZAllocEx encodes this at the head of each block */
			uAllocationBytes = reinterpret_cast<DWORD*>(this->a)[-2];

			if (uAllocationBytes > INT_MAX) // this means its negative since the datatype is unsigned
			{
				uAllocationBytes = ~uAllocationBytes;
			}
			/* If there is enough space is the allocation block for another object, do not allocate more memory */
			bAllocateMoreMemory = (uAllocationBytes - sizeof(PVOID)) / sizeof(T) <= uCount;
		}
		else
		{
			bAllocateMoreMemory = TRUE;
		}

		if (bAllocateMoreMemory)
		{
			if (uCount)
			{
				/* Always double the existing array size so we don't have to allocate mem as often */
				uSizeToAllocate = 2 * uCount;
			}
			else
			{
				uSizeToAllocate = 1;
			}

			this->Reserve(uSizeToAllocate);
		}

		/* Increase array count by one */
		size_t* pCount = &reinterpret_cast<size_t*>(this->a)[-1];
		*pCount += 1;

		T* pDest = &this->a[nIdx + 1];
		T* pSrc = &this->a[nIdx];
		size_t uSize = sizeof(T) * (uCount - nIdx);

		/* Shift memory to make space for the new object */
		memmove(pDest, pSrc, uSize);

		/* Initialize new memory space with T constructor */
		this->a[nIdx] = T();

		/* Return pointer to new object */
		return &this->a[nIdx];
	}

	void RemoveAt(size_t nIdx)
	{
		this->RemoveAt(&this->a[nIdx]);
	}

	void RemoveAt(T* pos) // TODO test this
	{
		pos->~T();

		T* pItemToRemove = pos;
		T* pNextItem = pos + 1;

		size_t nItemIdx = this->IndexOf(pNextItem);
		size_t nItemsToMove = this->GetCount() - nItemIdx;
		size_t nBytesToMove = nItemsToMove * sizeof(T);

		memmove(pItemToRemove, pNextItem, nBytesToMove);

		size_t* pCount = &reinterpret_cast<size_t*>(this->a)[-1];
		*pCount -= 1;
	}

	/// <summary>
	/// Gets size of array.
	/// </summary>
	size_t GetCount()
	{
		if (this->a)
		{
			size_t nCount = reinterpret_cast<size_t*>(this->a)[-1];
			return nCount;
		}
		else
		{
			return 0;
		}
	}

	/// <summary>
	/// Gets the index of the pointer passed into the function.
	/// Passing a pointer that does not exist in the array will result in undefined behavior.
	/// </summary>
	UINT IndexOf(T* pos)
	{
		return pos - this->a; // compiler automatically does the math here so all we need to write is the subtraction
	}

	/// <summary>
	/// Gets the next pointer in the array sequence, or nullptr if there is no next item.
	/// The return value is the original pointer passed into the pos parameter, and the pos
	/// parameter will contain the pointer to the next value.
	/// </summary>
	T* GetNext(T** pos)
	{
		T* result = *pos;

		*pos = *pos > this->a ? &result[-1] : nullptr;

		return result;
	}

	/// <summary>
	/// Gets the previous pointer in the array sequence, or nullptr if there is no previous item.
	/// The return value is the original pointer passed into the pos parameter, and the pos
	/// parameter will contain the pointer to the previous value.
	/// </summary>
	T* GetPrev(T** pos)
	{
		T* result = *pos;

		/* Highest index is array size - 1 */
		size_t nIndex = reinterpret_cast<size_t*>(this->a)[-1];
		nIndex -= 1;

		if (*pos < &this->a[nIndex])
		{
			*pos = result + 1;
		}
		else
		{
			*pos = nullptr;
		}

		return result;
	}

	/// <summary>
	/// Fetches a pointer to the value on the top of the array stack.
	/// </summary>
	T* GetHeadPosition()
	{
		if (this->a)
		{
			size_t nCount = reinterpret_cast<size_t*>(this->a)[-1];
			return &this->a[nCount - 1];
		}
		else
		{
			return nullptr;
		}
	}

	/// <summary>
	/// Removes all items from the array and calls their destructors.
	/// </summary>
	void RemoveAll()
	{
		if (this->a)
		{
			/* Get pointer to allocation base  (array base - 4 bytes) */
			DWORD* pAllocationBasePointer = &reinterpret_cast<DWORD*>(this->a)[-1];
			size_t nMaxIndex = *pAllocationBasePointer - 1;

			/* Call destructor  */
			this->Destroy(this->a, &this->a[nMaxIndex]);

			/* Free array allocation */
			ZAllocEx<ZAllocAnonSelector>::GetInstance()->Free((void**)pAllocationBasePointer);
			this->a = nullptr;
		}
	}

private:
	static void Construct(T* start, T* end)
	{
		for (T* i = start; i < end; i++)
		{
			i = T();
		}
	}

	static void Destroy(T* start, T* end)
	{
		for (T* i = start; i < end; i++)
		{
			i->~T();
		}
	}

	void Alloc(size_t uSize)
	{
		this->RemoveAll();

		if (!uSize) return;

		/* Allocate Desired Array Size + 4 bytes */
		/* We casting to a dword so we can write and adjust the pointer easier */
		DWORD* pAlloc = (DWORD*)ZAllocEx<ZAllocAnonSelector>::GetInstance()->Alloc(sizeof(T) * uSize + sizeof(PVOID));

		/* Assign number of array items to array head */
		*pAlloc = uSize;

		/* Assign start of real allocated block to array pointer */
		/* We take index 1 because index zero is the array item count */
		this->a = reinterpret_cast<T*>(pAlloc + 1);
	}

	void Realloc(size_t uNewArraySize, int nMode) // Has an extra arg (ZAllocHelper*) that may be used for memory profiling debug builds
	{
		size_t uCurArraySize, // current number of max items
			uMaxCountInAllocBlock; // max items that will fit inside of the allocated memory block
		/* Sometimes the above numbers are not the same because memory blocks are often larger than the requested array size */

		uCurArraySize = this->GetCount();

		if (uNewArraySize > uCurArraySize)
		{
			if (this->a)
			{
				/* Grab size of allocation block -- remember, ZAllocEx encodes this at the head of each block */
				uMaxCountInAllocBlock = reinterpret_cast<DWORD*>(this->a)[-2];

				if (uMaxCountInAllocBlock > INT_MAX) // this means its negative since the datatype is unsigned
				{
					uMaxCountInAllocBlock = ~uMaxCountInAllocBlock;
				}

				/* Determine the real number of array item slots based on the allocation block header */
				uMaxCountInAllocBlock = (uMaxCountInAllocBlock - 4) / sizeof(T);
			}
			else
			{
				uMaxCountInAllocBlock = 0;
			}

			/* If the real size of the existing allocation block fits the desired number of array items, do nothing, else allocate new memory */
			if (uNewArraySize > uMaxCountInAllocBlock)
			{
				/* Allocated new block */
				PVOID pNewAllocationBase = ZAllocEx<ZAllocAnonSelector>::GetInstance()->Alloc(sizeof(T) * uNewArraySize + sizeof(PVOID));

				/* Increment allocation base to save a spot for the array size encoding */
				pNewAllocationBase += 1;

				if ((nMode & 2) == 0)
				{
					/* Initialize all objects between the current array size and the increased array size in the new block */
					this->Construct(&pNewAllocationBase[uCurArraySize], &pNewAllocationBase[uNewArraySize]);
				}

				if (this->a)
				{
					if ((nMode & 1) == 0)
					{
						/* Copy existing array items into new allocation */
						memcpy(pNewAllocationBase, this->a, sizeof(T) * uCurArraySize);
					}

					/* Free old memory allocation */
					void** pCurrentAllocationBase = reinterpret_cast<void**>(this->a) - 1;
					ZAllocEx<ZAllocAnonSelector>::GetInstance()->Free(pCurrentAllocationBase);
				}
				this->a = pNewAllocationBase;
			}
			else if ((nMode & 2) == 0)
			{
				/* Initialize all objects between the current array size and the increased array size in the same block */
				this->Construct(&this->a[uCurArraySize], &this->a[uNewArraySize]);
			}
		}
		else
		{
			/* Destroy all objects between the new count and the old count */
			this->Destroy(&this->a[uNewArraySize], &this->a[uCurArraySize]);
		}

		if (this->a)
		{
			/* Reassign value at array size pointer to match new size */
			DWORD* pdwArraySize = &reinterpret_cast<DWORD*>(this->a)[-1];
			*pdwArraySize = uNewArraySize;
		}
	}

	void Reserve(size_t uItems)
	{
		size_t uCurArraySize;
		size_t uAllocationSize;
		size_t uMaxCountInAllocBlock;

		if (this->a)
		{
			/* Grab size of allocation block -- remember, ZAllocEx encodes this at the head of each block */
			uAllocationSize = reinterpret_cast<DWORD*>(this->a)[-2];

			if (uAllocationSize > INT_MAX) // this means its negative since the datatype is unsigned
			{
				uAllocationSize = ~uAllocationSize;
			}

			/* Determine the real number of array item slots based on the allocation block header */
			uMaxCountInAllocBlock = (uAllocationSize - sizeof(PVOID)) / sizeof(T);
		}
		else
		{
			uMaxCountInAllocBlock = 0;
		}

		if (uMaxCountInAllocBlock == uItems) return;

		uCurArraySize = this->GetCount();

		/* Allocate new block */
		DWORD* pNewAllocationBase = (DWORD*)ZAllocEx<ZAllocAnonSelector>::GetInstance()->Alloc(sizeof(T) * uItems + sizeof(PVOID));

		/* Encode new array size at allocation base */
		*pNewAllocationBase = uCurArraySize;

		/* Increment allocation base */
		pNewAllocationBase += 1;

		if (this->a)
		{
			/* Copy existing memory into the new allocation */
			memcpy(pNewAllocationBase, this->a, sizeof(T) * uCurArraySize);

			/* Free old memory allocation */
			void** pCurrentAllocationBase = &reinterpret_cast<void**>(this->a)[-1];
			ZAllocEx<ZAllocAnonSelector>::GetInstance()->Free(pCurrentAllocationBase);
		}

		this->a = reinterpret_cast<T*>(pNewAllocationBase);

		/* Reassign value at array size pointer to match new size */
		/*DWORD* pdwArraySize = &reinterpret_cast<DWORD*>(this->a)[-1];
		*pdwArraySize = uCurArraySize;*/
	}
};

assert_size(sizeof(ZArray<int>), 0x04)