#include "equality_comparable.h"

bool operator==(equality_comparable const& lhs, equality_comparable const& rhs)
{
	return lhs.impl_->equals(*rhs.impl_);
}

bool operator!=(equality_comparable const& lhs, equality_comparable const& rhs)
{
	return !(lhs == rhs);
}

equality_comparable::impl_base::~impl_base()
{
}