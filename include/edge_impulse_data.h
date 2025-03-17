#ifndef EDGE_IMPULSE_DATA_H
#define EDGE_IMPULSE_DATA_H

// Notice we do NOT include "metadata.h" here!

// 1) Declare an external constant for the frame size.
extern const int EI_FRAME_SIZE;

// 2) Externally declare your raw buffer (no size).
extern float raw_data_buffer[];

#endif // EDGE_IMPULSE_DATA_H
