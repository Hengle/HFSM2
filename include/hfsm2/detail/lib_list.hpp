#pragma once

namespace hfsm2 {
namespace detail {

////////////////////////////////////////////////////////////////////////////////

template <typename TItem, LongIndex NCapacity>
class List {
public:
	using Index = LongIndex;

	static constexpr Index CAPACITY = NCapacity;

	static constexpr Index INVALID = Index (-1);

private:
	using Item  = TItem;

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	struct Links {
		Index prev;
		Index next;
	};

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	union Cell {
		Item item;
		Links links;

		HSFM_INLINE Cell()
			: links{}
		{}
	};

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

public:
	template <typename... TArgs>
	Index emplace(TArgs... args);

	void remove(const Index i);

	HSFM_INLINE		  Item& operator[] (const Index i);
	HSFM_INLINE const Item& operator[] (const Index i) const;

	HSFM_INLINE Index count() const { return _count; }

private:
	HSFM_IF_ASSERT(void verifyStructure(const Index occupied = INVALID) const);

private:
	Cell _cells[CAPACITY];
	Index _vacantHead = 0;
	Index _vacantTail = 0;
	Index _last = 0;
	Index _count = 0;
};

////////////////////////////////////////////////////////////////////////////////

}
}

#include "lib_list.inl"
