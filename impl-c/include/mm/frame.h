#pragma once

#define FRAME_SHIFT 12 // 4Kb
#define FRAME_ADDR_BASE (1 << FRAME_SHIFT)
#define FRAME_SIZE (1 << FRAME_SHIFT)

// Mask for address inside frame
#define FRAME_MASK ((1 << FRAME_SHIFT) - 1)
