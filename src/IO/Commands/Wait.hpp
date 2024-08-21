#pragma once

#include <iosfwd>
#include <cstdint>

namespace sw::io
{
	struct Wait
	{
		constexpr static const char* Name = "WAIT";

		uint32_t ticks {};

		template <typename Visitor>
		void visit(Visitor& visitor)
		{
			visitor.visit("ticks", ticks);
		}
	};
}