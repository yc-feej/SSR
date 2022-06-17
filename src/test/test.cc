// Copyright 2020 Deeproute.ai. All Rights Reserved.
// Author: Yuchen Liu (yuchenliu@deeproute.ai)
#include "src/test/test.h"

ADD_REFLECTION_CLASS_MEMBER(ReflectionTestBase, val1_, val2_, val3_, val4_,
                            val5_, val6_, val7_);

ADD_REFLECTION_CLASS_MEMBER(ReflectionTestDerive, val1_, val2_, val3_, val4_,
                            val5_, val6_, val7_, val8_);

ADD_REFLECTION_CLASS_MEMBER(ReflectionTestPtr, val1_, val2_, val3_);
