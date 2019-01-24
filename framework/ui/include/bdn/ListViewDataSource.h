#pragma once

#include <string>

namespace bdn
{
	class ListViewDataSource
	{
	public:
		virtual ~ListViewDataSource() = default;

		virtual size_t numberOfRows() = 0;
		virtual String labelTextForRowIndex(size_t rowIndex) = 0;
	};
}